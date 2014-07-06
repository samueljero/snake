#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <limits.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>

#include <algorithm>
#include <thread>

#include <ext/stdio_filebuf.h>
#include <fstream>
#include<boost/tokenizer.hpp>

#include "log.h"
#include "gc.h"

int turretInstanceId = 0;
char *strategyComposeScript = NULL;
int quit = 0;

bool strCmp (const strategy& first, const strategy& second)
{
      unsigned int i=0;
      return first.weight < second.weight;
}

bool strategy::operator ==(const strategy &s2) {
    if (content.compare(s2.content) == 0) return true;
    return false;
}

bool strategy::operator<(const strategy &s2) {
    return (weight < s2.weight);
}

void gc::reportCollector() {
    int sd;
    struct sockaddr_in serv_addr;
    int t = 1;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t));

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(MAINGC_PORT);

    if (bind(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        perror("Failed to bind\n");
    listen(sd, 10);

    LOG(INFO) << "listening... on " << ntohs(serv_addr.sin_port);
    while(quit != 1) {
        struct sockaddr_in turret_addr;
        socklen_t sock_len;
        int sock = accept(sd,(struct sockaddr*) &turret_addr, &sock_len);
        if (sock <= 0) {
            perror("accept fail\n");
        } else {
            // message format - IP:PORT:MSG
            LOG(DEBUG) << "accept";
            char buff[1025];
            memset(buff, 0, 1024);
            int len = read (sock, buff, 1024);
            int len2 = 0;
            struct sockaddr_in dest;
            TurretInstance *ti = NULL;

            // Get addr and port
            // format ready:IP:port 
            char *ip = strtok(buff, ":");
            char *pch = strtok(NULL, ":");
            int port = atoi(pch);
            char *msg = strtok(NULL, "\n");
            LOG(DEBUG) << "rcvd from IP: " << ip << " PORT: " << port << " MSG: " << msg;

            memset(&dest, 0, sizeof(struct sockaddr_in));
            inet_pton(AF_INET, ip, &(dest.sin_addr));
            dest.sin_family = AF_INET;
            dest.sin_port = htons(port);

            // check if the instance already exists
            for (std::list<TurretInstance>::iterator it=turretIList.begin(); it!= turretIList.end(); it++) {
                ti = &(*it);
                if (ti->ifMatch(&dest)) { // found the instance
                    break;
                }
                ti = NULL;
            }

            // ***********************
            // lines start with keyword
            // ready: a turret instance reports it is ready to accept new strategies
            // perf: a turret instance reports performance for an execution
            // info: a turret instance reports additional information about an execution
            // ***********************
            
            if (ti != NULL) { // found the instance
                if (startsWith(msg, "perf") == 0) {
                    ti->status = TurretInstance::Status::reporting;
                    updatePerformance(ti, msg);
                }
                else if (startsWith(msg, "info") == 0) {
                    LOG(DEBUG) << "feedback " << msg;
                    // info:end :: reporting over
                    ti->status = TurretInstance::Status::reporting;
                    if (startsWith(msg, "info:end") == 0) {
                        pthread_cond_signal(&strategy_cond);
                    } 
                    else {
                        // info:key:value
                        //boost::char_separator<char> sep(":");
                        //boost::tokenizer< boost::char_separator<char> > tok(msg, sep);
                        //boost::tokenizer< boost::char_separator<char> >::iterator it=tok.begin();
                        char *info = strtok(msg, ":");
                        char *key = strtok(NULL, ":");
                        char *value = strtok(NULL, ":");
                        if (prev_feedback.find(key) != prev_feedback.end()) {
                            prev_feedback[key]++;
                        } else {
                            prev_feedback[key] = 0;
                        }
                        LOG(DEBUG) << "KEY: " << key << " VALUE: " << value << " HISTORY: " << prev_feedback[key];
                        std::string feedback_line = key;
                        feedback_line += ",";
                        feedback_line += value;
                        feedback_line += "," + std::to_string((long long)prev_feedback[key]);
                        feedback_line += "\n";

                        feedback.push_back(feedback_line);
                    }
                }
                else if (startsWith(msg, "ready") == 0) {
                    LOG(DEBUG) << "ready " << msg << " from TI " << *ti;
                    pthread_mutex_lock(&turret_mutex);
                    switch(ti->status) {
                        case TurretInstance::Status::ready: 
                            pthread_cond_signal(&distributor_cond);
                            break;
                        case TurretInstance::Status::sending:
                            LOG(DEBUG) << "We are still sending";
                            break;
                        case TurretInstance::Status::running:
                            LOG(DEBUG) << "TI was already runing. resubmitting the same strategy";
                            ti->sendMessage(ti->curStrategy->content.c_str(), strlen(ti->curStrategy->content.c_str()));
                            ti->status = TurretInstance::Status::running;
                            break;
                        case TurretInstance::Status::error:
                            ti->status = TurretInstance::Status::ready; 
                            pthread_cond_signal(&distributor_cond);
                            break;
                        case TurretInstance::Status::reporting:
                            ti->status = TurretInstance::Status::ready;
                            pthread_cond_signal(&distributor_cond);
                            break;
                        default:
                            ti->status = TurretInstance::Status::ready;
                            pthread_cond_signal(&distributor_cond);
                            break;
                    }
                    pthread_mutex_unlock(&turret_mutex);
                }
                else {
                    LOG(ERROR) << "received " << buff << " from Turret " << inet_ntoa(dest.sin_addr);
                }
            }
            else {
                // didn't find the instance
                if (startsWith(msg, "ready") == 0) { 
                    // add new turret instance
                    TurretInstance ti_new = TurretInstance(&dest, port, TurretInstance::ready);
                    LOG(INFO) << "New Turret Instance Added: " << inet_ntoa(dest.sin_addr) << ":" << ntohs(dest.sin_port) <<"   ID: " << ti_new.id;
                    turretIList.push_front(ti_new);
                    pthread_cond_signal(&distributor_cond);
                }
                else {
                    LOG(ERROR) << "rcvd " << buff << " from unknown";
                }
            }

            // XXX - to decide ping back
            while (len-len2 > 0) {
                 int cur = write(sock, buff + len2, len - len2);
                 if (cur <= 0) {
                     LOG(DEBUG) << "Fail to write to " << inet_ntoa(dest.sin_addr);
                     break;
                 }
                 len2 += cur;
            }
        }
        close(sock);
    }
    LOG(DEBUG) << "stopping reportCollector: " << quit;
    close(sd); 
}


// pipe with perl scripts: plug-in algorithm
void gc::strategyComposer() {
    // XXX load from saved
    if (waitingStrategy.empty()) { // put benign
        std::string line = "";
        expanding = true;
        pthread_mutex_lock(&strategy_mutex);
        executePipeToFillWaitingStrategy(line);
        pthread_mutex_unlock(&strategy_mutex);
        expanding = false;
    }
    while (!quit) {
        pthread_mutex_lock(&strategy_mutex);
        while (performanceResult.empty()) {
            if (quit) break;
            pthread_cond_wait(&strategy_cond, &strategy_mutex);
        }
        if (!performanceResult.empty()) {
            LOG(DEBUG) << "strategyComposer";
            std::string line = performanceResult.front();
            performanceResult.pop();
            expanding = true;
            //pthread_mutex_unlock(&strategy_mutex);
            LOG(INFO) << "Looking for more Strategies...";
            executePipeToFillWaitingStrategy(line);

            //pthread_mutex_lock(&strategy_mutex);
            expanding = false;
            pthread_cond_signal(&distributor_cond);
        } 
        pthread_mutex_unlock(&strategy_mutex);
    }
}

// send feedback available
void gc::send_feedback(int fd) 
{
    while (!feedback.empty()) {
        std::string line = feedback.front();
        feedback.pop_front();
        //= *it;
        if (int n = write(fd, line.c_str(), line.size()) < line.size()) {
            perror("fail to write pipe");
        } 
        LOG(DEBUG) << "WROTE FEEDBACK " << line;
    }
    if (int n = write(fd, "END", 3) < 3) {
        perror("fail to write pipe");
    } 
}

int gc::executePipeToFillWaitingStrategy(std::string &line) 
{
    #define PREAD 0
    #define PWRITE 1
    // main algorithm
    int pipe_p2c[2], pipe_c2p[2];

    if (pipe(pipe_p2c) != 0) {
        perror("pipe p2c fail\n");
    }
    if (pipe(pipe_c2p) != 0) {
        perror("pipe c2p fail\n");
    }

    pid_t pid = fork();
    if (pid == (pid_t)0) {
        if (dup2(pipe_p2c[PREAD], STDIN_FILENO) != 0 || close(pipe_p2c[PREAD]) != 0
                || close(pipe_p2c[PWRITE]) != 0) {
            perror("dup STDIN failed\n");
        }
        if (dup2(pipe_c2p[PWRITE], STDOUT_FILENO) != 1 || close(pipe_c2p[PWRITE]) != 0
                || close(pipe_c2p[0]) != 0) {
            perror("dup STDOUT failed\n");
        }

        int nres = execl(strategyComposeScript, strategyComposeScript, line.c_str(), NULL);
        if (nres < 0)
            perror("child");

        exit(nres);
    }
    else {
        char buf[1024];
        int n;
        std::string resultStr = "";

        close(pipe_p2c[PREAD]);
        close(pipe_c2p[PWRITE]);

        send_feedback(pipe_p2c[1]);
        close(pipe_p2c[PWRITE]);
        LOG(DEBUG) << "write " << n;

        LOG(DEBUG) << "start piping <-- ";
        __gnu_cxx::stdio_filebuf<char> filebuf(pipe_c2p[PREAD], std::ios::in); // 1
        std::istream is(&filebuf);
        std::string readline = "";
        // the format of each line should be weight:strategy
        while ( std::getline(is, readline)) {
            int weight;
            boost::char_separator<char> sep(":");
            boost::tokenizer< boost::char_separator<char> > tok(readline, sep);
            boost::tokenizer< boost::char_separator<char> >::iterator it=tok.begin();
                weight = atoi((*it++).c_str());
            if (it != tok.end()) {
                strategy str = strategy(weight, *it);
                waitingStrategy.push_front(str);
                LOG(DEBUG) << "ADD: " << str.content;
            }
        }
        waitingStrategy.sort(strCmp);
        for(std::list<strategy>::iterator it = waitingStrategy.begin(); it != waitingStrategy.end(); it++) {
            LOG(DEBUG) << "sorted: " << (*it).weight << " - " << (*it).content;
        }
        LOG(DEBUG) << "reading done";
        close(pipe_c2p[PREAD]);
    }
}

bool gc::ready_to_send() {
    if (waitingStrategy.empty()) {
        LOG(DEBUG) << "no waitingStrategy available";
        return false;
    }
    if (nextAvailableTurret() == NULL) {
        LOG(DEBUG) << "no availableTurret";
        return false;
    }
    return true;
}

TurretInstance * gc::nextAvailableTurret() {
    TurretInstance *ti  = NULL;
    for (std::list<TurretInstance>::iterator it=turretIList.begin(); it!= turretIList.end(); it++) {
        ti = &(*it);
        if (ti->status == TurretInstance::Status::ready) return ti;
    }
    return NULL;
}

void TurretInstance::sendMessage(const char *message, size_t message_len) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    status = TurretInstance::Status::sending; 
    if (sockfd < 0) perror("Error opening socket");
    LOG(DEBUG) << "connecting TurretInstance " << id << " addr: " << inet_ntoa(addr.sin_addr) << " port: " << ntohs(addr.sin_port);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Error connecting Turret");
        LOG(ERROR) << "can't connect turret : " << id;
    }
    else {
        LOG(DEBUG) << "will write: " << message;
        int n = write(sockfd, message, message_len+1);
        LOG(DEBUG) << "wrote " << n;
        // XXX maybe read back?
    }
    close(sockfd);
}

bool gc::ready_to_stop() {
    if (!waitingStrategy.empty()) {
        return false;
    }
    if (expanding) {
        return false;
    }
    pthread_mutex_lock(&turret_mutex);
    TurretInstance *ti  = NULL;
    for (std::list<TurretInstance>::iterator it=turretIList.begin(); it!= turretIList.end(); it++) {
        ti = &(*it);
        if (ti->status == TurretInstance::Status::running ||
                ti->status == TurretInstance::Status::reporting ||
                ti->status == TurretInstance::Status::sending) {
            pthread_mutex_unlock(&turret_mutex);
            return false;
        }
    }
    pthread_mutex_unlock(&turret_mutex);
    return true;
}

void gc::distributor() {
    while (!quit) {
        pthread_mutex_lock(&turret_mutex);
        while (!ready_to_send())
        {
            if (quit) break;
            pthread_cond_wait(&distributor_cond, &turret_mutex);
        }
        if (ready_to_send()) {

            strategy nextStr = waitingStrategy.front();
            LOG(DEBUG) << "before sending: we have " << waitingStrategy.size() << " strategies";
            //waitingStrategy.erase(waitingStrategy.begin());
            waitingStrategy.pop_front();
            LOG(DEBUG) << "next strategy: " << nextStr.content << " now we have " << waitingStrategy.size();

            TurretInstance *ti = nextAvailableTurret();
            ti->setCurStrategy(nextStr);
            ti->sendMessage(ti->curStrategy->content.c_str(), strlen(ti->curStrategy->content.c_str()));
            ti->status = TurretInstance::Status::running;
	    LOG(INFO) << "Sending Strategy to be Run on ID " << ti->id << ": " << nextStr.content;
            // XXX send the strategy to the instance

            pthread_cond_signal(&distributor_cond);
        }
        pthread_mutex_unlock(&turret_mutex);
    }
}

// update performance
void gc::updatePerformance(TurretInstance * ti, std::string perfString) {
    // XXX what if there's status mismatch?
    // update the strategy finished
    // signal appropriate threads and update this instance ready
    LOG(INFO) << "Received Performance Score from ID " << ti->id << ": " << perfString;
    performanceResult.push(perfString);
}

int main(int argc, char **argv)
{
    gc gc_instance = gc();
    strategyComposeScript = (char *)"./basic.pl";

    pthread_t thread;
    int t=1;
    quit = 0;

    // LOGGING
    FILELog::ReportingLevel() = FILELog::FromString("INFO");

    std::thread th_distributor(&gc::distributor, std::ref(gc_instance));
    std::thread th_strComposer(&gc::strategyComposer, std::ref(gc_instance));
    std::thread th_reportCollector(&gc::reportCollector, std::ref(gc_instance));
    sleep(1);
    while (1) {
        if (gc_instance.ready_to_stop() == true) {
            // IF READY TO STOP
            LOG(DEBUG) << "========= finish ============";
            TurretInstance *ti  = NULL;
            for (std::list<TurretInstance>::iterator it=gc_instance.turretIList.begin(); it!= gc_instance.turretIList.end(); it++)
            {
                // let turret instances know
                ti = &(*it);
                ti->sendMessage("STOP", 4);
            }
            quit = 1;
            break;
        }
    }

    LOG(DEBUG) << "waiting distributor to join";
    pthread_cond_signal(&distributor_cond);
    th_distributor.join();
    LOG(DEBUG) << "waiting strComposer to join";
    pthread_cond_signal(&strategy_cond);
    th_strComposer.join();

    exit(0);
}
