#ifndef TRANSITION_H
#define TRANSITION_H
#include <string>
#include <cstdio>

using namespace std;
namespace ns3 {
    class Transition
    {
        string m_rcvd;
        string m_send;
        int m_type;
        int m_triggerMesage;
        public:
        int m_rcvdType;
        int m_sendType;
        void SetTriggerMsg(int msgType) {m_triggerMesage = msgType; }
        int GetTriggerMsg() { return m_triggerMesage;}
        int GetType() {return m_type;}
        inline string Rcvd() { return m_rcvd; }
        inline string Send() { return m_send; }

        Transition
            (int type, string rcvd, string send) {
                m_type = type;
                m_rcvd = rcvd;
                m_send = send;
                m_rcvdType = GetMsgType(rcvd);
                m_sendType = GetMsgType(send);
            }
        Transition (const Transition& t) {
            m_type = t.m_type;
            m_rcvd = t.m_rcvd;
            m_send = t.m_send;
            m_rcvdType = GetMsgType(m_rcvd);
            m_sendType = GetMsgType(m_send);
        }

        Transition() {}

        friend ostream& operator<< (ostream& os, const Transition& tr);
        friend bool operator== (const Transition& l, const Transition& r);
        friend bool operator!= (const Transition& l, const Transition& r);
        friend bool operator< (const Transition& l, const Transition& r);
        friend bool operator> (const Transition& l, const Transition& r);
        static int GetTransitionType(int rcvd, int send);
        static int GetMsgType(string msgName);
        static int GetMsgTypeFromAction(string action);
    };
}
#endif
