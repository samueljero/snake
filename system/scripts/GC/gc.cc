#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>

#include <algorithm>
#include <string>
#include <fstream>
#include <list>
#include "log.h"

using namespace std;

#define MAINGC_PORT 9991
const char *strategyComposeScript = "./basic.pl";
int turretInstanceId = 0;
int listen_socket;
int quit = 0;

class TurretInstance {
public:
	enum Status {
		ready, running, reporting, sending, error
	};
	struct sockaddr_in addr;
	int sin_port;
	int port;
	int id;
	Status status;
	string curStrategy;
	void setCurStrategy(string str) {
		curStrategy = str;
	}

	TurretInstance(struct sockaddr_in* addrP, int pt, Status st) {
		memcpy(&addr, addrP, sizeof(struct sockaddr_in));
		port = sin_port = pt;
		status = st;
		id = ++turretInstanceId;
		curStrategy = "";
	}

	bool ifMatch(struct sockaddr_in* addrP) {
		if (memcmp(&addr, addrP, sizeof(struct sockaddr_in)) == 0)
			return true;
		return false;
	}

	void sendMessage(const char * msg, size_t msg_len);

	friend std::ostream& operator<<(std::ostream& stream,
			const TurretInstance & ti) {
		stream << ti.id;
		return stream;
	}

};

void TurretInstance::sendMessage(const char *message, size_t message_len) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		perror("Error creating socket");
	}
	status = TurretInstance::sending;
	LOG(DEBUG) << "sending to TurretInstance " << id << " addr: "
			<< inet_ntoa(addr.sin_addr) << " port: " << ntohs(addr.sin_port);
	if (connect(sockfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0) {
		perror("Error connecting Turret");
	} else {
		if(write(sockfd, message, message_len)<0){
			perror("Write failed!");
		}
	}
	close(sockfd);
}

struct turret_thread_args{
	int sock;
	TurretInstance *turret;
};

list<list<string> > global_reports;
pthread_mutex_t reports_mutex;
pthread_cond_t reports_cond;
pthread_mutex_t reports_cond_mutex;

list<string> global_strategies;
pthread_mutex_t strategies_mutex;

list<TurretInstance> global_turrets;
pthread_mutex_t turrets_mutex;

int pipe_to_prog[2];
int pipe_from_prog[2];
#define PREAD 0
#define PWRITE 1

void handleMessage(TurretInstance *ti, string msg, list<string> *rep) {
#define startsWith(s1, s2) strncmp(s1, s2, strlen(s2))
	string tmp;
	// ***********************
	// lines start with keyword
	// ready: a turret instance reports it is ready to accept new strategies
	// perf: a turret instance reports performance for an execution
	// info: a turret instance reports additional information about an execution
	// ***********************
	if (startsWith(msg.c_str(), "perf") == 0) {
		LOG(INFO) << "Turret " << *ti <<": Performance report: " << msg;
		ti->status = TurretInstance::reporting;
		rep->push_back(msg);
	} else if (startsWith(msg.c_str(), "info") == 0) {
		LOG(DEBUG) << "feedback " << msg;
		ti->status = TurretInstance::reporting;

		if (startsWith(msg.c_str(), "info:end") == 0) {
			LOG(INFO) << "Turret " << *ti <<": Feedback report";
			rep->push_back(msg);
			pthread_mutex_lock(&reports_mutex);
			global_reports.push_back(*rep);
			pthread_mutex_unlock(&reports_mutex);
			pthread_cond_signal(&reports_cond);
			rep->clear();
		} else {
			rep->push_back(msg);
		}
	} else if (startsWith(msg.c_str(), "ready") == 0) {
		LOG(DEBUG) << "Turret " << *ti << ": Ready";
		switch (ti->status) {
		case TurretInstance::sending:
			LOG(DEBUG) << "We are still sending";
			break;
		case TurretInstance::running:
			LOG(INFO) << "Turret " << *ti <<" was already running. Resubmitting the previous strategy";
			ti->sendMessage(ti->curStrategy.c_str(), strlen(ti->curStrategy.c_str()));
			ti->status = TurretInstance::running;
			break;
		case TurretInstance::ready:
		case TurretInstance::error:
		case TurretInstance::reporting:
		default:
			ti->status = TurretInstance::ready;
			pthread_mutex_lock(&strategies_mutex);
			while(global_strategies.empty() && !quit){
				pthread_mutex_unlock(&strategies_mutex);
				sleep(1);
				pthread_mutex_lock(&strategies_mutex);
			}
			if(quit)
				break;
			tmp = global_strategies.front();
			global_strategies.pop_front();
			pthread_mutex_unlock(&strategies_mutex);
			LOG(INFO) << "Turret " << *ti <<": Starting new strategy: " << tmp;
			ti->setCurStrategy(tmp);
			ti->sendMessage(ti->curStrategy.c_str(), strlen(ti->curStrategy.c_str()));
			ti->status = TurretInstance::running;
			break;
		}
	} else {
		LOG(ERROR) << "received " << msg << " from Turret Instance " << *ti;
	}
}

void *turret_thread(void *args)
{
	struct turret_thread_args *inp = (struct turret_thread_args*) args;
	int sock = inp->sock;
	TurretInstance *ti = inp->turret;
	list<string> rep;
	string tmp;
	int len;
	FILE *f;
	size_t n;
	char *buff;

	free(args);

	LOG(DEBUG)<<"Start Turret Thread";

	f = fdopen(sock, "r");
	if(!f){
		perror("fdopen failed!");
		return NULL;
	}

	n = 1024;
	buff = (char*)malloc(n);
	if(!buff){
		perror("malloc failed!");
		return NULL;
	}

	while((len = getline(&buff,&n,f)) > 0){
		tmp = string(buff,len);
		handleMessage(ti,tmp, &rep);
	}


	free(buff);
	fclose(f);
	close(sock);

	LOG(DEBUG)<<"End Turret Thread";
	return NULL;
}

void *accept_thread(void *args)
{

	char buff[50];
	struct sockaddr_in serv_addr;
	struct sockaddr_in dest;
	struct sockaddr_in turret_addr;
	socklen_t sock_len;
	TurretInstance *ti = NULL;
	pthread_t new_thread;
	struct turret_thread_args *arg;
	int sock;
	int len;
	int port;
	char *ip,*pch;
	int t = 1;

	LOG(DEBUG) << "Start accept_thread()";

	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_socket < 0){
		perror("Failed to create socket!");
		quit = 1;
		return NULL;
	}
	if(setsockopt(listen_socket , SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t))<0){
		close(listen_socket);
		perror("Failed sockopt!");
		quit = 1;
		return NULL;
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(MAINGC_PORT);

	if (bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Failed bind()!");
		close(listen_socket);
		quit = 1;
		return NULL;
	}
	if(listen(listen_socket, 10)<0){
		perror("Failed listen()!");
		close(listen_socket);
		quit = 1;
		return NULL;
	}

	LOG(INFO) << "listening... on " << ntohs(serv_addr.sin_port);
	while (!quit) {
		sock = accept(listen_socket, (struct sockaddr*) &turret_addr, &sock_len);
		if (sock <= 0) {
			if(!quit)
				perror("Failed accept()");
			continue;
		}

		memset(buff, 0, 50);

		/*First read*/
		len = read(sock, buff, 50);
		if(len < 0){
			perror("Read failed!");
			close(sock);
			continue;
		}
		ip = strtok(buff, ":");
		pch = strtok(NULL, "\n");
		port = atoi(pch);
		LOG(DEBUG) << "connection from IP: " << ip << " PORT: " << port;

		/*Find turret instance*/
		memset(&dest, 0, sizeof(struct sockaddr_in));
		inet_pton(AF_INET, ip, &(dest.sin_addr));
		dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		pthread_mutex_lock(&turrets_mutex);
		for (list<TurretInstance>::iterator it = global_turrets.begin(); it != global_turrets.end(); it++) {
			ti = &(*it);
			if (ti->ifMatch(&dest)) { // found the instance
				break;
			}
			ti = NULL;
		}
		if (ti == NULL) {
			TurretInstance nti = TurretInstance(&dest, port, TurretInstance::ready);
			LOG(INFO) << "New Turret Instance Added: "
					<< inet_ntoa(dest.sin_addr) << ":" << ntohs(dest.sin_port)
					<< "   ID: " << nti.id;
			global_turrets.push_front(nti);
			ti = &(global_turrets.front());
		}
		pthread_mutex_unlock(&turrets_mutex);

		arg = (struct turret_thread_args *)malloc(sizeof(struct turret_thread_args));
		if(!arg){
			perror("malloc() failed!");
			continue;
		}
		arg->sock = sock;
		arg->turret = ti;
		if(pthread_create(&new_thread, NULL, turret_thread, arg)<0){
			free(arg);
			close(sock);
			perror("Failed to create thread!");
			continue;
		}

		if(pthread_detach(new_thread)<0){
			perror("Failed to detach thread");
			continue;
		}
	}
	close(listen_socket);

	LOG(DEBUG)<<"End accept_thread()";
	quit = 1;
	return NULL;
}

void *strategies_thread(void *args)
{
	string tmp;
	char *buff;
	size_t n;
	FILE *f;
	int len;

	LOG(DEBUG)<< "Start strategies_thread()";

	f = fdopen(pipe_from_prog[PREAD], "r");
	if(!f){
		perror("fdopen failed!");
		return NULL;
	}

	n = 1024;
	buff = (char*)malloc(n);
	if(!buff){
		perror("malloc failed!");
		return NULL;
	}

	while(!quit){
		len = getline(&buff,&n,f);
		if(len < 0){
			if(!feof(f)){ perror("getline failed!"); }
			break;
		}
		tmp = string(buff,len);
		LOG(INFO)<<"Adding new strategy: " << tmp;
		pthread_mutex_lock(&strategies_mutex);
		global_strategies.push_back(tmp);
		pthread_mutex_unlock(&strategies_mutex);
	}

	fclose(f);
	free(buff);
	LOG(DEBUG)<< "End strategies_thread()";
	quit = 1;
	return NULL;
}

void *reports_thread(void *args)
{
	list<string> rep;
	int ret;
	LOG(DEBUG)<< "Start reports_thread()";

	while(!quit){
		pthread_mutex_lock(&reports_cond_mutex);
		while(global_reports.empty() && !quit){
			pthread_cond_wait(&reports_cond, &reports_cond_mutex);
		}
		if(quit)
			break;
		pthread_mutex_unlock(&reports_cond_mutex);
		pthread_mutex_lock(&reports_mutex);
		rep = global_reports.front();
		global_reports.pop_front();
		pthread_mutex_unlock(&reports_mutex);

		LOG(DEBUG) << "Sending report to script";
		for(list<string>::iterator it=rep.begin(); it!=rep.end();it++){
			ret = write(pipe_to_prog[PWRITE],it->c_str(),it->length());
			if(ret <0){
				perror("Write() failed!");
				return NULL;
			}
		}
	}

	LOG(DEBUG)<< "End Reports_thread()";
	quit = 1;
	return NULL;
}

int do_exec_setup()
{
	if (pipe(pipe_from_prog) != 0) {
		perror("pipe from program failed");
	}
	if (pipe(pipe_to_prog) != 0) {
		perror("pipe to program failed");
	}

	LOG(DEBUG)<< "Begin Fork";
	pid_t pid = fork();
	if (pid == (pid_t) 0) {
		if (dup2(pipe_to_prog[PREAD], STDIN_FILENO) != 0
				|| close(pipe_to_prog[PREAD]) != 0
				|| close(pipe_to_prog[PWRITE]) != 0) {
			perror("dup STDIN failed");
		}
		if (dup2(pipe_from_prog[PWRITE], STDOUT_FILENO) != 1
				|| close(pipe_from_prog[PWRITE]) != 0
				|| close(pipe_from_prog[PREAD]) != 0) {
			perror("dup STDOUT failed");
		}

		int nres = execl(strategyComposeScript, strategyComposeScript, NULL);
		if (nres < 0)
			perror("child");
		exit(nres);
	} else {
		close(pipe_to_prog[PREAD]);
		close(pipe_from_prog[PWRITE]);
	}
	return 0;
}


int main(int argc, char **argv) {
	pthread_t accept_threadv;
	pthread_t strategies_threadv;
	pthread_t reports_threadv;
	quit = 0;

	pthread_cond_init(&reports_cond, NULL);
	pthread_mutex_init(&reports_cond_mutex,NULL);
	pthread_mutex_init(&reports_mutex,NULL);
	pthread_mutex_init(&strategies_mutex,NULL);
	pthread_mutex_init(&turrets_mutex,NULL);

	/* Don't keep around zombie processes for forked children */
	struct sigaction sigchld_action;
	sigchld_action.sa_handler = SIG_DFL, sigchld_action.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sigchld_action, NULL);

	/* Set logging level */
	FILELog::ReportingLevel() = FILELog::FromString("INFO");

	do_exec_setup();

	if(pthread_create(&accept_threadv,NULL, accept_thread,NULL)<0){
		LOG(ERROR)<< "Cannot create accept thread!";
		exit(1);
	}

	if(pthread_create(&strategies_threadv,NULL, strategies_thread,NULL)<0){
		LOG(ERROR)<< "Cannot create strategy generation thread!";
		exit(1);
	}

	if(pthread_create(&reports_threadv,NULL, reports_thread,NULL)<0){
		LOG(ERROR)<< "Cannot create report copy thread!";
		exit(1);
	}

	/* Sleep */
	sleep(10);
	while((!global_reports.empty() || !global_strategies.empty()) && !quit){
		sleep(1);
	}

	if(!global_strategies.empty()){
		LOG(WARNING)<< "Exiting with strategies still in queue!";
	}

	/* Exit */
	LOG(INFO)<< "Exiting";
	for(list<TurretInstance>::iterator it=global_turrets.begin(); it!=global_turrets.end(); it++){
		it->sendMessage("STOP", 4);
	}
	quit = 1;

	close(pipe_to_prog[PWRITE]);
	close(pipe_from_prog[PREAD]);
	close(listen_socket);
	pthread_cond_signal(&reports_cond);

	pthread_join(strategies_threadv,NULL);
	pthread_join(accept_threadv,NULL);
	pthread_join(reports_threadv,NULL);
	return 0;
}
