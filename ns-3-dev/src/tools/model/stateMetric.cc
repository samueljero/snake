#include <stdlib.h>
#include <iostream>
#include "stateMachine.h"
#include "stateMetric.h"

using namespace std;

namespace ns3 {

    ostream& operator<<(ostream& os, const StateMetric& s) { os << s.m_name; return os ;}
    bool operator== (const StateMetric &l, const StateMetric &r) {return (l.m_name == r.m_name); }
    bool operator!= (const StateMetric &l, const StateMetric &r) {return !(l == r);}
    bool operator>  (const StateMetric &l, const StateMetric &r) {return l.m_name > r.m_name; }
    bool operator<  (const StateMetric& l, const StateMetric& r) {return l.m_name < r.m_name; }

    void StateMetricTracker::UpdateMetric(string name, State st, double newvalue) {
        m_mapStateMetric[name].UpdateValue(st, newvalue);
    }

    void StateMetricTracker::UpdateByDelta(string name, State st, double delta) {
        m_mapStateMetric[name].UpdateValueDelta(st, delta);
    }

    double StateMetricTracker::GetMetricValue(string name, State st) {
        return m_mapStateMetric[name].GetValue(st);
    }

    // call using Simulator::Now()
    void StateMetricTracker::Start(string name, State st, unsigned long now) {
        m_mapStateMetric[name].UpdateValue(st, (double)now);
    }

    void StateMetricTracker::End(string name, State st, unsigned long now) {
        double start = m_mapStateMetric[name].GetValue(st);
        m_mapStateMetric[name].UpdateValue(st, (double)now - start);
    }

    list<State> StateMetric::GetStates() {
        list<State> states;
        for(map<State,double>::iterator it = m_mapStateValue.begin(); it != m_mapStateValue.end(); ++it) {
            states.push_back(it->first);
        }
        return states;
    }

    list<string> StateMetricTracker::GetMetrics() {
        list<string> metrics;
        for(map<string,StateMetric>::iterator it = m_mapStateMetric.begin(); it != m_mapStateMetric.end(); ++it) {
            metrics.push_back(it->first);
        }
        return metrics;
    }

    // this is mainly for anaylsis. 
    void StateMetricTracker::PrintAll(ostream& os) {
        os << "Metric\n" ;
        os << "metric,\tstate,\tvalue\n";
        list<string> metrics = GetMetrics();
        for (list<string>::iterator it = metrics.begin(); it != metrics.end(); it++) {
            string metric = *it;
            list<State> states = m_mapStateMetric[metric].GetStates();
            for (list<State>::iterator it_st = states.begin(); it_st != states.end(); it_st++) {
                State st = *it_st;
                os << metric << ",\t" << st << ",\t" << m_mapStateMetric[metric].GetValue(st) << "\n";
            }
        }
    }

    void StateMetricTracker::Reset() {
        for(map<string,StateMetric>::iterator it = m_mapStateMetric.begin(); it != m_mapStateMetric.end(); ++it) {
            StateMetric sm = it->second;
            sm.clear();
        }
        m_mapStateMetric.clear();
    }

    // if there's any time value that's not finished, finish here
}