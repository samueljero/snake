#include <stdlib.h>
#include <iostream>
#include "stateMachine.h"

using namespace std;

namespace ns3 {

    void StateMachine::AddState(State state) {
        m_stateSet.insert(state);
    }

    TrSet StateMachine::GetValidTransitions(State from) {
        return m_validTransitions[from];
    }

    TrSet StateMachine::GetTransitionsTo(State to) {
        TrSet trSet = m_reverseTransitions[to];
        for(TrSet::iterator it = trSet.begin(); it != trSet.end(); ){
            Transition tr = *it;
            if (tr.From() == m_curState) {
                trSet.erase(it++);
            } else {
                ++it;
            }
        }
        return trSet;
    }

    TrSet StateMachine::GetInvalidTransitions(State from) {
        TrSet invalid;
        TrSet valid = m_validTransitions[from];
        for(TrSet::iterator it = m_trSet.begin(); it != m_trSet.end(); it++) {
            if (valid.find(*it) == valid.end()) invalid.insert(*it);
        }
        return invalid;
    }

    void StateMachine::AddTransition(Transition transition) {
        m_trSet.insert(transition);
        State from = transition.From();
        State to = transition.To();
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
        cout << "valid transitions" << endl;
        StateSet::iterator iterState;
        for (iterState = m_stateSet.begin(); iterState != m_stateSet.end(); iterState++) {
            TrSet fromSet = m_validTransitions[*iterState];
            for (TrSet::iterator iter = fromSet.begin(); iter != fromSet.end(); iter++) {
                Transition tr = *iter;
                cout << " from " << tr.From() << " to " << tr.To() << " rcvd " << tr.Rcvd() << " send " << tr.Send() << endl;
            }
        }
        cout << "reverse transitions" << endl;
        for (iterState = m_stateSet.begin(); iterState != m_stateSet.end(); iterState++) {
            TrSet tset = m_reverseTransitions[*iterState];
            for (TrSet::iterator iter = tset.begin(); iter != tset.end(); iter++) {
                Transition tr = *iter;
                cout << " to " << tr.To() << " from " << tr.From() << " rcvd " << tr.Rcvd() << " send " << tr.Send() << endl;
            }
        }
        
    }

    std::ostream& operator << (std::ostream& os, const ns3::Transition& tr) {
        os << " from: " << tr.m_from << " to: " << tr.m_to << " rcvd: " << tr.m_rcvd << " send: " << tr.m_send;
    }


    State StateMachine::MakeTransition(Transition tr) {
        // if valid - move
        TrSet fromSet = m_validTransitions[m_curState];
        m_prevState = m_curState;
        m_curState = "UNDEFINED";
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
