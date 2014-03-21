#ifndef STATE_METRIC_H
#define STATE_METRIC_H

#include <list>

using namespace std;
namespace ns3 {
    // All metrics are double - just the way Gatling performance metric was

    class StateMetric {
        string m_name;
        map<State, double> m_mapStateValue;
        public:
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
    };


    // Want to way to update and get metric values
    // How often would they be called?
    class StateMetricTracker {
        // map of state matrics key by name
        map <string, StateMetric> m_mapStateMetric;
        public:
        StateMetricTracker() {}
        void UpdateMetric(string name, State st, double newvalue);
        void UpdateByDelta(string name, State st, double delta);
        double GetMetricValue(string name, State st);
        StateMetric GetMetric(string name, State st);
        list<string> GetMetrics();
        void PrintAll();
    };
}

#endif
