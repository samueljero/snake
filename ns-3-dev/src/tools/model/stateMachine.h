#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include <string>
#include <set>
#include <map>

#include "state.h"
#include "transition.h"
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
                if (l.From().compare(r.From()) == false) return (l.From().GetName() < r.From().GetName());
                if (l.To().compare(r.To()) == false ) return (l.To().GetName() < r.To().GetName());
                if (l.Rcvd() != r.Rcvd()) return (l.Rcvd() < r.Rcvd());
                return (l.Send() < r.Send());
            }
            inline bool operator() (string l, string r)
            {
                return (l < r);
            }
    };

    typedef set <State, Comp> StateSet;
    typedef set <Transition, Comp> TrSet;
    typedef pair <State, TrSet> TrSetPair;

    class StateMachine {
        public:
            State m_curState;
            State m_prevState;
            StateSet m_stateSet;     // set of states
            TrSet m_trSet;
            map <State, TrSet> m_validTransitions; // valid transitions for each state
            map <State, TrSet> m_reverseTransitions; // transitions that will lead to the given state

            State GetCurrentState(void) {
                return m_curState;
            };
            TrSet GetValidTransitions(State from); // set of transitions that are valid from the current state
            TrSet GetInvalidTransitions(State from); // set of transitions not valid in the current state
            TrSet GetTransitionsTo(State to); // set of transitions need to go "to" state from current state
            void Print();
            void PrintRules();

            void AddState(State state);
            void AddTransition(Transition tr);
            void Start(State start) { m_curState = start; };
            State MakeTransition(Transition);
    };
}
#endif
