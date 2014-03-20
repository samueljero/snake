#ifndef TRANSITION_H
#define TRANSITION_H
using namespace std;
namespace ns3 {
    class Transition
    {
        string m_rcvd;
        string m_send;
        class State m_from;
        class State m_to;
        public:
        inline string Rcvd() { return m_rcvd; }
        inline string Send() { return m_send; }
        class State From() { return m_from; }
        class State To() { return m_to; }

        Transition
            (string rcvd, string send) {
                m_rcvd = rcvd;
                m_send = send;
                m_from = State("empty");
                m_to = State("empty");
            }

        Transition
            (string rcvd, string send, string from, string to) {
                m_rcvd = rcvd;
                m_send = send;
                m_from = State(from);
                m_to = State(to);
            }
        bool compare(Transition target) {
            if (m_from.compare(target.From()) != 0) return (m_from.compare(target.From()));
            if (m_rcvd.compare(target.Rcvd()) != 0) return (m_rcvd.compare(target.Rcvd()));
            if (m_to.compare(target.To()) != 0) return (m_to.compare(target.To()));
            return m_send.compare(target.Send());
        }
        friend ostream& operator<< (ostream& os, const Transition& tr);
    };
}
#endif
