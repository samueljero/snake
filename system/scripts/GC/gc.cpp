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


#include "log.h"
#include "gc.h"

int turretInstanceId = 0;
char *strategyComposeScript = NULL;

bool strategy::operator ==(const strategy &s2) {
    if (content.compare(s2.content) == 0) return true;
    return false;
}

bool strategy::operator<(const strategy &s2) {
    return (weight < s2.weight);
}

int quit = 0;

void gc::perfCollector() {
    int sd;
    struct sockaddr_in serv_addr;
    int t = 1;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t));

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(9999);

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
            LOG(DEBUG) << "accept";
            char buff[1025];
            int len = read (sock, buff, 1024);
            struct sockaddr peer;
            socklen_t peer_len;
            // XXX getpeer didn't work
            if (getpeername(sock, &peer, &peer_len) == -1) {
                perror("getpeername doesn't work\n");
            }
            LOG(DEBUG) << "read " << len << " bytes " << buff;
            int len2 = 0;
            struct sockaddr_in *dest = (struct sockaddr_in *) &peer;
            TurretInstance *ti = NULL;

            for (std::list<TurretInstance>::iterator it=turretInstances.begin(); it!= turretInstances.end(); it++) {
                ti = &(*it);
                if (ti->ifMatch(dest)) {
                    // found the instance
                    if(startsWith(buff, "perf")) {
                        LOG(DEBUG) << "match!";
                        pthread_cond_signal(&strategy_cond);
                        break;
                    }
                }
                ti = NULL;
            }
            if (ti == NULL && startsWith(buff, "ready") == 0) { // add to turret instance
                // format ready:IP:port // XXX here I am
                char *pch = strtok(buff, ":");
                pch = strtok(NULL, ":");
                char *ip = strdup(pch);
                pch = strtok(NULL, ":");
                int port = atoi(pch);
                inet_pton(AF_INET, ip, &(dest->sin_addr));
                dest->sin_family = AF_INET;
                dest->sin_port = htons(port);
                TurretInstance ti_new = TurretInstance(dest, port, TurretInstance::ready);
                LOG(DEBUG) << " new turret instance added: " << inet_ntoa(dest->sin_addr) << ":" << ntohs(dest->sin_port) <<" id" << ti_new.id;
                turretInstances.push_front(ti_new);
                availableTurret++;
                pthread_cond_signal(&distributor_cond);
            }
            if (ti != NULL) {
                LOG(ERROR) << "received " << buff << " from unkown Turret " << inet_ntoa(dest->sin_addr);
            }

            // XXX - to decide ping back
            while (len-len2 > 0) {
                 int cur = write(sock, buff + len2, len - len2);
                 if (cur <= 0) {
                     LOG(INFO) << "Fail to write to " << inet_ntoa(dest->sin_addr);
                     break;
                 }
                 len2 += cur;
            }
        }
        close(sock);
    }
    LOG(DEBUG) << "stopping perfCollector: " << quit;
    close(sd); 
}

// pipe with perl scripts: plug-in algorithm
void gc::strategyComposer() {
    // XXX load from saved
    if (waitingStrategy.empty()) { // put benign
        strategy benign = strategy("BENIGN", 0);
        waitingStrategy.push_front(benign);
    }
    while (!quit) {
        pthread_mutex_lock(&strategy_mutex);
        while (performanceResult.empty()) {
            pthread_cond_wait(&strategy_cond, &strategy_mutex);
        }
        if (!performanceResult.empty()) {
            LOG(DEBUG) << "strategyComposer";
            std::string line = performanceResult.front();
            performanceResult.pop();
            expanding = true;
            pthread_mutex_unlock(&strategy_mutex);

            #define PREAD 0
            #define PWRITE 1
            // main algorithm
            int inpipe[2], outpipe[2];

            pipe(inpipe);
            pipe(outpipe);

            pid_t pid = fork();
            if (pid == (pid_t)0) {
                dup2(inpipe[PREAD], STDIN_FILENO);
                dup2(outpipe[PWRITE], STDOUT_FILENO);

                close(inpipe[PREAD]);
                close(inpipe[PWRITE]);
                close(outpipe[PREAD]); 
                close(outpipe[PWRITE]); 
                LOG(DEBUG) << "execl: " << line;
                int nres = execl(strategyComposeScript, strategyComposeScript, line.c_str(), NULL);
                perror("child");
                exit(nres);
            }
            else {
                char buf[1024];
                std::string resultStr = "";
                close(inpipe[PREAD]);
                close(outpipe[PWRITE]);

                //int n = write(inpipe[PWRITE], "test", 5);
                //LOG(DEBUG) << n << "bytes wrote";
                int len = 0;
                while (1) {
                    int n = read(outpipe[PREAD], buf, 102);
                    if (n <= 0) break;
                    resultStr.append(buf, n);
                    len+=n;
                }
                LOG(DEBUG) << len << "bytes read " << resultStr;
                close(inpipe[PWRITE]);
                close(outpipe[PREAD]);
            }

            pthread_mutex_lock(&strategy_mutex);
            expanding = false;
        } 
        pthread_mutex_unlock(&strategy_mutex);
    }
}

bool gc::ready_to_send() {
    if (waitingStrategy.empty()) return false;
    if (availableTurret == 0) return false;
    return true;
}

TurretInstance * gc::nextAvailableTurret() {
    TurretInstance *ti  = NULL;
    for (std::list<TurretInstance>::iterator it=turretInstances.begin(); it!= turretInstances.end(); it++) {
        ti = &(*it);
        if (ti->status == TurretInstance::Status::ready) return ti;
    }
    return NULL;
}

void gc::distributor() {
    while (!quit) {
        pthread_mutex_lock(&strategy_mutex);
        while (!ready_to_send())
        {
            pthread_cond_wait(&distributor_cond, &strategy_mutex);
        }
        if (ready_to_send()) {
            strategy nextStr = waitingStrategy.front();
            waitingStrategy.erase(waitingStrategy.begin());

            TurretInstance *ti = nextAvailableTurret();
            ti->status = TurretInstance::Status::running;
            runningTurretCnt++;
            pthread_mutex_unlock(&strategy_mutex);

            // XXX send the strategy to the instance
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) perror("Error opening socket");
            LOG(DEBUG) << "connecting TurretInstance " << ti->id << " addr: " << inet_ntoa(ti->addr.sin_addr) << " port: " << ntohs(ti->addr.sin_port);
            if (connect(sockfd, (struct sockaddr*)&ti->addr, sizeof(struct sockaddr_in)) < 0)
                perror("Error connecting Turret");
            else {
                LOG(DEBUG) << "will writte: " << nextStr.content.c_str();
                int n = write(sockfd, nextStr.content.c_str(), strlen(nextStr.content.c_str())+1);
                LOG(DEBUG) << "wrote " << n;
                // XXX maybe read back?
            }
            close(sockfd);

            pthread_mutex_lock(&strategy_mutex);
            // XXX status update
            runningTurretCnt++;
            performanceResult.push("test string");
            pthread_cond_signal(&strategy_cond);
            LOG(DEBUG) << "performanceResult added" << performanceResult.size();
            availableTurret--;
        }
        pthread_mutex_unlock(&strategy_mutex);
    }
}

bool gc::finishCondition() {
    if (waitingStrategy.empty() && runningTurretCnt == 0 && expanding == false) return true;
    return false;
}

int main(int argc, char **argv)
{
    gc gc_instance = gc();
    strategyComposeScript = (char *)"./test.pl";

    pthread_t thread;
    int t=1;
    quit = 0;

    // LOGGING
    FILELog::ReportingLevel() = FILELog::FromString("DEBUG");

    std::thread th_distributor(&gc::distributor, std::ref(gc_instance));
    std::thread th_strComposer(&gc::strategyComposer, std::ref(gc_instance));
    std::thread th_perfCollector(&gc::perfCollector, std::ref(gc_instance));
    sleep(1);
    while (1) {
        if (gc_instance.finishCondition() == true) {
            LOG(DEBUG) << "finish";
            quit = 1;
            break;
        }
    }

    th_distributor.join();
    th_perfCollector.join();
    th_strComposer.join();

    return 0;
}
