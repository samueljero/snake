#include <stdlib.h>
#include <iostream>
#include "stateMachine.h"
#include "stateMetric.h"

static int sm_debug=0;

using namespace std;

namespace ns3 {

    ostream& operator<<(ostream& os, const State& s) {
        os << s.m_name;
        return os;
    }
    bool operator== (const State &l, const State &r) {return (l.m_name == r.m_name); }
    bool operator!= (const State &l, const State &r) {return !(l == r);}
    bool operator>  (const State &l, const State &r) {return l.m_name > r.m_name; }
    bool operator<  (const State& l, const State& r) {return l.m_name < r.m_name; }

    ostream& operator<< (std::ostream& os, const Transition& tr) {
        os <<  "type: " << tr.m_type << " rcvd: " << tr.m_rcvd << " " << tr.m_rcvdType <<
            " send: " << tr.m_send << " " << tr.m_sendType;
        return os;
    }
    bool operator== (const Transition  &l, const Transition &r) {return (l.m_type == r.m_type); }
    bool operator!= (const Transition  &l, const Transition &r) {return !(l == r);}
    bool operator>  (const Transition  &l, const Transition &r) {return l.m_type > r.m_type; }
    bool operator<  (const Transition & l, const Transition &r) {return l.m_type < r.m_type; }

    // state machine construction
    void StateMachine::AddState(State state) {
        m_stateSet.insert(state);
        m_stateMap[state.GetName()] = state;
    }

    void StateMachine::AddTransition(Transition transition, State from, State to) {
        // check if already in
        (m_nextTransitionMap[from])[transition.GetType()] = to;
        (m_reverseTransitionMap[to])[transition.GetType()] = from;

        m_trMap[transition.GetType()] = transition; // TODO
        if (transition.Rcvd().compare(0, 2, "M_") == 0) {
            string name = transition.Rcvd().substr(2);
            m_trMsgMap[from][name].first = transition;
        }
        if(transition.Rcvd().compare(0,2,"A_")==0){
        	if(transition.Send().length()>2){
				string name = transition.Send().substr(2);
				m_trMsgMap[from][name].second = transition;
        	}
        }
        if(transition.Rcvd().compare(0,2,"E_")==0){
        	if(transition.Send().length()>2){
				string name = transition.Send().substr(2);
				m_trMsgMap[from][name].second = transition;
        	}
        }

        if (m_stateSet.find(from) == m_stateSet.end()) m_stateSet.insert(from);
        if (m_stateSet.find(to) == m_stateSet.end()) m_stateSet.insert(to);

    }

    NextMap StateMachine::GetValidTransitions(State from) {
        return m_nextTransitionMap[from];
    }

    NextMap StateMachine::GetTransitionsTo(State to) {
        NextMap map = m_reverseTransitionMap[to];
        for(NextMap::iterator it = map.begin(); it != map.end(); ){
            if (it->second != m_curState) {
                map.erase(it++);
            } else {
                ++it;
            }
        }
        return map;
    }

    TrSet StateMachine::GetInvalidTransitions(State from) {
        NextMap valid = m_nextTransitionMap[from];
        TrSet invalid;
        for(TrMap::iterator it = m_trMap.begin(); it != m_trMap.end(); it++) {
            if (valid.find(it->first) == valid.end()) {
                invalid.insert(it->second);
            }
        }
        return invalid;
    }


    void StateMachine::Print() {
        cout << "cur state:" << GetCurrentState() << endl;
    }
       
    void StateMachine::PrintRules() {
        cout << "transitions" << endl;
        for (TrMap::iterator it = m_trMap.begin(); it != m_trMap.end(); it++) {
            Transition t = it->second;
            cout << t << endl;
        }
        cout << "valid transitions" << endl;
        StateSet::iterator iterState;
        for (iterState = m_stateSet.begin(); iterState != m_stateSet.end(); iterState++) {
            State st = *iterState;
            NextMap fromSet = m_nextTransitionMap[st];
            for (NextMap::iterator iter = fromSet.begin(); iter != fromSet.end(); iter++) {
                Transition tr = m_trMap[iter->first];
                cout << "from:" << st << " " << tr << endl;
            }
        }
        cout << "reverse transitions" << endl;
        for (iterState = m_stateSet.begin(); iterState != m_stateSet.end(); iterState++) {
            State st = *iterState;
            NextMap tset = m_reverseTransitionMap[st];
            for (NextMap::iterator iter = tset.begin(); iter != tset.end(); iter++) {
                Transition tr = m_trMap[iter->first];
                cout << "to: " << st << " " << tr << endl;
            }
        }
    }


    Transition *StateMachine::GetMatchingTransition(State st, int rcvd, int send) {
        NextMap nmap = GetValidTransitions(st);
        for(NextMap::iterator it = nmap.begin(); it != nmap.end(); it++){
            Transition *t = &(m_trMap[it->first]);
            if (t->m_sendType == send && t->m_rcvdType == rcvd) {
                // we never encounter where both rcvd and send are positive. 
                // therefore, this should be the case only one side exist in the rule itself
                // and cover both send == -1 , rcvd == -1 cases
                return t;
            }
            if (t->m_rcvdType == rcvd && t->m_sendType > 0) {
                // cache it
                m_cachedRcvdMsg = rcvd;
            }
            if (m_cachedRcvdMsg > 0 && t->m_rcvdType == m_cachedRcvdMsg && t->m_sendType == send) {
                // rule match the the cached rcvd
                m_cachedRcvdMsg = -1;
                return t;
            }
        }
        return NULL;
    }

    State StateMachine::MakeTransition(int rcvdType, int sendType, unsigned long now) {
        Transition *t = GetMatchingTransition(m_curState, rcvdType, sendType);
        if (t != NULL) return MakeTransition(t->GetType(), now);
        return State("INVALID");
    }

    State StateMachine::MakeTransition(int trType, unsigned long now) {
        NextMap next = m_nextTransitionMap[m_curState];
        if (next.find(trType) != next.end()) {
            smt.End("time_spent", m_curState, now);
            m_curState = next[trType];
            smt.Start("time_spent", m_curState, now);
            smt.IncrementMetric("visit_cnt", m_curState);
        }
        
      // else {
      //     m_valid = false;
      //     smt.End("time_spent", m_curState, now);
      //     m_curState = State("ILLEGAL");
      // }
        if(sm_debug>0){Print();}
        return m_curState;
    }

    int StateMachine::GetTransitionType(string rcvd, string send) {
        for(TrMap::iterator it = m_trMap.begin(); it != m_trMap.end(); it++) {
            Transition tr = it->second;
            if (tr.Send() == send && tr.Rcvd() == rcvd) return tr.GetType();
        }

        return (m_trMap.size() + 1);
    }
}
