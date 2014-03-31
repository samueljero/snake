#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include <string>
#include <set>
#include <map>

#include "state.h"
#include "transition.h"
#include "stateMetric.h"
using namespace std;

namespace ns3 {

    class Comp
    {
        public:
            inline bool operator() (State l, State r)
            {
                return (l < r);
            }

            inline bool operator() (Transition l, Transition r)
            {
                return (l.GetType() < r.GetType());
            }
            inline bool operator() (string l, string r)
            {
                return (l < r);
            }
    };

    typedef set <State, Comp> StateSet;
    typedef set <Transition, Comp> TrSet;
    typedef map <int, State> NextMap;
    typedef map <int, Transition> TrMap;
    typedef map <State, map<string, pair<Transition,Transition> > > TrMsgMap;
    typedef map <string, State> StateMap;

    class StateMachine {
        State m_curState;
        State m_prevState;
        StateSet m_stateSet;     // set of states
        StateMap m_stateMap;
        TrMap m_trMap;
        TrMsgMap m_trMsgMap;
        bool m_valid;
        int m_cachedRcvdMsg;

        map <State, NextMap> m_nextTransitionMap; // valid transitions for each state:
                                    // m_nextTransitions[from_state][transitionType] --> State
        map <State, NextMap> m_reverseTransitionMap; // valid transitions for each state:
                                    // m_nextTransitions[to_state][transitionType] --> from_state

        public:
        StateMetricTracker smt;
        // construction
        void AddState(State state);
        void AddTransition(Transition tr, State from, State to);
        void AddTransition(Transition tr);
        int GetTransitionType(string rcvd) { return GetTransitionType(rcvd, 0); }
        int GetTransitionType(string rcvd, string send);
        Transition GetTransition(int type) { return m_trMap[type];}

        // state machine information
        State GetCurrentState(void) { return m_curState; };
        StateMetricTracker* GetStateMetricTracker(void) { return &smt; };
        void IncrementMetric(string name) {smt.IncrementMetric(name, m_curState); };

        // to help attacks
        NextMap GetValidTransitions(State from); // set of transitions that are valid from the current state
        TrSet GetInvalidTransitions(State from); // set of transitions not valid in the current state
        NextMap GetTransitionsTo(State to); // set of transitions need to go "to" state from current state

        // for us
        void Print();
        void PrintRules();

        // state machine moving
        void Start(State start, unsigned long now) {
            m_curState = start;
            m_cachedRcvdMsg = -1;
            m_valid = true;
            smt.Start("time_spent", m_curState, now);
            smt.IncrementMetric("visit_cnt", m_curState);
        };
        // mainly to make sure the current state time is updated correctly
        void Finish(unsigned long now) { smt.End("time_spent", m_curState, now); }
        // TODO: return action
        State MakeTransition(int trType, unsigned long now);
        State MakeTransition(string msgTypeName, unsigned long now);
        State MakeTransition(int rcvdType, int sendType, unsigned long now) ;
        Transition * GetMatchingTransition(State st, int rcvd, int send) ;
    };
}
#endif
