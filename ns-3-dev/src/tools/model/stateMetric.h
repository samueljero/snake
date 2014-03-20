#ifndef STATE_METRIC_H
#define STATE_METRIC_H

using namespace std;
namespace ns3 {
    // TO THINK: is it better to make metric only be double or to make it polymorphic?
    typedef pair <State, class T> StateValuePair;
    template <class T> class StateMetric {
        string m_name;
        map<State, StateValuePair> m_mapStateValue;
        StateMetric(string name) {m_name = name;}
    };

    template <typename T> class CompareMetric
    {
        public:
            inline bool operator() (StateMetric<T> l, StateMetric<T> r)
            {
                return (l.m_name < r.m_name);
            }
    };

    class StateMetricTracker {
        map <string, StateMetric<T>, CompareMetric<T> > m_mapStateMetric;
        StateMetricTracker();
        void UpdateMetric(string name, State st, T newvalue);
        void AddMetric(string name, State st, T delta);
    };
}

#endif
