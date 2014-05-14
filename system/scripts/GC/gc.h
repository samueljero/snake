#ifndef _GC
#define _GC

#include <string>
#include <pthread.h>
#include <list>
#include <queue>
#include <set>

pthread_mutex_t history_mutex;
pthread_mutex_t strategy_mutex;
pthread_cond_t  strategy_cond;
pthread_cond_t  distributor_cond;

class strategy {
    public:
    int weight;
    std::string content;
    bool operator== (const strategy &s2);
    bool operator< (const strategy &s2);
    strategy(std::string c, int w) {
        content = c;
        weight = w;
    }
};

extern int turretInstanceId;

class TurretInstance {
    public:
        enum Status {
            ready,
            running,
            error
        };
        struct sockaddr_in addr;
        int sin_port;
        int port;
        int id;
        Status status;
        TurretInstance(struct sockaddr_in* addrP, int pt, Status st) {
            memcpy(&addr, addrP, sizeof(struct sockaddr_in));
            status = st;
            id = turretInstanceId++;
        }
        bool ifMatch(struct sockaddr_in* addrP) {
            if (memcmp(&addr, addrP, sizeof(struct sockaddr_in)) == 0) return true;
            return false;
        }

};

class comp 
{
    public:
        comp();
        bool operator() (const strategy &lhs, const strategy &rhs) const
        {
            return ((lhs).weight<(rhs).weight);
        }
        bool operator() (const TurretInstance &lhs, const TurretInstance &rhs) const
        {
            return (lhs.addr.sin_addr.s_addr < rhs.addr.sin_addr.s_addr);
        }
};

#define startsWith(s1, s2) strncmp(s1, s2, strlen(s2))
class gc {
    // strategy
    std::list<strategy> waitingStrategy;
    std::queue<std::string> performanceResult;
    bool expanding;
    std::list<TurretInstance> turretInstances;

    public:
    int runningTurretCnt;
    int availableTurret;


    void perfCollector();
    void strategyComposer();
    void distributor();
    bool finishCondition();
    bool ready_to_send();
    TurretInstance *nextAvailableTurret();

    gc() {
        runningTurretCnt = 0;
        availableTurret = 0;
        expanding = false;
        pthread_mutex_init(&history_mutex, NULL);
        pthread_mutex_init(&(strategy_mutex), NULL);
        pthread_cond_init(&(distributor_cond), NULL);
        pthread_cond_init(&(strategy_cond), NULL);
    }
    ~gc() {
        pthread_mutex_destroy(&history_mutex);
        pthread_mutex_destroy(&strategy_mutex);
    }
};

#endif 
