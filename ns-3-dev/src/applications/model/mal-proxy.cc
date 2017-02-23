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
#include "ns3/dotParser.h"
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace std;

static bool global_consult_gatling = true;
static short global_once = 0;
static bool global_record_message = true;
static int app_debug = 2;

#define CTRL_IP "127.0.0.1"
#define CTRL_PORT 8888
//#define TCP_CONTROLLER 1
#define UDP_CONTROLLER 1

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("MalProxyApplication");
NS_OBJECT_ENSURE_REGISTERED(MalProxy);

void MalProxy::ClearStrategyForMsg(int type)
{
	for(int j=0; j < MAX_STATES; j++){
		for(int d=0; d < DIRECTIONS; d++){
			deliveryActions[j][d][type][NONE] = false;
			deliveryActions[j][d][type][DROP] = false;
			deliveryActions[j][d][type][DUP] = false;
			deliveryActions[j][d][type][DELAY] = false;
			deliveryActions[j][d][type][DIVERT] = false;
			deliveryActions[j][d][type][REPLAY] = false;
			deliveryActions[j][d][type][LIE] = false;
			deliveryActions[j][d][type][BURST] = false;
			deliveryActions[j][d][type][INJECT] = false;
			deliveryActions[j][d][type][WINDOW] = false;
			deliveryActions[j][d][type][RETRY] = false;
			for (int k = 0; k < FIELD; k++) {
				if (lyingValues[j][d][type][k] != NULL) {
					delete lyingValues[0][d][type][k];
					lyingValues[j][d][type][k] = NULL;
				}
			}

			burst_sched[d][type]=false;
			burst[d][type].clear();
		}
		if(injectStates[j]!=NULL){
			delete injectStates[j];
			injectStates[j]=NULL;
		}
		if(windowStates[j]!=NULL){
			delete windowStates[j];
			windowStates[j]=NULL;
		}
		injectMsg[j]=-1;
		windowMsg[j]=-1;
	}

}

void MalProxy::ClearStrategy()
{
	for(int j=0; j < MAX_STATES; j++){
		for(int d=0; d < DIRECTIONS; d++){
			for (int i = 0; i < MSG; i++) {
				deliveryActions[j][d][i][NONE] = false;
				deliveryActions[j][d][i][DROP] = false;
				deliveryActions[j][d][i][DUP] = false;
				deliveryActions[j][d][i][DELAY] = false;
				deliveryActions[j][d][i][DIVERT] = false;
				deliveryActions[j][d][i][REPLAY] = false;
				deliveryActions[j][d][i][LIE] = false;
				deliveryActions[j][d][i][BURST] = false;
				deliveryActions[j][d][i][INJECT] = false;
				deliveryActions[j][d][i][WINDOW] = false;
				deliveryActions[j][d][i][RETRY] = false;
				for (int k = 0; k < FIELD; k++) {
					if (lyingValues[j][d][i][k] != NULL) {
						delete lyingValues[j][d][i][k];
						lyingValues[j][d][i][k] = NULL;
					}
				}
				burst_sched[d][i]=false;
				burst[d][i].clear();
			}
		}
		if(injectStates[j]!=NULL){
			delete injectStates[j];
			injectStates[j]=NULL;
		}
		if(windowStates[j]!=NULL){
			delete windowStates[j];
			windowStates[j]=NULL;
		}
		injectMsg[j]=-1;
		windowMsg[j]=-1;
	}
}

void MalProxy::ParseStrategy(string line)
{
	ClearStrategy();
	AddStrategy(line);
}

void MalProxy::AddStrategy(string line)
{
	string actstr;
	string tmp;
	int cur;

	int state=0;
	int dir=0;
	int msgType;
	int action;
	string avalue;
	int field;
	string fmval;

	NS_LOG_DEBUG("Adding new strategy " << line);
	if (app_debug > 0) {
		std::cout << "MALProxy] " << "Adding new strategy " << line
				<< std::endl;
	}
	while (line.length() > 0) {
		/*Split out this action*/
		cur=line.find(",");
		if(cur==-1){
			actstr=line;
			line="";
		}else{
			actstr=line.substr(0,cur);
			line=line.substr(cur+1);
		}


		/*Parse State Prefix*/
		cur=actstr.find("?");
		tmp=actstr.substr(0,cur);
		boost::algorithm::trim(tmp);
		if(tmp=="*"){
			state=-1;
		}else{
			state=sm_server.GetStateAsInt(State(tmp));
		}
		actstr = actstr.substr(cur + 1);

		/*Parse Direction Prefix*/
		cur=actstr.find("?");
		tmp=actstr.substr(0,cur);
		boost::algorithm::trim(tmp);

		if(tmp=="*"){
			dir=-1;
		}else{
			dir=atoi(tmp.c_str());
		}
		actstr = actstr.substr(cur + 1);

		/*Parse Message Type*/
		cur = actstr.find(" ");
		msgType = Message::StrToType(actstr.substr(0, cur).c_str());
		actstr = actstr.substr(cur + 1);

		/*Parse malicious action*/
		cur = actstr.find(" ");
		string malact = actstr.substr(0, cur);
		actstr = actstr.substr(cur + 1);
		if (malact == "NONE") {
			action=NONE;
		} else if (malact == "DROP") {
			action=DROP;
		} else if (malact == "DUP") {
			action=DUP;
		} else if (malact == "DELAY") {
			action=DELAY;
		} else if (malact == "DIVERT") {
			action=DIVERT;
		} else if (malact == "REPLAY") {
			action=REPLAY;
		} else if (malact == "BURST") {
			action=BURST;
		} else if (malact=="INJECT") {
			action=INJECT;
		} else if (malact=="WINDOW") {
			action=WINDOW;
		} else if (malact == "LIE") {
			action=LIE;
		} else {
			std::cout<< "Error! Unrecognized action: " << malact<<std::endl;
		}

		/*Parse action value*/
		avalue="";
		if(action==DROP){
			avalue=actstr;
		}else if(action==INJECT){
			avalue=actstr;
		}else if(action==WINDOW){
			avalue=actstr;
		}else if(action==LIE){
			cur = actstr.find(" ");
			if (cur != -1) {
				fmval = actstr.substr(0, cur);
				actstr = actstr.substr(cur+1);
			} else {
				fmval = actstr;
				actstr = "";
			}

			cur = actstr.find(" ");
			if (cur != -1) {
				field = atoi(actstr.substr(0, cur).c_str());
				actstr = actstr.substr(cur);
			} else {
				field = atoi(actstr.c_str());
				actstr = "";
			}
		}else{
			cur = actstr.find(" ");
			if (cur != -1) {
				avalue = actstr.substr(0, cur);
				actstr = actstr.substr(cur);
			} else {
				avalue = actstr;
				actstr = "";
			}
		}

		NS_LOG_DEBUG("msgType " << msgType << " action " << malact << " value " << avalue);
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "state " << state << "(" << sm_server.GetState(state) << ") dir " << dir << " msgType " << msgType << " action "
					<< malact << " value " << avalue <<  " fmval " << fmval << " field " << field << std::endl;
		}

		/*Set tables*/
		if(action==INJECT){
			if(state >=0){
				injectMsg[state]=msgType;
				injectStates[state]=strdup(avalue.c_str());
			}else{
				InjectPacket(msgType, avalue.c_str());
			}
		}else if(action==WINDOW){
			if(state >=0){
				windowMsg[state]=msgType;
				windowStates[state]=strdup(avalue.c_str());
			}else{
				Window(msgType, avalue.c_str());
			}
		}else{
			if(state>=0){
				if(dir>=0){
					deliveryActions[state][dir][msgType][action]=true;
					if(action==LIE){
						lyingValues[state][dir][msgType][field]=strdup(fmval.c_str());
					}else{
						deliveryValues[state][dir][msgType][action]=atof(avalue.c_str());
					}
				}else{
					for(int d=0; d < DIRECTIONS; d++){
						deliveryActions[state][d][msgType][action]=true;
						if(action==LIE){
							lyingValues[state][d][msgType][field]=strdup(fmval.c_str());
						}else{
							deliveryValues[state][d][msgType][action]=atof(avalue.c_str());
						}
					}
				}
			}else{
				if(dir >=0){
					for(int i=0; i < MAX_STATES; i++){
						deliveryActions[i][dir][msgType][action]=true;
						if(action==LIE){
							lyingValues[i][dir][msgType][field]=strdup(fmval.c_str());
						}else{
							deliveryValues[i][dir][msgType][action]=atof(avalue.c_str());
						}
					}
				}else{
					for(int i=0; i < MAX_STATES; i++){
						for(int d=0; d < DIRECTIONS; d++){
							deliveryActions[i][d][msgType][action]=true;
							if(action==LIE){
								lyingValues[i][d][msgType][field]=strdup(fmval.c_str());
							}else{
								deliveryValues[i][d][msgType][action]=atof(avalue.c_str());
							}
						}
					}
				}
			}
		}
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
					MakeIpv4AddressChecker()).AddAttribute("Port",
					"Port on which we listen for incoming packets.",
					UintegerValue(9),
					MakeUintegerAccessor(&MalProxy::m_port),
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
}

void MalProxy::DoDispose(void)
{
	NS_LOG_FUNCTION_NOARGS ();
	Application::DoDispose();
}

void MalProxy::AddMessage(int src, int dest, int dir, size_t size, uint8_t *m) {
    class message_data data = message_data(src, dest, dir, size, m);
    m_messageQueue.push(data);
}

void MalProxy::DumpMessages(ostream &os) {
    unsigned long seq=0;
    os << "seq,dir,size,data\n";
	while (!m_messageQueue.empty()) {
        class message_data data = m_messageQueue.front();
        os << seq++ << "," << data.src << "," << data.dest << "," << data.dir << "," << data.size << ",";
        os.write((const char *)data.data, data.size);
        os << "\n";
        free(data.data);
        m_messageQueue.pop();
    }
}

int MalProxy::Command(string command, std::string &output)
{
	if (command.compare("Gatling Pause\n") == 0) {
		global_consult_gatling = false;
		return 1;
	}
	if (command.compare("Gatling Resume\n") == 0) {
		global_consult_gatling = true;
		return 1;
	}
	if(command.compare(0, strlen("GatlingLoadStateDiagram"), "GatlingLoadStateDiagram")==0){
		string path=command.substr(strlen("GatlingLoadStateDiagram"));
		boost::algorithm::trim(path);
		DotParser dp;
		dp.parseGraph(path.c_str());
		dp.BuildStateMachine(&sm_server);
		dp.BuildStateMachine(&sm_client);
		sm_server.Start(State("LISTEN"), Simulator::Now().GetMilliSeconds());
		sm_client.Start(State("CLOSED"), Simulator::Now().GetMilliSeconds());
		return 1;
	}
	if(command.compare(0, strlen("GatlingSendStateStats"), "GatlingSendStateStats")==0){
		/* TODO: Return to controller! */
		stringstream ss;
		ss.str("");
		sm_server.Finish(Simulator::Now().GetMilliSeconds());
		sm_server.GetStateMetricTracker()->PrintAll(ss);
		ss<<"======="<<std::endl;
		sm_client.Finish(Simulator::Now().GetMilliSeconds());
		sm_client.GetStateMetricTracker()->PrintAll(ss);
		output=ss.str();
		return 1;
	}
	if(command.compare("GatlingClearStateStats\n")==0){
		sm_server.GetStateMetricTracker()->Reset();
		sm_client.GetStateMetricTracker()->Reset();
		return 1;
	}
	if (command.compare(0, strlen("GatlingStartMessageRecording"), "GatlingStartMessageRecording") == 0) {
                std::cout << "Start message recording" << std::endl;
		global_record_message = true;
		return 1;
	}
	if (command.compare(0, strlen("GatlingStopMessageRecording"), "GatlingStopMessageRecording") == 0) {
		global_record_message = false;
		return 1;
	}
	if (command.compare(0, strlen("GatlingDumpRecordedMessages"), "GatlingDumpRecordedMessages") == 0) {
            // filename is the next argument
            ofstream dumpfilestr;
            char *filename = "dump_recording";
            if (command.length() > strlen("GatlingDumpRecordedMessages") + 1) {
                filename = (char *)command.c_str() + strlen("GatlingDumpRecordedMessages") + 1;
                if (strstr(filename, "\n") != NULL) filename[strstr(filename, "\n")-filename] = 0;
            }
            dumpfilestr.open(filename, ofstream::out | ofstream::trunc);
            std::cout << "dump messages to file: [" << filename << "]" << std::endl;
            //stringstream ss;
            //ss.str("");
            DumpMessages(dumpfilestr);
            //output=ss.str();
            dumpfilestr.close();
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
	NS_LOG_INFO("MalProxy local address:  " << m_local << " port: " << m_port);
	srand((unsigned) time(0));
}

void MalProxy::StopApplication()
{
	NS_LOG_FUNCTION_NOARGS ();
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

int MalProxy::MalUDPMsg(Message *m, int dir,maloptions *res)
{
	if(MalMsg(m,dir)==true){
		return MaliciousStrategy(m,dir,res);
	}
	return NONE;
}

int MalProxy::MalMsg(Message *m, int dir)
{
	int state=sm_server.GetStateAsInt();

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
		return MalMsg(m->encMsg, dir);
	}

	if (m->type < 0 || m->type >= MSG) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "invalid type " << m->type << ":"
					<< MSG << std::endl;
		}
		return 0;
	}

	if (deliveryActions[state][dir][m->type][DROP]) {
		return 1;
	}

	for (int i = 0; i < FIELD; i++) {
		if (lyingValues[state][dir][m->type][i] != NULL) {
			if (app_debug > 0) {
				std::cout << "MALProxy] " << "will lie for " << m->type
						<< std::endl;
			}
			return 1;
		}
	}

	if (deliveryActions[state][dir][m->type][DUP]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will dup for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[state][dir][m->type][DELAY]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will delay for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[state][dir][m->type][DIVERT]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will divert for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[state][dir][m->type][REPLAY]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will replay for " << m->type
					<< std::endl;
		}
		return 1;
	}

	if (deliveryActions[state][dir][m->type][BURST]) {
		if (app_debug > 0) {
			std::cout << "MALProxy] " << "will burst for " << m->type
					<< std::endl;
		}
		return 1;
	}

	return 0;
}

int MalProxy::MaliciousStrategy(Message *m, int dir,maloptions *res)
{
	bool lie = false;
	int state=sm_server.GetStateAsInt();

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

			if (deliveryActions[state][dir][cur->type][DROP]) {
				double prob = 100 * (double) rand() / (double) RAND_MAX;
				if (prob <= deliveryValues[state][dir][cur->type][DROP]) {
					NS_LOG_INFO("MalProxy dropping message");
					res->action = DROP;
					return DROP;
				}
			}

			for (int i = 0; i < FIELD; i++) {
				if (lyingValues[state][dir][cur->type][i] != NULL) {
					lie = true;
				}
			}

			if (deliveryActions[state][dir][cur->type][DUP]) {
				NS_LOG_INFO("MalProxy duplicating message " << (int)deliveryValues[state][dir][cur->type][DUP] << " times");
				if (res->duptimes < (int) deliveryValues[state][dir][cur->type][DUP]) {
					res->duptimes = (int) deliveryValues[state][dir][cur->type][DUP];
				}
			}

			if (deliveryActions[state][dir][cur->type][DIVERT]) {
				NS_LOG_INFO("MalProxy diverting message" << cur->type);
				res->divert = true;
			}

			if (deliveryActions[state][dir][cur->type][DELAY]) {

				if (res->delay < deliveryValues[state][dir][cur->type][DELAY]) {
					res->delay = deliveryValues[state][dir][cur->type][DELAY];
				}
				if (deliveryValues[state][dir][cur->type][DELAY] < 0) {
					res->delay = -1 * deliveryValues[state][dir][cur->type][DELAY]
							* (double) rand() / (double) RAND_MAX;
				} NS_LOG_INFO("MalProxy delaying message " << res->delay << " seconds");
			}

			if (deliveryActions[state][dir][cur->type][REPLAY]) {
				// By default, replay packet will go to the original sender. It can be used with DIVERT/DUP.
				// If BROADCAST is good, we can work on that as well.
				res->replay = (int) deliveryValues[state][dir][cur->type][REPLAY];
			}

			if (deliveryActions[state][dir][cur->type][BURST]) {
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
					if (lyingValues[state][dir][cur->type][i] != NULL) {
						cur->ChangeValue(i, lyingValues[state][dir][cur->type][i]);
					}
				}
			}
			cur = cur->encMsg;
		}

	}
	res->action = DELAY;
	return DELAY;
}

int MalProxy::MalTransportProtocol(Ptr<Packet> packet, lowerLayers ll, maloptions *res)
{
	connection *c;
	std::vector<seq_state> *seq_list;
	std::vector<seq_state> *ack_list;
	int seq_offset = 0;
	int ack_offset = 0;
	Message *m;
	char tmp[32];

	res->action = NONE;

	m = new Message(packet->PeekDataForMal());
	if (packet->GetSize() < m->size) {
		return NONE;
	}

	/*Check ports*/
#if (defined SOURCE_PORT_FIELD) && (defined DEST_PORT_FIELD)
	if ((ll.dir == FROMTAP && m->GetDestPort() != this->m_port)
			|| (ll.dir == TOTAP && m->GetSourcePort() != this->m_port)) {
		/*Packet we don't care about*/
		res->action = NONE;
		return NONE;
	}
#endif

	/*Debug output to display packet*/
	//std::cout << ntohs(((BaseMessage*) m->msg)->SOURCE_PORT_FIELD) << "-->"<< ntohs(((BaseMessage*) m->msg)->DEST_PORT_FIELD) << std::endl;
	//std::cout << "Type: " << m->FindMsgType() << " (" << m->TypeToStr(m->FindMsgType()) << ")" << std::endl;
	//std::cout << "Size: " << m->FindMsgSize() << std::endl;

	/*Adjust Sequence Numbers*/
#if (defined SEQUENCE_FIELD) && (defined ACKNOWLEDGEMENT_FIELD)
	if (ll.dir == FROMTAP) {
		/*Forward direction, from "malicious" host*/
		c = FindConnection(ll.iph.GetSource(), ll.iph.GetDestination(),
				m->GetSourcePort(), m->GetDestPort());
		seq_list = &(c->d1);
		ack_list = &(c->d2);
	} else {
		/*Reverse direction, from a victim*/
		c = FindConnection(ll.iph.GetDestination(), ll.iph.GetSource(),
				m->GetDestPort(), m->GetDestPort());
		seq_list = &(c->d2);
		ack_list = &(c->d1);

	}
	seq_offset = FindOffset(seq_list, SequenceNumber32(m->GetSequenceNumber()));
	ack_offset = FindOffset(ack_list, SequenceNumber32(m->GetAcknowledgementNumber()));

	m->SetSequenceNumber(m->GetSequenceNumber()+seq_offset);
	m->SetAcknowledgementNumber(m->GetAcknowledgementNumber()+ack_offset);
#endif

        if (global_record_message) {
            static uint64_t seq = 0;
            AddMessage(ll.iph.GetSource().Get(), ll.iph.GetDestination().Get(), ll.dir, m->FindMsgSize(), m->msg);
        }
	/*Check for Malicious Actions and Do them!*/
	int result = MalMsg(m, ll.dir);
	if (result == 0) {
		res->action = NONE;
		RunStateMachines(m,&ll, res);
		ShouldInject();
		return NONE;
	} else if (result == 2) {
		res->action = RETRY;
		return RETRY;
	}
	MaliciousStrategy(m, ll.dir, res);
	RunStateMachines(m,&ll, res);
	ShouldInject();

	/*Handle Burst action*/
	if (res->burst){
		burst[ll.dir][m->type].push_back(std::make_pair(packet,ll));
		if (!burst_sched[ll.dir][m->type]) {
			Simulator::Schedule(Time(Seconds(deliveryValues[sm_server.GetStateAsInt()][ll.dir][m->type][BURST])),
					&MalProxy::Burst, this, m->type, ll.dir);
			burst_sched[ll.dir][m->type] = true;
		}
		res->action = DROP;
	}

	/*Checksum will be recomputed as actions are applied... particularly for Divert*/
	delete (m);
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

//t=time databytes ip_src ip_dest 0=port_src 1=port_dest 2=seq 3=ack 4=reserved 5=size 6=type 7=urg 8=ece 9=cwr 10=window 12=urgptr
void MalProxy::InjectPacket(int type, const char *spec)
{
	int databytes;
	Ptr<Packet> p;
	TcpHeader tcph;
	Message *m;
	double sec;
	int len = 0;
	char ssrc[100];
	char sdest[100];
	Ipv4Address src;
	Ipv4Address dest;
	int size;

	sscanf(spec, "t=%lf %i %99s %99s%n", &sec, &databytes, ssrc, sdest, &len);
	src = Ipv4Address(ssrc);
	dest = Ipv4Address(sdest);
	spec += len;
	size = Message::GetMessageHeaderSize(type);
	uint8_t *buf=(uint8_t*)malloc(size);
	if(!buf){
		return;
	}
	memset(buf,0,size);
	p = new Packet(buf,size);
	free(buf);
	m = new Message(p->PeekDataForMal());
	m->CreateMessage(type, spec);
	m->DoChecksum(size + databytes,src,dest,IPprotoNum());


	Simulator::Schedule(Time(Seconds(sec)),
						&MalProxy::DoInjectPacket, this, p, src,dest);
	return;
}

void MalProxy::DoInjectPacket(Ptr<Packet> p, Ipv4Address src, Ipv4Address dest)
{
	Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();
	if (ipv4 != 0) {

		/*Handle state machine*/
		Message *m;
		m=new Message(p->PeekDataForMal());
		lowerLayers ll;
		ll.dir=FROMTAP;
		maloptions res;
		res.action=NONE;
		res.burst=false;
		res.delay=0;
		res.divert=0;
		res.duptimes=1;
		res.replay=0;
		RunStateMachines(m, &ll, &res);
		delete(m);

		Ipv4Header header;
		header.SetDestination(dest);
		header.SetProtocol(IPprotoNum());
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
		ipv4->Send(p, src, dest, IPprotoNum(), route);
	}
}

void MalProxy::Burst(int type, int dir)
{
	if (burst_sched[dir][type] == false) {
		return;
	}

	if(app_debug >0){
		std::cout<<"Bursting for type "<< type << std::endl;
	}
	for (int i = 0; i < burst[dir][type].size(); i++) {
		burst[dir][type][i].second.iph.EnableChecksum();
		burst[dir][type][i].first->AddHeader(burst[dir][type][i].second.iph);
		((TapBridge*)(burst[dir][type][i].second.obj))->SendPacket(burst[dir][type][i].first,burst[dir][type][i].second);
	}
	burst[dir][type].clear();
	burst_sched[dir][type] = false;
}

//w=window t=time ip_src ip_dest port_src port_dest size
void MalProxy::Window(int type, const char* spec)
{
	int window;
	double sec;
	int len;
	unsigned int seq;
	unsigned int itter;
	char ip_src[100];
	char ip_dest[100];
	char p_src[100];
	char p_dest[100];
	char pspec[1000];
	double inc=0.0001;
	int strlen;

	sscanf(spec, "w=%i t=%lf %99s %99s%n", &window, &sec, ip_src, ip_dest, &strlen);
	spec +=strlen;

	itter = (0xFFFFFFFF) / window;

	seq = 0;
	for (int i = 0; i < itter; i++) {
#ifdef SEQUENCE_FIELD_NUM
#ifdef WINDOW_FIELD_NUM
		snprintf(pspec, 1000, "t=%f 0 %s %s %i=%i %i=%i %s", sec, ip_src,
				ip_dest, SEQUENCE_FIELD_NUM, seq, WINDOW_FIELD_NUM, window, spec);
#else
		snprintf(pspec, 1000, "t=%f 0 %s %s %s", sec, ip_src, ip_dest, spec);
#endif
#else
		snprintf(pspec, 1000, "t=%f 0 %s %s %s", sec, ip_src,ip_dest, spec);
#endif
		InjectPacket(type,pspec);
		seq += window;
		sec+=inc;
	}
}

void MalProxy::RunStateMachines(Message *m, lowerLayers *ll, maloptions *res)
{
	if(res->action==BURST){
		/*Handled elsewhere*/
		return;
	}

	string packetTypeMetric = "pkt_cnt_" + m->TypeToStr(m->type);

	for(int i=0 ; i < res->duptimes;i++){
       if(ll->dir==FROMTAP){
    	   	  if(res->replay==1){
    	   		sm_client.IncrementMetric("sent_pkt_cnt");
    	   		sm_client.MakeTransition(-1,m->type,Simulator::Now().GetMilliSeconds()+res->delay*1000);
    	   		if(res->action!=DROP){
    	   			sm_client.IncrementMetric("rcvd_pkt_cnt");
    	   			sm_client.IncrementMetric(packetTypeMetric);
    	   			sm_client.MakeTransition(m->type,-1,Simulator::Now().GetMilliSeconds()+res->delay*1000);
    	   		}
    	   	  }else if(res->divert==1){
    	   		sm_client.IncrementMetric("sent_pkt_cnt");
    	   		sm_client.MakeTransition(-1,m->type,Simulator::Now().GetMilliSeconds()+res->delay*1000);
    	   	  }else{
    	   	   sm_client.IncrementMetric("sent_pkt_cnt");
               sm_client.MakeTransition(-1, m->type,Simulator::Now().GetMilliSeconds()+res->delay*1000);
               if(res->action!=DROP){
            	   sm_server.IncrementMetric("rcvd_pkt_cnt");
            	   sm_server.IncrementMetric(packetTypeMetric);
            	   sm_server.MakeTransition(m->type,-1,Simulator::Now().GetMilliSeconds()+res->delay*1000);
               }
    	   	  }
       }else{
    	   if(res->replay==1){
    		   sm_server.IncrementMetric("sent_pkt_cnt");
    		   sm_server.MakeTransition(-1,m->type,Simulator::Now().GetMilliSeconds()+res->delay*1000);
    		   if(res->action!=DROP){
    			   sm_server.IncrementMetric("rcvd_pkt_cnt");
    			   sm_server.IncrementMetric(packetTypeMetric);
    			   sm_server.MakeTransition(m->type,-1,Simulator::Now().GetMilliSeconds()+res->delay*1000);
    		   }
    	   }else if(res->divert==1){
    		   sm_server.IncrementMetric("sent_pkt_cnt");
    		   sm_server.MakeTransition(-1,m->type,Simulator::Now().GetMilliSeconds()+res->delay*1000);
    	   }else{
    		   sm_server.IncrementMetric("sent_pkt_cnt");
               sm_server.MakeTransition(-1,m->type,Simulator::Now().GetMilliSeconds()+res->delay*1000);
               if(res->action!=DROP){
            	   sm_client.IncrementMetric("rcvd_pkt_cnt");
            	   sm_client.IncrementMetric(packetTypeMetric);
            	   sm_client.MakeTransition(m->type,-1,Simulator::Now().GetMilliSeconds()+res->delay*1000);
               }
    	   }
       }
	}
}

void MalProxy::ShouldInject(){
	if(injectStates[sm_server.GetStateAsInt()]!=NULL){
		if(app_debug > 0){
			std::cout<<"Injecting packet type " << injectMsg[sm_server.GetStateAsInt()] << " in state " << sm_server.GetCurrentState() << std::endl;
		}
		InjectPacket(injectMsg[sm_server.GetStateAsInt()],injectStates[sm_server.GetStateAsInt()]);
		delete injectStates[sm_server.GetStateAsInt()];
		injectStates[sm_server.GetStateAsInt()]=NULL;
	}
	if(windowStates[sm_server.GetStateAsInt()]!=NULL){
		if(app_debug > 0){
			std::cout<<"Windowing packet type " << windowMsg[sm_server.GetStateAsInt()] << " in state " << sm_server.GetCurrentState() << std::endl;
		}
		Window(windowMsg[sm_server.GetStateAsInt()],windowStates[sm_server.GetStateAsInt()]);
		delete windowStates[sm_server.GetStateAsInt()];
		windowStates[sm_server.GetStateAsInt()]=NULL;
	}
	return;
}


bool MalProxy::ShouldDoUDP(){
#ifdef IS_UDP
	return true;
#else
	return false;
#endif
}
bool MalProxy::ShouldDoTransport(){
#ifdef IS_TRANSPORT
	return true;
#else
	return false;
#endif
}
int  MalProxy::IPprotoNum(){
#ifdef IP_PROTO_NUM
	return IP_PROTO_NUM;
#else
	return 0;
#endif
}



} // Namespace ns3

