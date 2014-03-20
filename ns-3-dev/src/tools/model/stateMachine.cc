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

}
