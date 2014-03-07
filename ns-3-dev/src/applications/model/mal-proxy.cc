/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/tcp-socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/realtime-simulator-impl.h"
#include <sys/time.h>

#include "mal-proxy.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ns3/ipv4.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/tap-bridge.h"

using namespace std;

static bool global_consult_gatling = true;
static short global_once = 0;
static int app_debug = 0;

#define CTRL_IP "127.0.0.1"
#define CTRL_PORT 8888
//#define TCP_CONTROLLER 1
#define UDP_CONTROLLER 1

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("MalProxyApplication");
NS_OBJECT_ENSURE_REGISTERED(MalProxy);

void MalProxy::ClearStrategyForMsg(int type)
{
	deliveryActions[type][NONE] = false;
	deliveryActions[type][DROP] = false;
	deliveryActions[type][DUP] = false;
	deliveryActions[type][DELAY] = false;
	deliveryActions[type][DIVERT] = false;
	deliveryActions[type][REPLAY] = false;
	deliveryActions[type][LIE] = false;
	deliveryActions[type][BURST] = false;
	deliveryActions[type][INJECT] = false;
	deliveryActions[type][WINDOW] = false;
	deliveryActions[type][RETRY] = false;
	for (int j = 0; j < FIELD; j++) {
		if (lyingValues[type][j] != NULL) {
			delete lyingValues[type][j];
			lyingValues[type][j] = NULL;
		}
	}
	burst_sched[type]=false;
	burst[type].clear();
}

void MalProxy::ClearStrategy()
{
	for (int i = 0; i < MSG; i++) {
		deliveryActions[i][NONE] = false;
		deliveryActions[i][DROP] = false;
		deliveryActions[i][DUP] = false;
		deliveryActions[i][DELAY] = false;
		deliveryActions[i][DIVERT] = false;
		deliveryActions[i][REPLAY] = false;
		deliveryActions[i][LIE] = false;
		deliveryActions[i][BURST] = false;
		deliveryActions[i][INJECT] = false;
		deliveryActions[i][WINDOW] = false;
		deliveryActions[i][RETRY] = false;
		for (int j = 0; j < FIELD; j++) {
			if (lyingValues[i][j] != NULL) {
				delete lyingValues[i][j];
				lyingValues[i][j] = NULL;
			}
		}
		burst_sched[i]=false;
		burst[i].clear();
	}
}

void MalProxy::ParseStrategy(string line)
{
	ClearStrategy();
	AddStrategy(line);
}

void MalProxy::AddStrategy(string line)
{

	NS_LOG_DEBUG("Adding new strategy " << line);
	if (app_debug > 0) {
		std::cout << "MALProxy] " << "Adding new strategy " << line
				<< std::endl;
	}
	while (line.length() > 0) {
		NS_LOG_DEBUG("left " << line << " " << line.length());
		int cur = line.find(" ");
		int msgType = Message::StrToType(line.substr(0, cur).c_str());
		line = line.substr(cur + 1);

		cur = line.find(" ");
		string malact = line.substr(0, cur);
		line = line.substr(cur + 1);

		cur = line.find(" ");
		string value;
		if (cur != -1) {
			value = line.substr(0, cur);
			line = line.substr(cur + 1);
		} else {
			value = line;
			line = "";
		}

		NS_LOG_DEBUG("msgType " << msgType << " action " << malact << " value " << value);
		if (app_debug > 1) {
			std::cout << "MALProxy] " << "msgType " << msgType << " action "
					<< malact << " value " << value << std::endl;
		}

		if (malact == "NONE") {
			deliveryActions[msgType][NONE] = true;
			deliveryValues[msgType][NONE] = 0;
		} else if (malact == "DROP") {
			deliveryActions[msgType][DROP] = true;
			deliveryValues[msgType][DROP] = atof(value.c_str());
			;
		} else if (malact == "DUP") {
			deliveryActions[msgType][DUP] = true;
			deliveryValues[msgType][DUP] = atof(value.c_str());
		} else if (malact == "DELAY") {
			deliveryActions[msgType][DELAY] = true;
			deliveryValues[msgType][DELAY] = atof(value.c_str());
		} else if (malact == "DIVERT") {
			deliveryActions[msgType][DIVERT] = true;
			deliveryValues[msgType][DIVERT] = atof(value.c_str());
		} else if (malact == "REPLAY") {
			deliveryActions[msgType][REPLAY] = true;
			deliveryValues[msgType][REPLAY] = atof(value.c_str());
		} else if (malact == "BURST") {
			deliveryActions[msgType][BURST] = true;
			deliveryValues[msgType][BURST] = atof(value.c_str());
		} else if (malact == "LIE") {
			cur = line.find(" ");
			int field;
			if (cur != -1) {
				field = atoi(line.substr(0, cur).c_str());
				line = line.substr(cur + 1);
			} else {
				field = atoi(line.c_str());
				line = "";
			} NS_LOG_DEBUG("lying on field " << field);
			lyingValues[msgType][field] = strdup(value.c_str());
		}
		//INJECT, WINDOW are much more complicated

	}

}

TypeId MalProxy::GetTypeId(void)
{
	static TypeId tid =
			TypeId("ns3::MalProxy").SetParent<Application>().AddConstructor<
					MalProxy>().AddAttribute("Local",
					"The Address on which to Bind the rx socket.",
					Ipv4AddressValue(),
					MakeIpv4AddressAccessor(&MalProxy::m_local),
					MakeIpv4AddressChecker()).AddAttribute("UDPPort",
					"Port on which we listen for incoming packets.",
					UintegerValue(9),
					MakeUintegerAccessor(&MalProxy::m_udp_port),
					MakeUintegerChecker<uint16_t>()).AddAttribute("TCPPort",
					"Port on which we listen for incoming packets.",
					UintegerValue(9),
					MakeUintegerAccessor(&MalProxy::m_tcp_port),
					MakeUintegerChecker<uint16_t>());
	return tid;
}

MalProxy::MalProxy()
{
	NS_LOG_FUNCTION_NOARGS ();
	conn_list.clear();
	evt_resume = false;
	ctrltype = STATEBASED;
}

MalProxy::~MalProxy()
{
	NS_LOG_FUNCTION_NOARGS ();
	m_udp_socket = 0;
	m_tcp_socket = 0;
}

void MalProxy::DoDispose(void)
{
	NS_LOG_FUNCTION_NOARGS ();
	Application::DoDispose();
}

int MalProxy::Command(string command)
{
	if (command.compare("Gatling Pause\n") == 0) {
		global_consult_gatling = false;
		return 1;
	}
	if (command.compare("Gatling Resume\n") == 0) {
		global_consult_gatling = true;
		return 1;
	}
	if (command.compare(0, strlen("Learned"), "Learned") == 0) {
		string line = string(command.c_str() + strlen("Learned") + 1);
		if (app_debug > 1) {
			std::cout << "MALProxy] " << "learning " << line << " from "
					<< command << std::endl;
		}
		int cur = line.find(" ");
		int msgType = Message::StrToType(line.substr(0, cur).c_str());
		m_learned[msgType] = line;

		ClearStrategy();
		std::map<int, std::string>::iterator it;
		for (it = m_learned.begin(); it != m_learned.end(); it++) {
			AddStrategy(it->second);
		}
		return 1;
	}
	if (command.compare(0, strlen("Once"), "Once") == 0) {
		string line = string(command.c_str() + strlen("Once") + 1);
		if (app_debug > 1) {
			std::cout << "MALProxy] " << "Once " << line << std::endl;
		}
		ParseStrategy(line.c_str());
		global_once = 1;
		return 1;
	}
	if (app_debug > 1) {
		std::cout << "MALProxy] " << "command : " << command << std::endl;
	}
	ParseStrategy(command);
	return 1;
}

void MalProxy::StartApplication(void) {
	NS_LOG_FUNCTION_NOARGS ();

	ClearStrategy();
	if (m_udp_port != 0) {
		if (m_udp_socket == 0) {
			TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
			m_udp_socket = Socket::CreateSocket(GetNode(), tid);
			InetSocketAddress local = InetSocketAddress(m_local, m_udp_port);
			//InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_udp_port);
			m_udp_socket->Bind(local);
		}
	}

	if (m_tcp_port != 0) {
		if (m_tcp_socket == 0) {
			TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
			m_tcp_socket = Socket::CreateSocket(GetNode(), tid);
			InetSocketAddress local = InetSocketAddress(m_local, m_tcp_port);
			int res = m_tcp_socket->Bind(local);
			m_tcp_socket->Listen();
			NS_LOG_INFO("MalProxy local address:  " << m_local << " port: " << m_tcp_port << " bind: " << res );
		}
	}
	srand((unsigned) time(0));
}

void MalProxy::StopApplication()
{
	NS_LOG_FUNCTION_NOARGS ();

	if (m_udp_socket != 0) {
		m_udp_socket->Close();
	}

	if (m_tcp_socket != 0) {
		m_tcp_socket->Close();
	}
}

/*Communicate back to the Controller. Currently only used for Greedy Search.*/
int MalProxy::CommunicateController(Message *m)
{
#ifdef UDP_CONTROLLER
	int sockfd, n;
	char read_buffer[256], send_buffer[256];
	struct sockaddr_in serv_addr;
	sprintf(send_buffer, "%d", m->type); // send Msg Type
	std::cout << "MALProxy] " << "Asking Controller about Msg " << m->type
			<< std::endl;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(CTRL_IP);
	serv_addr.sin_port = htons(CTRL_PORT);
	if (sendto(sockfd, send_buffer, strlen(send_buffer), 0,
			(struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		std::cout << "MALProxy] " << "Sending to controller failed!!"
				<< std::endl;
	}
	close(sockfd);
	return 0;
#else
#ifdef TCP_CONTROLLER
	int sockfd, n;
	char read_buffer[256], send_buffer[256];
	struct sockaddr_in serv_addr;
	sprintf(send_buffer, "%d", m->type); // send Msg Type

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(CTRL_IP);
	serv_addr.sin_port = htons(CTRL_PORT);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cout << "MALProxy] " << "Sending to controller failed!!" << std::endl;
		return NONE;
	}

	write(sockfd, send_buffer, strlen(send_buffer));
	close(sockfd);
	n = read(sockfd, read_buffer, 255);
	if(app_debug>1) {std::cout << "MALProxy] " << "READ FROM CTRL: " << read_buffer << "]" << std::endl;}
	// XXX - I should consider to use shorter strings or numbers instead of
	// long strings. leave it for now
	if (strncmp(read_buffer, "Learned", strlen("Learned")) == 0) {
		m_learned[m->type] = string(read_buffer+strlen("Learned")+1);
		//AddStrategy(m_learned[m->type]);
		ParseStrategy(m_learned[m->type]);
		// add this to learned vector
	}
	if (strncmp(read_buffer, "Once", strlen("Once")) == 0) {
		ParseStrategy(read_buffer+strlen("Once")+1);
		//AddStrategy(m_learned[m->type]);
		global_once = 1;
	}
	else {
		if (strncmp(read_buffer, "Msg NONE", strlen("Msg NONE")) == 0) {
			// do nothing
			return 0;
		}
		// otherwise we will get a new command // start from C ?
		if (strncmp(read_buffer, "C ", strlen("C ")) == 0) {
			ParseStrategy(read_buffer + 2);
			//ClearStrategyForMsg(m->type);
			//AddStrategy(read_buffer + 2);
		}
		else std::cout << "MALProxy] " << "Something wrong. I don't understand the command" << std::endl;
	}
	/* strategy should be processed via command interface
	 else { // otherwise it's a strategy
	 }
	 */
	return n;
#endif
#endif
}

int MalProxy::MalMsg(Message *m)
{
	/*Safety Check*/
	if (m->type < 0 || m->type >= MSG) {
		return 0;
	}

	if (global_once == 1) {
		if (app_debug > 0) {
			std::cout << "once executed" << std::endl;
		}
		global_once = 2;
		return 1;
	}

	/*Greedy Strategy---Check with Controller, if we don't know what
	 * to do for this message type*/
	if (ctrltype == GREEDY) {
		if (global_consult_gatling) {
			if (m_learned.find(m->type) != m_learned.end()) {
				return 1;
			}
			if (app_debug > 0) {
				std::cout << "MALProxy] "
						<< "Determining Malicious action for type: " << m->type
						<< std::endl;
			}
			if (m->encMsg != NULL) {
				Message *cur = m->encMsg;
				if (cur->type > 0 && m->type < MSG) {
					CommunicateController(cur);
				} else {
					CommunicateController(m);
				}
			} else {
				CommunicateController(m);
			}
			/*Pause simulator to wait for response!*/
			Simulator::ToggleFreeze(true);
			return 2;
		}
	}

	/*See if we should modify this message*/

	if (m->encMsg != NULL) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "Encapsulated msg type "
					<< m->encMsg->type << std::endl;
		}
		return MalMsg(m->encMsg);
	}

	if (m->type < 0 || m->type >= MSG) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "invalid type " << m->type << ":"
					<< MSG << std::endl;
		}
		return 0;
	}

	if (deliveryActions[m->type][DROP]) {
		return 1;
	}

	for (int i = 0; i < FIELD; i++) {
		if (lyingValues[m->type][i] != NULL) {
			if (app_debug > 0) {
				std::cout << "MALProxy] " << "will lie for " << m->type
						<< std::endl;
			}
			return 1;
		}
	}

	if (deliveryActions[m->type][DUP]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will dup for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[m->type][DELAY]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will delay for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[m->type][DIVERT]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will divert for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[m->type][REPLAY]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will replay for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[m->type][BURST]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will burst for " << m->type
					<< std::endl;
		}
		return 1;
	}

	return 0;
}

int MalProxy::MaliciousStrategy(Message *m, maloptions *res)
{
	bool lie = false;

	/*Safety Check*/
	if (m->type < 0 || m->type >= MSG) {
		return 0;
	}

	if (global_once == 2) {
		global_once = 0;
	}

	/*Modify Message as indicated by our strategy*/
	Message *cur = m;
	while (cur != NULL) {
		if (cur->type >= 0 && cur->type < MSG) {

			if (deliveryActions[cur->type][DROP]) {
				double prob = 100 * (double) rand() / (double) RAND_MAX;
				if (prob <= deliveryValues[cur->type][DROP]) {
					NS_LOG_INFO("MalProxy dropping message");
					res->action = DROP;
					return DROP;
				}
			}

			for (int i = 0; i < FIELD; i++) {
				if (lyingValues[cur->type][i] != NULL) {
					lie = true;
				}
			}

			if (deliveryActions[cur->type][DUP]) {
				NS_LOG_INFO("MalProxy duplicating message " << (int)deliveryValues[cur->type][DUP] << " times");
				if (res->duptimes < (int) deliveryValues[cur->type][DUP]) {
					res->duptimes = (int) deliveryValues[cur->type][DUP];
				}
			}

			if (deliveryActions[cur->type][DIVERT]) {
				NS_LOG_INFO("MalProxy diverting message" << cur->type);
				res->divert = true;
			}

			if (deliveryActions[cur->type][DELAY]) {

				if (res->delay < deliveryValues[cur->type][DELAY]) {
					res->delay = deliveryValues[cur->type][DELAY];
				}
				if (deliveryValues[cur->type][DELAY] < 0) {
					res->delay = -1 * deliveryValues[cur->type][DELAY]
							* (double) rand() / (double) RAND_MAX;
				} NS_LOG_INFO("MalProxy delaying message " << res->delay << " seconds");
			}

			if (deliveryActions[cur->type][REPLAY]) {
				// By default, replay packet will go to the original sender. It can be used with DIVERT/DUP.
				// If BROADCAST is good, we can work on that as well.
				res->replay = (int) deliveryValues[cur->type][REPLAY];
			}

			if (deliveryActions[cur->type][BURST]) {
				res->burst = true;
			}

		}
		cur = cur->encMsg;
	}

	if (lie == true) {
		cur = m;
		while (cur != NULL) {
			if (cur->type >= 0 && cur->type < MSG) {

				for (int i = 0; i < FIELD; i++) {
					if (lyingValues[cur->type][i] != NULL) {
						cur->ChangeValue(i, lyingValues[cur->type][i]);
					}
				}
			}
			cur = cur->encMsg;
		}

	}
	res->action = DELAY;
	return DELAY;
}

int MalProxy::MalTCP(Ptr<Packet> packet, lowerLayers ll, maloptions *res)
{
	TcpHeader tcph;
	connection *c;
	std::vector<seq_state> *seq_list;
	std::vector<seq_state> *ack_list;
	int seq_offset = 0;
	int ack_offset = 0;
	Message *m;
	char tmp[32];

	//tcph.EnableChecksums();
	//tcph.InitializeChecksum(ip.GetSource(), ip.GetDestination(), TcpL4Protocol::PROT_NUMBER);
	res->action = NONE;
	packet->PeekHeader(tcph);

	if ((ll.dir == FROMTAP && tcph.GetDestinationPort() != this->m_tcp_port)
			|| (ll.dir == TOTAP && tcph.GetSourcePort() != this->m_tcp_port)) {
		/*Packet we don't care about*/
		res->action = NONE;
		return NONE;
	}

	if (ll.dir == FROMTAP) {
		/*Forward direction, from "malicious" host*/
		c = FindConnection(ll.iph.GetSource(), ll.iph.GetDestination(),
				tcph.GetSourcePort(), tcph.GetDestinationPort());
		seq_list = &(c->d1);
		ack_list = &(c->d2);
	} else {
		/*Reverse direction, from a victim*/
		c = FindConnection(ll.iph.GetDestination(), ll.iph.GetSource(),
				tcph.GetDestinationPort(), tcph.GetSourcePort());
		seq_list = &(c->d2);
		ack_list = &(c->d1);

	}

	seq_offset = FindOffset(seq_list, tcph.GetSequenceNumber());
	ack_offset = FindOffset(ack_list, tcph.GetAckNumber());

	m = new Message(packet->PeekDataForMal());

	std::cout << ntohs(((BaseMessage*) m->msg)->src) << "-->"
			<< ntohs(((BaseMessage*) m->msg)->dest) << std::endl;
	std::cout << "Type: " << m->FindMsgType() << " ("
			<< m->TypeToStr(m->FindMsgType()) << ")" << std::endl;
	std::cout << "Size: " << m->FindMsgSize() << " (" << m->FindMsgSize() * 4
			<< ")" << std::endl;

	sprintf(tmp, "+%i\n", seq_offset);
	m->ChangeBaseMessage(2, tmp);
	sprintf(tmp, "+%i\n", ack_offset);
	m->ChangeBaseMessage(3, tmp);

	int result = MalMsg(m);
	if (result == 0) {
		res->action = NONE;
		return NONE;
	} else if (result == 2) {
		res->action = RETRY;
		return RETRY;
	}
	MaliciousStrategy(m, res);

	if (res->burst){
		burst[m->type].push_back(std::make_pair(packet,ll));
		if (!burst_sched[m->type]) {
			Simulator::Schedule(Time(Seconds(deliveryValues[m->type][BURST])),
					&MalProxy::Burst, this, m->type);
			burst_sched[m->type] = true;
		}
		res->action = DROP;
	}

	delete (m);

	packet->PeekHeader(tcph);
	std::cout << tcph.GetSourcePort() << "!--->" << tcph.GetDestinationPort()
			<< std::endl;
	/*Checksum will be recomputed as actions are applied... particularly for Divert*/
	return res->action;
}

MalProxy::connection* MalProxy::FindConnection(Ipv4Address src,	Ipv4Address dest, int sport, int dport)
{
	connection c;

	for (int i = 0; i < conn_list.size(); i++) {
		if (conn_list[i].ip_dest == dest && conn_list[i].ip_src == src
				&& conn_list[i].port_dest == dport
				&& conn_list[i].port_src == sport) {
			return &conn_list[i];
		}
	}

	c.ip_dest = dest;
	c.ip_src = src;
	c.port_dest = dport;
	c.port_src = sport;
	conn_list.push_back(c);
	return &conn_list[conn_list.size() - 1];
}

int MalProxy::FindOffset(std::vector<seq_state> *lst, SequenceNumber32 seq)
{
	if (lst->empty()) {
		return 0;
	}

	if (seq >= (*lst)[lst->size() - 1].start_seq) {
		return (*lst)[lst->size() - 1].offset;
	}

	for (int i = lst->size() - 2; i >= 0; i--) {
		if (seq >= (*lst)[i].start_seq) {
			return (*lst)[i].offset;
		}
	}

	return 0;
}

void MalProxy::AddOffset(std::vector<seq_state> *lst, SequenceNumber32 seq,int offset)
{
	seq_state s;

	if (seq <= (*lst)[lst->size() - 1].start_seq) {
		/*WTF?*/
		return;
	}

	s.start_seq = seq;
	s.offset = offset;
	lst->push_back(s);
	return;
}

void MalProxy::StoreEvent(EventImpl *event)
{
	sav_evt = event;
}

void* MalProxy::Save()
{
	return sav_evt;
}

void MalProxy::Load(void *ptr)
{
	sav_evt = (EventImpl*) ptr;
	if (ptr != NULL) {
		evt_resume = true;
		//Ptr<RealtimeSimulatorImpl> impl = DynamicCast<RealtimeSimulatorImpl> (Simulator::GetImplementation ());
		//impl->ScheduleRealtimeNow(sav_evt);
	}
}

void MalProxy::Resume()
{
	if (evt_resume == true && sav_evt != NULL) {
		sav_evt->Invoke();
		evt_resume = false;
	}
}

//type databytes ip_src ip_dest 0=port_src 1=port_dest 2=seq 3=ack 4=reserved 5=type 6=urg 7=ece 8=cwr 9=window 11=urgptr
void MalProxy::InjectPacket(char *spec)
{
	int databytes;
	Ptr<Packet> p;
	TcpHeader tcph;
	Message *m;
	int type;
	int len = 0;
	char ssrc[100];
	char sdest[100];
	Ipv4Address src;
	Ipv4Address dest;

	sscanf(spec, "%i %i %s %s%n", &type, &databytes, ssrc, sdest, &len);
	src = Ipv4Address(ssrc);
	dest = Ipv4Address(sdest);
	spec += len;
	p = new Packet(databytes);

	tcph.EnableChecksums();
	tcph.InitializeChecksum(src, dest, TcpL4Protocol::PROT_NUMBER);
	p->AddHeader(tcph);
	m = new Message(p->PeekDataForMal());
	m->CreateMessage(type, spec);

	p->RemoveHeader(tcph);
	p->AddHeader(tcph); //checksum

	DoInjectPacket(p, src, dest);
	return;
}

void MalProxy::DoInjectPacket(Ptr<Packet> p, Ipv4Address src, Ipv4Address dest)
{
	Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();
	if (ipv4 != 0) {
		Ipv4Header header;
		header.SetDestination(dest);
		header.SetProtocol(TcpL4Protocol::PROT_NUMBER);
		Socket::SocketErrno errno_;
		Ptr<Ipv4Route> route;
		Ptr<NetDevice> oif(0); //specify non-zero if bound to a source address
		if (ipv4->GetRoutingProtocol() != 0) {
			route = ipv4->GetRoutingProtocol()->RouteOutput(p, header, oif,
					errno_);
		} else {
			NS_LOG_ERROR ("No IPV4 Routing Protocol");
			route = 0;
		}
		ipv4->Send(p, src, dest, TcpL4Protocol::PROT_NUMBER, route);
	}
}

void MalProxy::Burst(int type)
{
	if (burst_sched[type] == false) {
		return;
	}

	for (int i = 0; i < burst[type].size(); i++) {
		burst[type][i].second.iph.EnableChecksum();
		burst[type][i].first->AddHeader(burst[type][i].second.iph);
		((TapBridge*)(burst[type][i].second.obj))->SendPacket(burst[type][i].first,burst[type][i].second);
	}
	burst[type].clear();
	burst_sched[type] = false;
}

//w=window t=type ip_src ip_dest port_src port_dest
void MalProxy::Window(char* spec)
{
	int window;
	int type;
	int len;
	int seq;
	int itter;
	char ip_src[100];
	char ip_dest[100];
	char p_src[100];
	char p_dest[100];
	char pspec[1000];

	sscanf(spec, "w=%i t=%i %s %s %s %s", &window, &type, ip_src, ip_dest,
			p_src, p_dest);

	itter = (0xFFFFFFFF) / window;

	seq = 0;
	for (int i = 0; i < itter; i++) {
		snprintf(pspec, 1000, "%i 0 %s %s 0=%s 1=%s 2=%i 5=%i", type, ip_src,
				ip_dest, p_src, p_dest, seq, type);
		InjectPacket(pspec);
		seq += window;
	}
}

} // Namespace ns3

