#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include <string>
#include <set>
#include <map>

namespace ns3 {
    class Transition;
    typedef std::string State;

    class Transition
    {
        public:
            std::string m_rcvd;
            std::string m_send;
            std::string m_from;
            std::string m_to;
            inline std::string Rcvd() { return m_rcvd; }
            inline std::string Send() { return m_send; }
            inline State From() { return m_from; }
            inline State To() { return m_to; }
            Transition (std::string rcvd, std::string send, std::string from, std::string to) {
                m_rcvd = rcvd;
                m_send = send;
                m_from = from;
                m_to = to;
            }
            bool compare(Transition target) {
                if (m_from.compare(target.From()) != 0) return (m_from.compare(target.From()));
                if (m_rcvd.compare(target.Rcvd()) != 0) return (m_rcvd.compare(target.Rcvd()));
                if (m_to.compare(target.To()) != 0) return (m_to.compare(target.To()));
                return m_send.compare(target.Send());
            }


    };
    std::ostream& operator << (std::ostream& os, const Transition& tr);

    class Comp
    {
        public:
            inline bool operator() (Transition l, Transition r)
            {
                if (l.From() != r.From()) return (l.From() < r.From());
                if (l.Rcvd() != r.Rcvd()) return (l.Rcvd() < r.Rcvd());
                if (l.To() != r.To()) return (l.To() < r.To());
                return (l.Send() < r.Send());
            }
            inline bool operator() (std::string l, std::string r)
            {
                return (l < r);
            }
    };
    
    typedef std::set <State, Comp> StateSet;
    typedef std::set <Transition, Comp> TrSet;
    typedef std::pair <State, TrSet> TrSetPair;

    class StateMachine {
        public:
            State m_curState;
            State m_prevState;
            StateSet m_stateSet;     // set of states
            TrSet m_trSet;
            std::map <State, TrSet> m_validTransitions; // valid transitions for each state
            std::map <State, TrSet> m_reverseTransitions; // transitions that will lead to the given state

            State GetCurrentState(void) {
                return m_curState;
            };
            TrSet GetValidTransitions(State from); // set of transitions that are valid from the current state
            TrSet GetInvalidTransitions(State from); // set of transitions not valid in the current state
            TrSet GetTransitionsTo(State to); // set of transitions need to go "to" state from current state
            void Print();

            void AddState(State state);
            void AddTransition(Transition);
            void Start(State start) { m_curState = start; };
            State MakeTransition(Transition);

    };
}
#endif
