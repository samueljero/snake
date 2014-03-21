#include <stdlib.h>
#include <iostream>
#include "stateMachine.h"
#include "stateMetric.h"

using namespace std;

namespace ns3 {

    ostream& operator<<(ostream& os, const State& s) { os << s.m_name; return os ;}
    bool operator== (const State &l, const State &r) {return (l.m_name == r.m_name); }
    bool operator!= (const State &l, const State &r) {return !(l == r);}
    bool operator>  (const State &l, const State &r) {return l.m_name > r.m_name; }
    bool operator<  (const State& l, const State& r) {return l.m_name < r.m_name; }

    ostream& operator<<(ostream& os, const StateMetric& s) { os << s.m_name; return os ;}
    bool operator== (const StateMetric &l, const StateMetric &r) {return (l.m_name == r.m_name); }
    bool operator!= (const StateMetric &l, const StateMetric &r) {return !(l == r);}
    bool operator>  (const StateMetric &l, const StateMetric &r) {return l.m_name > r.m_name; }
    bool operator<  (const StateMetric& l, const StateMetric& r) {return l.m_name < r.m_name; }

    void StateMachine::AddState(State state) {
        m_stateSet.insert(state);
    }

    TrSet StateMachine::GetValidTransitions(State from) {
        return m_validTransitions[from];
    }

    TrSet StateMachine::GetTransitionsTo(State to) {
        TrSet trSet;
        trSet = m_reverseTransitions[to];
        for(TrSet::iterator it = trSet.begin(); it != trSet.end(); ){
            Transition tr = *it;
            State from = tr.From();
            if (from.compare(m_curState) == false) {
                trSet.erase(it++);
            } else {
                ++it;
            }
        }
        return trSet;
    }

    TrSet StateMachine::GetInvalidTransitions(State from) {
        TrSet invalid;
        TrSet valid;
        valid = m_validTransitions[from];
        for(TrSet::iterator it = m_trSet.begin(); it != m_trSet.end(); it++) {
            if (valid.find(*it) == valid.end()) invalid.insert(*it);
        }
        return invalid;
    }

    void StateMachine::AddTransition(Transition transition) {
        m_trSet.insert(transition);
        State from = (transition.From());
        State to = (transition.To());
        if (m_stateSet.find(from) == m_stateSet.end()) m_stateSet.insert(from);
        if (m_stateSet.find(to) == m_stateSet.end()) m_stateSet.insert(to);

        if (m_validTransitions.find(from) == m_validTransitions.end()) {
            TrSet fromSet = m_validTransitions[from];
            fromSet.insert(transition);
            m_validTransitions[from] = fromSet;
        } else {
            m_validTransitions[from].insert(transition);
        }
        if (m_reverseTransitions.find(to) == m_reverseTransitions.end()) {
            TrSet toSet = m_reverseTransitions[to];
            toSet.insert(transition);
            m_reverseTransitions[to] = toSet;
        } else {
            m_reverseTransitions[to].insert(transition);
        }
    }

    void StateMachine::Print() {
        cout << "cur state:" << GetCurrentState() << endl;
    }
       
    void StateMachine::PrintRules() {
        cout << "valid transitions" << endl;
        StateSet::iterator iterState;
        for (iterState = m_stateSet.begin(); iterState != m_stateSet.end(); iterState++) {
          State st = *iterState;
            TrSet fromSet = m_validTransitions[st];
            for (TrSet::iterator iter = fromSet.begin(); iter != fromSet.end(); iter++) {
                Transition tr = *iter;
                cout << " from " << tr.From() << " to " << tr.To() << " rcvd " << tr.Rcvd() << " send " << tr.Send() << endl;
            }
        }
        cout << "reverse transitions" << endl;
        for (iterState = m_stateSet.begin(); iterState != m_stateSet.end(); iterState++) {
          State st = *iterState;
            TrSet tset = m_reverseTransitions[st];
            for (TrSet::iterator iter = tset.begin(); iter != tset.end(); iter++) {
                Transition tr = *iter;
                cout << " to " << tr.To() << " from " << tr.From() << " rcvd " << tr.Rcvd() << " send " << tr.Send() << endl;
            }
        }
        
    }

    ostream& operator<< (std::ostream& os, const Transition& tr) {
        os << " from: " << tr.m_from << " to: " << tr.m_to << " rcvd: " << tr.m_rcvd << " send: " << tr.m_send;
    }


    State StateMachine::MakeTransition(Transition tr) {
        // if valid - move
        TrSet fromSet = m_validTransitions[m_curState];
        m_prevState = m_curState;
        m_curState = State("UNDEFINED");
        for (TrSet::iterator iter = fromSet.begin(); iter != fromSet.end(); iter++) {
            Transition curTr = *iter;
            if (curTr.compare(tr) == 0) {
                cout << "legal transition " << curTr << endl;
                m_curState = tr.To();
                return m_curState;
            }
        }
        cout << "Illegal transition " << tr << endl;
        return m_curState;
    }

    void StateMetricTracker::UpdateMetric(string name, State st, double newvalue) {
        m_mapStateMetric[name].UpdateValue(st, newvalue);
    }

    void StateMetricTracker::UpdateByDelta(string name, State st, double delta) {
        m_mapStateMetric[name].UpdateValueDelta(st, delta);
    }

    double StateMetricTracker::GetMetricValue(string name, State st) {
        return m_mapStateMetric[name].GetValue(st);
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

    void StateMetricTracker::PrintAll() {
        cout << "---------------------------------" << endl;
        list<string> metrics = GetMetrics();
        for (list<string>::iterator it = metrics.begin(); it != metrics.end(); it++) {
            string metric = *it;
            list<State> states = m_mapStateMetric[metric].GetStates();
            for (list<State>::iterator it_st = states.begin(); it_st != states.end(); it_st++) {
                State st = *it_st;
                cout << "metric: " << metric << " state: " << st << " value : " << m_mapStateMetric[metric].GetValue(st) << endl;
            }
        }
        cout << "---------------------------------" << endl;
    }
}
