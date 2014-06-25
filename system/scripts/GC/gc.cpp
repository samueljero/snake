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

int quit = 0;

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
                    updatePerformance(ti, msg);
                }
                else if (startsWith(msg, "ready") == 0) {
                    LOG(DEBUG) << "ready " << msg;
                    switch(ti->status) {
                        case TurretInstance::Status::ready:
                            pthread_cond_signal(&distributor_cond);
                            break;
                        case TurretInstance::Status::running:
                            ti->status = TurretInstance::Status::ready;
                            ti->sendStrategy(); // repeat the previous 
                            break;
                        case TurretInstance::Status::error:
                            ti->status = TurretInstance::Status::ready;
                            availableTurret++;
                            pthread_cond_signal(&distributor_cond);
                            break;
                        default:
                            ti->status = TurretInstance::Status::ready;
                            availableTurret++;
                            pthread_cond_signal(&distributor_cond);
                            break;
                    }
                } else {
                    LOG(ERROR) << "received " << buff << " from Turret " << inet_ntoa(dest.sin_addr);
                }
            }
            else if (startsWith(msg, "ready") == 0) { 
                // add new turret instance
                TurretInstance ti_new = TurretInstance(&dest, port, TurretInstance::ready);
                LOG(DEBUG) << " new turret instance added: " << inet_ntoa(dest.sin_addr) << ":" << ntohs(dest.sin_port) <<" id" << ti_new.id;
                turretIList.push_front(ti_new);
                availableTurret++;
                pthread_cond_signal(&distributor_cond);
            }
            else {
                LOG(ERROR) << "rcvd " << buff << " from unknown";
            }

            // XXX - to decide ping back
            while (len-len2 > 0) {
                 int cur = write(sock, buff + len2, len - len2);
                 if (cur <= 0) {
                     LOG(INFO) << "Fail to write to " << inet_ntoa(dest.sin_addr);
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
        strategy benign = strategy(0, "*?*?BaseMessage NONE 0"); // basic benign strategy twice
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

                LOG(DEBUG) << "start piping";
                __gnu_cxx::stdio_filebuf<char> filebuf(outpipe[PREAD], std::ios::in); // 1
                std::istream is(&filebuf);
                std::string line;
                // the format of each line should be weight:strategy
                while ( std::getline(is, line)) {
                    int weight;
                    boost::char_separator<char> sep(":");
                    boost::tokenizer< boost::char_separator<char> > tok(line, sep);
                    boost::tokenizer< boost::char_separator<char> >::iterator it=tok.begin();
                    weight = atoi((*it++).c_str());
                    strategy str = strategy(weight, *it);
                    waitingStrategy.push_front(str);
                    LOG(DEBUG) << " str: " << str.content << " weight: " << str.weight;
                }
                waitingStrategy.sort(strCmp);
                for(std::list<strategy>::iterator it = waitingStrategy.begin(); it != waitingStrategy.end(); it++) {
                   LOG(DEBUG) << "sorted: " << (*it).weight << " - " << (*it).content;
                }
                LOG(DEBUG) << "reading done";
                close(inpipe[PWRITE]);
                close(outpipe[PREAD]);
            }
            pthread_mutex_lock(&strategy_mutex);
            expanding = false;
            pthread_cond_signal(&distributor_cond);
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
    for (std::list<TurretInstance>::iterator it=turretIList.begin(); it!= turretIList.end(); it++) {
        ti = &(*it);
        if (ti->status == TurretInstance::Status::ready) return ti;
    }
    return NULL;
}

void TurretInstance::sendStrategy() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) perror("Error opening socket");
    LOG(DEBUG) << "connecting TurretInstance " << id << " addr: " << inet_ntoa(addr.sin_addr) << " port: " << ntohs(addr.sin_port);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Error connecting Turret");
        LOG(ERROR) << "can't connect turret : " << id;
    }
    else {
        LOG(DEBUG) << "will write: " << curStrategy->content.c_str();
        int n = write(sockfd, curStrategy->content.c_str(), strlen(curStrategy->content.c_str())+1);
        LOG(DEBUG) << "wrote " << n;
        // XXX maybe read back?
    }
    close(sockfd);
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

            ti->setCurStrategy(nextStr);
            ti->sendStrategy();
            // XXX send the strategy to the instance

            pthread_mutex_lock(&strategy_mutex);
            runningTurretCnt++;
            availableTurret--;
            pthread_cond_signal(&distributor_cond);
        }
        pthread_mutex_unlock(&strategy_mutex);
    }
}

// update performance
void gc::updatePerformance(TurretInstance * ti, std::string perfString) {
    // XXX what if there's status mismatch?
    // update the strategy finished
    // signal appropriate threads and update this instance ready
    LOG(DEBUG) << "performanceResult added - ti: " << ti->id << " perf: " << perfString << " size: " << performanceResult.size();
    performanceResult.push(perfString);
    pthread_cond_signal(&strategy_cond);
}

bool gc::finishCondition() {
    pthread_mutex_lock(&strategy_mutex);
    if (waitingStrategy.empty() && runningTurretCnt == 0 && expanding == false) {
        pthread_mutex_unlock(&strategy_mutex);
        return true;
    }
    pthread_mutex_unlock(&strategy_mutex);
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
    std::thread th_reportCollector(&gc::reportCollector, std::ref(gc_instance));
    sleep(1);
    while (1) {
        if (gc_instance.finishCondition() == true) {
            LOG(DEBUG) << "finish";
            quit = 1;
            break;
        }
    }

    th_distributor.join();
    th_reportCollector.join();
    th_strComposer.join();

    return 0;
}
