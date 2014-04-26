#ifndef STATE_METRIC_H
#define STATE_METRIC_H

#include <list>

using namespace std;
/* StateMetricTracker 
 * This is merely a simple API to retain metrics we want to observe
 * Calling these functions are the most important part of the scenario
 * Some of metrics 
 * 1. time spent in the state - do in the state machine
 * 2. received message count - do in the tap device
 * 3. sent message count - do in the tap device
 * PrintAll will flush all results out and it is CSV format, intended for analysis
 */
namespace ns3 {
    // All metrics are double - just the way Gatling performance metric was

    class StateMetric {
        string m_name;
        bool m_isTime;
        map<State, double> m_mapStateValue;

        public:
        void SetTimeProperty(bool value) {m_isTime = value; }
        void UpdateValue(State st, double newvalue) { m_mapStateValue[st] = newvalue; }
        void UpdateValueDelta(State st, double delta) { m_mapStateValue[st] += delta; }
        double GetValue(State st) { return m_mapStateValue[st]; }
        friend ostream& operator<<(ostream& os, const StateMetric& s);
        friend bool operator== (const StateMetric& l, const StateMetric& r);
        friend bool operator!= (const StateMetric& l, const StateMetric& r);
        friend bool operator< (const StateMetric& l, const StateMetric& r);
        friend bool operator> (const StateMetric& l, const StateMetric& r);
        size_t GetCount() {return m_mapStateValue.size();}
        list<State> GetStates();
        void clear() {m_mapStateValue.clear();}
    };


    // Want to way to update and get metric values
    // How often would they be called?
    class StateMetricTracker {
        // map of state matrics key by name
        map <string, StateMetric> m_mapStateMetric;
        typedef set<int> MsgSet; // set of message types
        map <State, MsgSet> m_mapStateSentMsgs;
        public:
        StateMetricTracker() {}
        void UpdateMetric(string name, State st, double newvalue);
        void AddMsgType(State st, int msgType);
        void UpdateByDelta(string name, State st, double delta);
        void IncrementMetric(string name, State st) {UpdateByDelta(name, st, 1);} // for counters
        void Start(string name, State st, unsigned long now);
        void End(string name, State st, unsigned long now);
        double GetMetricValue(string name, State st);
        StateMetric GetMetric(string name, State st);
        list<string> GetMetrics();
        void PrintAll(ostream &os);
        void Reset();
    };
}

#endif
