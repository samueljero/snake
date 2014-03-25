#ifndef TRANSITION_H
#define TRANSITION_H
using namespace std;
namespace ns3 {
    class Transition
    {
        string m_rcvd;
        string m_send;
        int m_type;
        public:
        int GetType() {return m_type;}
        inline string Rcvd() { return m_rcvd; }
        inline string Send() { return m_send; }

        Transition
            (int type, string rcvd, string send) {
                m_type = type;
                m_rcvd = rcvd;
                m_send = send;
            }
        Transition (const Transition& t) {
            m_type = t.m_type;
            m_rcvd = t.m_rcvd;
            m_send = t.m_send;
        }

        Transition() {}

        friend ostream& operator<< (ostream& os, const Transition& tr);
        friend bool operator== (const Transition& l, const Transition& r);
        friend bool operator!= (const Transition& l, const Transition& r);
        friend bool operator< (const Transition& l, const Transition& r);
        friend bool operator> (const Transition& l, const Transition& r);
    };
}
#endif
