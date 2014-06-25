#ifndef _GC
#define _GC

#include <string>
#include <pthread.h>
#include <list>
#include <queue>
#include <set>
#include <unordered_map>


#define MAINGC_PORT 9991

pthread_mutex_t turret_mutex;
pthread_mutex_t strategy_mutex;
pthread_cond_t  strategy_cond;
pthread_cond_t  distributor_cond;

class strategy {
    public:
    int weight;
    std::string content;
    bool operator== (const strategy &s2);
    bool operator< (const strategy &s2);
    strategy(int w, std::string c) {
        content = c;
        weight = w;
    }
    strategy(const strategy &s2) {
        content = s2.content;
        weight = s2.weight;
    }
};

extern int turretInstanceId;

const char *statusname[] = {"ready", "running", "reporting", "sending", "error"};

class TurretInstance {
    public:
        enum Status {
            ready,
            running,
            reporting,
            sending,
            error
        };
        struct sockaddr_in addr;
        int sin_port;
        int port;
        int id;
        Status status;
        strategy *curStrategy;
        void setCurStrategy(strategy str) { curStrategy = new strategy(str); }
        TurretInstance(struct sockaddr_in* addrP, int pt, Status st) {
            memcpy(&addr, addrP, sizeof(struct sockaddr_in));
            status = st;
            id = ++turretInstanceId;
        }
        bool ifMatch(struct sockaddr_in* addrP) {
            if (memcmp(&addr, addrP, sizeof(struct sockaddr_in)) == 0) return true;
            return false;
        }
        void sendMessage(const char * msg, size_t msg_len);
        friend std::ostream& operator<< (std::ostream& stream, const TurretInstance & ti) {
            stream << " ID: " << ti.id << " Status: " << statusname[ti.status];
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
    std::list<std::string> feedback;
    std::unordered_map<std::string, int> prev_feedback;
    std::queue<std::string> performanceResult;
    bool expanding;

    public:
    std::list<TurretInstance> turretIList;


    void reportCollector();
    void strategyComposer();
    void distributor();
    bool ready_to_send();
    bool ready_to_stop();
    void updatePerformance(TurretInstance *ti, std::string perfString);
    TurretInstance *nextAvailableTurret();
    int executePipeToFillWaitingStrategy(std::string &args);
    void send_feedback(int fd);

    gc() {
        expanding = false;
        pthread_mutex_init(&turret_mutex, NULL);
        pthread_mutex_init(&(strategy_mutex), NULL);
        pthread_cond_init(&(distributor_cond), NULL);
        pthread_cond_init(&(strategy_cond), NULL);
    }
    ~gc() {
        pthread_mutex_destroy(&turret_mutex);
        pthread_mutex_destroy(&strategy_mutex);
    }
};

#endif 
