#ifndef STATE_H
#define STATE_H

using namespace std;
namespace ns3 {
    class State
    {
        string m_name;
        public:
        State() {
            m_name = "invalid";
        }
        State (string name) {
            m_name = name;
        }
        string GetName() {return m_name;};
        friend ostream& operator<<(ostream& os, const State& s);
        friend bool operator== (const State& l, const State& r);
        friend bool operator!= (const State& l, const State& r);
        friend bool operator< (const State& l, const State& r);
        friend bool operator> (const State& l, const State& r);
    };

}
#endif
