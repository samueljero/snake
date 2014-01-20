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
#include <sys/time.h>


#include "mal-proxy.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

static bool global_consult_gatling = true;
static short global_once = 0;
static int app_debug=0;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MalProxyApplication");
NS_OBJECT_ENSURE_REGISTERED (MalProxy);


void MalProxy::PrintPairs() {
        std::map<Ptr<Socket>, Ptr<Socket> >::iterator pi;
        NS_LOG_LOGIC("**********************************************************");
        for (pi = m_pair.begin(); pi != m_pair.end(); pi++) {
                NS_LOG_LOGIC(" [ " << (*pi).first << " ] -> [ " << (*pi).second << " ]");
        }
        std::map<Ptr<Socket>, Ipv4Address>::iterator ci;
        NS_LOG_LOGIC("**********************************************************");
        for (ci = m_tcp_conn.begin(); ci != m_tcp_conn.end(); ci++) {
                NS_LOG_LOGIC(" [ " << (*ci).first << " ] -> [ " << (*ci).second << " ]");
        }

}

void MalProxy::ClearStrategyForMsg(int type) {
  deliveryActions[type][NONE] = false;
  deliveryActions[type][DROP] = false;
  deliveryActions[type][DUP] = false;
  deliveryActions[type][DELAY] = false;
  deliveryActions[type][DIVERT] = false;
  deliveryActions[type][REPLAY] = false;
  for (int j = 0; j < FIELD; j++) {
    if (lyingValues[type][j] != NULL) {
      delete lyingValues[type][j];
      lyingValues[type][j] = NULL;
    }
  }
}


void MalProxy::ClearStrategy() {
	for (int i = 0; i < MSG; i++) {
		deliveryActions[i][NONE] = false;
		deliveryActions[i][DROP] = false;
		deliveryActions[i][DUP] = false;
		deliveryActions[i][DELAY] = false;
		deliveryActions[i][DIVERT] = false;
		deliveryActions[i][REPLAY] = false;
		for (int j = 0; j < FIELD; j++) {
			if (lyingValues[i][j] != NULL) {
				delete lyingValues[i][j];
				lyingValues[i][j] = NULL;
			}
		}
	}

}

void MalProxy::ParseStrategy(string line) {
	ClearStrategy();
  AddStrategy(line);
}

void MalProxy::AddStrategy(string line) {

	NS_LOG_DEBUG("Adding new strategy " << line);
	if(app_debug>0){std::cout << "MALProxy] " << "Adding new strategy " << line << std::endl;}
	while (line.length() > 0) {
		NS_LOG_DEBUG("left " << line << " " << line.length());
		int cur = line.find(" ");
		int msgType = Message::StrToType(line.substr(0,cur).c_str());
		line = line.substr(cur+1);
		
		cur = line.find(" ");
		string malact = line.substr(0,cur);
		line = line.substr(cur+1);

		cur = line.find(" ");
		string value;
		if (cur != -1) {
			value = line.substr(0,cur);
			line = line.substr(cur+1);
		} else {
			value = line;
			line = "";
		}

		NS_LOG_DEBUG("msgType " << msgType << " action " << malact << " value " << value);
		if(app_debug>1){std::cout << "MALProxy] " << "msgType " << msgType << " action " << malact << " value " << value << std::endl;}

		if (malact == "NONE") {
			deliveryActions[msgType][NONE] = true;
			deliveryValues[msgType][NONE] = 0;
		} else if (malact == "DROP") {
			deliveryActions[msgType][DROP] = true;
			deliveryValues[msgType][DROP] = atof(value.c_str());;
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
		} else if (malact == "LIE") {
			cur = line.find(" ");
			int field;
			if (cur != -1) {
				field = atoi(line.substr(0,cur).c_str());
				line = line.substr(cur+1);
			} else {
				field = atoi(line.c_str());
				line = "";
			}
			NS_LOG_DEBUG("lying on field " << field);
			lyingValues[msgType][field] = strdup(value.c_str());
		}
			
	}

}



TypeId
MalProxy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MalProxy")
    .SetParent<Application> ()
    .AddConstructor<MalProxy> ()
    .AddAttribute ("Local", "The Address on which to Bind the rx socket.",
                   Ipv4AddressValue (),
                   MakeIpv4AddressAccessor (&MalProxy::m_local),
                   MakeIpv4AddressChecker ())
    .AddAttribute ("UDPPort", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&MalProxy::m_udp_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("TCPPort", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&MalProxy::m_tcp_port),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

MalProxy::MalProxy ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

MalProxy::~MalProxy()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_udp_socket = 0;
  m_tcp_socket = 0;
}

void
MalProxy::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

int
MalProxy::Command(string command)
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
	string line = string(command.c_str() + strlen("Learned")+1);
	if(app_debug>1){std::cout << "MALProxy] " << "learning " << line << " from " << command << std::endl;}
	int cur = line.find(" ");
	int msgType = Message::StrToType(line.substr(0, cur).c_str());
	m_learned[msgType] = line;

	ClearStrategy();
	std::map<int,std::string>::iterator it;
	for(it=m_learned.begin(); it!=m_learned.end();it++){
		AddStrategy(it->second);
	}
	return 1;
  }
	if (command.compare(0, strlen("Once"), "Once") == 0) {
    string line = string(command.c_str() + strlen("Once")+1);
    if(app_debug>1){std::cout << "MALProxy] " <<"Once " << line << std::endl;}
		ParseStrategy(line.c_str());
    global_once = 1;
    return 1;
  }
	if(app_debug>1){std::cout << "MALProxy] " <<"command : " <<command << std::endl;}
	ParseStrategy(command);
	return 1;
}

void 
MalProxy::StartApplication (void)
{
	NS_LOG_FUNCTION_NOARGS ();
	
	ClearStrategy();
	if (m_udp_port != 0) {
		if (m_udp_socket == 0)
		{
			TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
			m_udp_socket = Socket::CreateSocket (GetNode (), tid);
			InetSocketAddress local = InetSocketAddress (m_local, m_udp_port);
			//InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_udp_port);
			m_udp_socket->Bind (local);
		}

		m_udp_socket->SetRecvCallback (MakeCallback (&MalProxy::HandleUDPRead, this));
		//m_udp_socket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),MakeCallback (&MalProxy::HandleUDPAccept, this));
		m_udp_socket->SetCloseCallbacks(MakeCallback(&MalProxy::HandleClose, this), MakeCallback(&MalProxy::HandleClose, this));
	}

	if (m_tcp_port != 0) {
		if (m_tcp_socket == 0)
		{
			TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
			m_tcp_socket = Socket::CreateSocket (GetNode (), tid);
			InetSocketAddress local = InetSocketAddress (m_local, m_tcp_port);
			int res = m_tcp_socket->Bind (local);
			m_tcp_socket->Listen();
			NS_LOG_INFO("MalProxy local address:  " << m_local << " port: " << m_tcp_port << " bind: " << res );
		}

		m_tcp_socket->SetRecvCallback (MakeCallback (&MalProxy::HandleTCPRead, this));
		m_tcp_socket->SetAcceptCallback (
				MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
				MakeCallback (&MalProxy::HandleAccept, this));
		// the first one is for normalClose and the other is for errorClose
		m_tcp_socket->SetCloseCallbacks(MakeCallback(&MalProxy::HandleClose, this), MakeCallback(&MalProxy::HandleClose, this));
	}
	srand((unsigned)time(0));
}

void MalProxy::HandleClose(Ptr<Socket> s1)
{
	NS_LOG_INFO(" PEER CLOSE ");
	NS_LOG_INFO("**********************************************************");

	if (s1->m_pair) {
		s1->m_pair->Close();
		s1->m_pair->m_pair = NULL;
		s1->m_pair = NULL;
	}
}

void 
MalProxy::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_udp_socket != 0) 
    {
      m_udp_socket->Close ();
      m_udp_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  if (m_tcp_socket != 0)
    {
      m_tcp_socket->Close ();
      m_tcp_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

}

/* TODO: 
malproxy should consult the controller to see what to do upon receiving a
message.  as if an action is learned, this can be unnecessary overhead. to
prevent this, the controller should signal that it doesn't want to branch
anymore on a certain message */
	void
MalProxy::HandleUDPRead (Ptr<Socket> socket)
{
/*
	ProfileFunction("HandleUDPRead", true);

	Ptr<Packet> packet;
	Address from;

	//TODO: Malproxy breaks if UDP packet sent is very large (probably > 1500)

	while (packet = socket->RecvFrom (from))
	{

		NS_LOG_LOGIC ("UDP " << m_local << "Received " << packet->GetSize () << " bytes from " <<
				InetSocketAddress::ConvertFrom (from).GetIpv4 ());
		NS_LOG_LOGIC ("PACKET ID: " << packet->GetUid() << " SIZE: " << packet->GetSize());
		//std::cout << "Got packet with size " << packet->GetSize() << std::endl;

		Ipv4Address dst;
		MaliciousTag mtag_old;
		MaliciousTag mtag_new_in;
		MaliciousTag mtag_new_out;

		packet->RemovePacketTag(mtag_old);
		mtag_old.Print(std::cerr);
		dst = mtag_old.GetIPDestination();
		uint16_t dst_port = mtag_old.GetDestPort();

		mtag_new_in.SetHardwareDestination(mtag_old.GetHardwareDestination());
		mtag_new_in.SetTimer(mtag_old.GetSec(), mtag_old.GetUSec());
		mtag_new_out.SetHardwareDestination(mtag_old.GetHardwareDestination());
		mtag_new_out.SetTimer(mtag_old.GetSec(), mtag_old.GetUSec());

		if (InetSocketAddress::IsMatchingType (from))
		{
			mtag_old.Print(std::cerr);

			//uint8_t *msg_data = new uint8_t[packet->GetSize()];
			//packet->CopyData(msg_data, packet->GetSize());
			//Message *msg = new Message(msg_data);
			Message *msg = new Message((uint8_t*)packet->PeekData());


			if (InetSocketAddress::ConvertFrom(from).GetIpv4() == m_local || deliveryActions[msg->type][REPLAY]) {
				NS_LOG_LOGIC("*********************************************** THIS IS FROM ME DST: " << dst);
				mtag_new_out.SetIPSource(m_local.Get());
				mtag_new_out.SetIPDest(dst.Get());
				NS_LOG_LOGIC("MalProxy will intercept msgType " << msg->type);
			}
			
			if (InetSocketAddress::ConvertFrom(from).GetIpv4() != m_local) {
				dst = m_local;
				NS_LOG_LOGIC("*********************************************** THIS IS FROM OTHER DST: " << dst);
				mtag_new_in.SetSpoof(true);
				mtag_new_in.SetIPSource(InetSocketAddress::ConvertFrom(from).GetIpv4 ().Get());
				mtag_new_in.SetSrcPort (InetSocketAddress::ConvertFrom (from).GetPort ());
				mtag_new_in.SetIPDest(m_local.Get());
				mtag_new_in.SetDestPort(m_udp_port);
				NS_LOG_LOGIC("MalProxy bypass message");
			}

			pair <Ipv4Address, uint16_t> host (dst, dst_port);
			Ptr<Socket> new_socket;	
			//std::cout << "finding host " << dst << ":" << dst_port;
			if (m_udp_conn.find(host) == m_udp_conn.end()) {

				new_socket = Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
				if (InetSocketAddress::ConvertFrom(from).GetIpv4() == m_local) {
					if (new_socket->Bind(m_local) != 0) {
						NS_LOG_LOGIC("BIND on m_local failed");
					}
					//socket->Connect(InetSocketAddress(m_local, InetSocketAddress::ConvertFrom(from).GetPort()));
				} else {
					if (new_socket->Bind(InetSocketAddress::ConvertFrom(from).GetIpv4 ()) != 0) {
						NS_LOG_LOGIC("BIND on from failed");
					}
					//socket->Connect(from);
				}
				new_socket->Connect(InetSocketAddress(dst, dst_port));
				NS_LOG_LOGIC(" PAIR DEST: " << dst << " : " << dst_port);
				new_socket->SetRecvCallback (MakeCallback (&MalProxy::HandleUDPRead, this));
				m_udp_conn[host] = new_socket;
				//std::cout << " new socket";
			} else {
				new_socket = m_udp_conn[host];
			}
			//std::cout << " " << new_socket << std::endl;
			mtag_new_in.SetMalStatus(MaliciousTag::FROMPROXY);
			mtag_new_in.SetSrcNode(GetNode()->GetId());
			mtag_new_in.Print(std::cerr);
			mtag_new_out.SetMalStatus(MaliciousTag::FROMPROXY);
			mtag_new_out.SetSrcNode(GetNode()->GetId());
			mtag_new_out.Print(std::cerr);


			if (InetSocketAddress::ConvertFrom(from).GetIpv4() == m_local) {
				//std::cout << "dst " << dst << ":" << dst_port << " socket: " << new_socket << std::endl;
				MalSendUDPMsg(new_socket, packet, msg, mtag_new_out, packet->GetSize());

				//Ptr<Packet> packet2 = Create<Packet> (m->msg, packet->GetSize());
				//packet2->AddPacketTag(mtag_new);
				//new_socket->Send(packet2);
			} else {
				packet->AddPacketTag(mtag_new_in);
				new_socket->Send(packet);
				if (deliveryActions[msg->type][REPLAY]) {
					//cout << "Replaying message " << m->type << " from " << InetSocketAddress::ConvertFrom(from).GetIpv4() << endl;
					MalSendUDPMsg(socket, packet, msg, mtag_new_out, packet->GetSize());
				}
			}
			//delete msg_data;
			delete msg;

		} 


	}


	ProfileFunction("HandleUDPRead", false);
	num_processed++;
	if (num_processed % 10000 == 0) {
		ProfileFunction("DUMP", false);
	}
	*/
}

#define CTRL_IP "127.0.0.1"
#define CTRL_PORT 8888
//#define TCP_CONTROLLER 1
#define UDP_CONTROLLER 1

int MalProxy::CommunicateController(Message *m)
{
#ifdef UDP_CONTROLLER
	int sockfd, n;
	char read_buffer[256], send_buffer[256];
	struct sockaddr_in serv_addr;
	sprintf(send_buffer, "%d", m->type); // send Msg Type
  std::cout << "MALProxy] " << "Asking Controller about Msg " << m->type << std::endl;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(CTRL_IP);
	serv_addr.sin_port = htons(CTRL_PORT);
  if (sendto(sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		std::cout << "MALProxy] " << "Sending to controller failed!!" << std::endl;
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
	if(app_debug>1){std::cout << "MALProxy] " << "READ FROM CTRL: " << read_buffer << "]" <<  std::endl;}
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

bool
MalProxy::MalMsg(Message *m) {

  // XXX - this should be where to check LEARNED etc
  if (global_once == 1) {
	if(app_debug>0){std::cout <<"once executed" << std::endl;}
    global_once = 2;
    return true;
  }

  if (global_consult_gatling) {
    if (m->type < 0 || m->type >= MSG) {
      return false;
    }
    if (m_learned.find(m->type) != m_learned.end()) {
      return true;
    }
    if(app_debug>0){std::cout << "MALProxy] " << "Determining Malicious action for type: " << m->type << std::endl;}
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
    return true;
  }

	if (m->encMsg != NULL) {
		if(app_debug>0){std::cout << "MALProxy] " << "Encapsulated msg type " << m->encMsg->type << std::endl;}
		if (MalMsg(m->encMsg)) {
			return true;
		}
	}

	if (m->type < 0 || m->type >= MSG) {
		if(app_debug>0){std::cout << "MALProxy] " <<"invalid type " << m->type << ":" << MSG << std::endl;}
		return false;
	}

	if (deliveryActions[m->type][DROP]) {
		return true;
	} 

	for (int i = 0; i < FIELD; i++) {
		if (lyingValues[m->type][i] != NULL) {
			if(app_debug>0){std::cout << "MALProxy] " << "will lie for " << m->type << std::endl;}
			return true;
		}
	}

	if (deliveryActions[m->type][DUP]) {
		if(app_debug>0){std::cout << "MALProxy] " << "will dup for " << m->type << std::endl;}
		return true;	
	} 

	if (deliveryActions[m->type][DELAY]) {
		if(app_debug>0){std::cout << "MALProxy] " << "will delay for " << m->type << std::endl;}
		return true;
	} 

	if (deliveryActions[m->type][DIVERT]) {
		if(app_debug>0){std::cout << "MALProxy] " << "will divert for " << m->type << std::endl;}
		return true;
	}

	if (deliveryActions[m->type][REPLAY]) {
		if(app_debug>0){std::cout << "MALProxy] " << "will replay for " << m->type << std::endl;}
		return true;	
	} 

	return false;
}


// XXX - THIS SHOULD BE COMBINED WITH MALSEND UDP
void
MalProxy::MalSendMsg(Ptr<Socket> socket, Message *m, MaliciousTag mtag, uint32_t size) {

}

void
MalProxy::MalSendUDPMsg(Ptr<Socket> socket, Ptr<Packet> packet, Message *m, MaliciousTag mtag, uint32_t size) {
	ProfileFunction("MalSendUDPMsg", true);

	/*
		struct timeval tv1, tv2, diff;
		struct timezone tz;
		gettimeofday(&tv1, &tz);
	*/

	NS_LOG_INFO("MalProxy intercepting msgtype " << m->type);
	//std::cout << "MalProxy intercepting msgtype " << m->type << " packet size " << size << std::endl;
	// XXX - tell controller that it should start snapshot (whenever?)
	
	int duptimes = 1;
	bool divert = false;
	double delay = 0.0;
	bool lie = false;

	Message *cur = m;
	while (cur != NULL) {	
		if (cur->type >= 0 && cur->type < MSG) {

			if (deliveryActions[cur->type][DROP]) {
				double prob = 100*(double)rand()/(double)RAND_MAX;
				if (prob <= deliveryValues[cur->type][DROP]) {
					NS_LOG_INFO("MalProxy dropping message");
					ProfileFunction("MalSendUDPMsg", false);
					return;
				}
			} 

			for (int i = 0; i < FIELD; i++) {
				if (lyingValues[cur->type][i] != NULL) {
					lie = true;
				}
			}
			
			if (deliveryActions[cur->type][DUP]) {
				NS_LOG_INFO("MalProxy duplicating message " << (int)deliveryValues[cur->type][DUP] << " times");
				if (duptimes < (int)deliveryValues[cur->type][DUP]) {
					duptimes = (int)deliveryValues[cur->type][DUP];
				}
			} 

			if (deliveryActions[cur->type][DIVERT]) {
				NS_LOG_INFO("MalProxy diverting message" << cur->type);
				divert = true;
			}

			if (deliveryActions[cur->type][DELAY]) {

				if (delay < deliveryValues[cur->type][DELAY]) {
					delay = deliveryValues[cur->type][DELAY];
				}
				if (deliveryValues[cur->type][DELAY] < 0) {
					delay = -1*deliveryValues[cur->type][DELAY]*(double)rand()/(double)RAND_MAX;
				}
				NS_LOG_INFO("MalProxy delaying message " << delay << " seconds");
			} 

		}
		cur = cur->encMsg;
	}


	if (lie == true) {
		uint8_t *msg_data = new uint8_t[size];
		packet->CopyData(msg_data, size);
		Message *msg = new Message(msg_data);
		cur = msg;
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
		packet = Create<Packet> (msg_data, size);
		delete msg;
		delete msg_data;

	}


	/*
	//std::cout << "Sending packet" << std::endl;
		gettimeofday(&tv2, &tz);
		diff.tv_sec  = tv2.tv_sec  - tv1.tv_sec;
		diff.tv_usec = tv2.tv_usec - tv1.tv_usec;
		if ( diff.tv_usec < 0 )
		{
			diff.tv_usec = diff.tv_usec + 1000000;
			diff.tv_sec--;
		} 
		//std::cout << "malsendmsg process in " << diff.tv_sec << " " << diff.tv_usec << std::endl;
	*/

	//Ptr<Packet> packet = Create<Packet> (m->msg, size);
	
	packet->AddPacketTag(mtag);
	for (int i = 1; i <= duptimes; ++i) {

		if (divert) {
			int r = rand()%m_udp_conn.size();
			int count = 0;
			std::map<std::pair<Ipv4Address,uint16_t>, Ptr<Socket> >::iterator iter = m_udp_conn.begin();
			while (count++ < r) {
				++iter;	
			}
			mtag.SetIPDest(iter->first.first.Get());
			socket = iter->second;
			
			MaliciousTag tag;
			packet->RemovePacketTag(tag);
			packet->AddPacketTag(mtag);

		}

		if (delay <= 0.0) {
			socket->Send(packet);
		} else {
			Time next(Seconds(delay));
			Simulator::Schedule(next, &MalProxy::SendDelayMsg, this, socket, packet);
		}
	}
	ProfileFunction("MalSendUDPMsg", false);

}

int
MalProxy::MaliciousStrategyUDP(Ptr<Packet> packet, Message *m, uint32_t size,
int *divert, double *delay, int *duptimes, int *replay) {
	bool lie = false;

  if (global_consult_gatling) { // Controlled by the controller. while to use break. will work like if
    // 1. Learned messages
    if (m_learned.find(m->type) != m_learned.end()) {
      // XXX leared actions are already parsed
    }
    else if (m->type < 0)  {
      std::cout << "MALProxy] " << "invalid" << std::endl;
    }
    else {
      //CommunicateController(m);
    }
  } else {
  }
  if (global_once == 2) global_once = 0;
action:
	Message *cur = m;
	while (cur != NULL) {	
		if (cur->type >= 0 && cur->type < MSG) {

			if (deliveryActions[cur->type][DROP]) {
				double prob = 100*(double)rand()/(double)RAND_MAX;
				if (prob <= deliveryValues[cur->type][DROP]) {
					NS_LOG_INFO("MalProxy dropping message");
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
				if (*duptimes < (int)deliveryValues[cur->type][DUP]) {
					*duptimes = (int)deliveryValues[cur->type][DUP];
				}
			} 

			if (deliveryActions[cur->type][DIVERT]) {
				NS_LOG_INFO("MalProxy diverting message" << cur->type);
				*divert = true;
			}

			if (deliveryActions[cur->type][DELAY]) {

				if (*delay < deliveryValues[cur->type][DELAY]) {
					*delay = deliveryValues[cur->type][DELAY];
				}
				if (deliveryValues[cur->type][DELAY] < 0) {
					*delay = -1*deliveryValues[cur->type][DELAY]*(double)rand()/(double)RAND_MAX;
				}
				NS_LOG_INFO("MalProxy delaying message " << *delay << " seconds");
			}

			if (deliveryActions[cur->type][REPLAY]) {
				// By default, replay packet will go to the original sender. It can be used with DIVERT/DUP.
				// If BROADCAST is good, we can work on that as well.
				*replay = (int)deliveryValues[cur->type][REPLAY];
			}

		}
		cur = cur->encMsg;
	}

	if (lie == true) {
		//uint8_t *msg_data = new uint8_t[size];
		//packet->CopyData(msg_data, size);
		//Message *msg = new Message(msg_data);
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
		//packet = Create<Packet> (msg_data, size);
		//delete msg;
		//delete msg_data;

	}
	/*
	for (int i = 1; i <= *duptimes; ++i) {
		
		if (*divert) {
			int r = rand()%m_udp_conn.size();
			int count = 0;
			std::map<std::pair<Ipv4Address,uint16_t>, Ptr<Socket> >::iterator iter = m_udp_conn.begin();
			while (count++ < r) {
				++iter;	
			}
		}

		if (*delay <= 0.0) {
		} else {
			//cout << "delaying " << delay << endl;
			Time next(Seconds(*delay));
			//Simulator::Schedule(next, &MalProxy::SendDelayMsg, this, socket, packet);
		}
	}
	*/
	return DELAY;
}

void
MalProxy::SendDelayMsg(Ptr<Socket> socket, Ptr<Packet> packet) {
	NS_LOG_INFO("MalProxy Sending delayed packet");
	socket->Send(packet);
}

void
MalProxy::HandleTCPRead (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address from;
	Ptr<Socket> pairsocket = m_pair[socket];

	while (packet = socket->RecvFrom (from))
	{

		NS_LOG_LOGIC (m_local << "Received " << packet->GetSize () << " bytes from " <<
				InetSocketAddress::ConvertFrom (from).GetIpv4 ());
		NS_LOG_LOGIC ("PACKET ID: " << packet->GetUid() << " SIZE: " << packet->GetSize());

		Ipv4Address dst;
		MaliciousTag mtag_old;
		MaliciousTag mtag_new;

		//packet->PrintPacketTags(std::cerr);
		packet->RemovePacketTag(mtag_old);
		mtag_old.Print(std::cerr);
		dst = mtag_old.GetIPDestination();

		NS_ASSERT_MSG(m_tcp_conn.find(socket) != m_tcp_conn.end(), "Received a packet from an unknown socket ");
		mtag_new.SetHardwareDestination(mtag_old.GetHardwareDestination());


		if (InetSocketAddress::IsMatchingType (from))
		{
			Message *m = NULL;
			MessageState *ms = NULL;
			if (InetSocketAddress::ConvertFrom (from).GetIpv4 () == m_local) {
				NS_LOG_LOGIC("*********************************************** THIS IS FROM ME");
				mtag_new.SetMalStatus(MaliciousTag::FROMPROXY);
				mtag_new.SetIPSource(InetSocketAddress::ConvertFrom(from).GetIpv4().Get());
				dst = socket->m_orgDestIP;

				ms = &(m_state[pairsocket]);
				if (ms->buffer == NULL) {
					uint8_t *basemsg = new uint8_t[sizeof(BaseMessage)];
					packet->CopyData(basemsg, sizeof(BaseMessage));
					ms->cur = 0;
					ms->size = ((BaseMessage*)basemsg)->size;
					ms->buffer = new uint8_t[ms->size];
					delete basemsg;
				}
				
				do {
					int copySize = packet->GetSize();
                                        std::cout << " in copy size while" << std::endl;
					if (packet->GetSize() > ms->size-ms->cur) {
						copySize = ms->size-ms->cur;
					} else {
						std::cout << "There are extra bytes in this packet that are not associated with the previous message. We are not handling this currently!\n";
					}
					packet->CopyData(ms->buffer+ms->cur, copySize);
					ms->cur += packet->GetSize();
				} while ((packet = socket->RecvFrom(from)) != 0 && ms->cur < ms->size);

				if (ms->cur < ms->size) {
					return;
				}
				m = new Message(ms->buffer);
			} else {
				NS_LOG_LOGIC("*********************************************** THIS IS FROM OTHER");
				dst = m_local;
				mtag_new.SetMalStatus(MaliciousTag::FROMPROXY);
				mtag_new.SetSpoof(true);
				mtag_new.SetIPSource(InetSocketAddress::ConvertFrom(from).GetIpv4 ().Get());
				mtag_new.SetSrcPort(InetSocketAddress::ConvertFrom (from).GetPort ());
				mtag_new.SetIPDest(m_local.Get());
			}
			NS_LOG_LOGIC(" APP DST: " << dst << " vs " << m_tcp_conn[m_pair[socket]]);

			NS_LOG_LOGIC("SENDING the packet to the original destination: " << dst);
			NS_LOG_LOGIC("orig socket " << socket << " dest socket " << m_pair[socket]);
			if (InetSocketAddress::ConvertFrom (from).GetIpv4 () == m_local) {
				// XXX TCP lot's of things to do
				//MalSendMsg(m_pair[socket], m, mtag_new, packet->GetSize());
				delete ms->buffer;
				ms->buffer = NULL;
			} else {
				packet->AddPacketTag(mtag_new);
				m_pair[socket]->Send (packet);
			}
		}
    }
}

void MalProxy::HandleAccept (Ptr<Socket> s, const Address& from)
{
	NS_LOG_FUNCTION (this << s << from);
	NS_LOG_LOGIC("ACCEPT IN ECHO SERVER from " << InetSocketAddress::ConvertFrom(from));
	s->SetRecvCallback (MakeCallback (&MalProxy::HandleTCPRead, this));

	Ipv4Address dst = s->m_orgDestIP;
	if (InetSocketAddress::ConvertFrom (from).GetIpv4 () == m_local) {
		NS_LOG_LOGIC(" FROM MY TAP - set dest: " << dst);
	} else {
		dst = m_local;
		NS_LOG_LOGIC(" FROM  OUTSIDE - set dest: " << dst);
	}
	NS_LOG_LOGIC("ORIGINAL DST: " << dst << " original src: " << s->m_srcNode);
	NS_LOG_LOGIC("FROM: " << InetSocketAddress::ConvertFrom(from).GetIpv4());

	//open other tcp connection...
	Ptr<Socket> new_socket = Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::TcpSocketFactory"));
	if (InetSocketAddress::ConvertFrom (from).GetIpv4 () == m_local) {
		new_socket->Bind(m_local);
	} else {
		NS_LOG_LOGIC("BIND WITH REMOTE ADDRESS\n");
		new_socket->Bind(InetSocketAddress::ConvertFrom (from).GetIpv4 () );
	}
	// let the socket know orginial source information
	new_socket->m_orgSrcIP = InetSocketAddress::ConvertFrom(from).GetIpv4();
	new_socket->m_orgSrcPort = InetSocketAddress::ConvertFrom(from).GetPort();
	NS_LOG_LOGIC(" CONNECT TO " << dst << " : " << m_tcp_port); 
	new_socket->Connect(InetSocketAddress(dst, m_tcp_port));
	new_socket->SetRecvCallback (MakeCallback (&MalProxy::HandleTCPRead, this));
	//should do something if it fails...

	m_tcp_conn[new_socket] = dst;
	m_tcp_conn[s] = InetSocketAddress::ConvertFrom(from).GetIpv4();
	m_pair[s] = new_socket;
	m_pair[new_socket] = s;
	s->m_pair = new_socket;
	new_socket->m_pair = s;
}




} // Namespace ns3


