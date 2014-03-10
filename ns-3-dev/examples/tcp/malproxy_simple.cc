/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 *
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/simple-net-device.h"
#include "ns3/simple-channel.h"
#include "ns3/csma-layout-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include <pthread.h>

// Network topology (default)
//
//     n1--- n0---n2            .
//


using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("MalSimple");

NodeContainer terminals;
NetDeviceContainer terminalDevices;
std::vector<Ptr<SimpleNetDevice> > simpleDevices;
set<int> udp_ports;
set<int> tcp_ports;
std::vector<string> ip_addrs;
std::vector<string> tap_names;
std::vector<string> mac_addresses;
ifstream topology;
ApplicationContainer* apps;
map<int, int> delayMap;
int debug=0;

void 
PopulateArpCache () 
{ 
	Ptr<ArpCache> arp = CreateObject<ArpCache> (); 
	arp->SetAliveTimeout (Seconds(3600 * 24 * 365)); 
	for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i) 
	{ 
		Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> (); 
		if (ip == 0) {
			std::cout << "no ip for " << (*i)->GetId() << std::endl;
			continue;
		}
		//NS_ASSERT(ip !=0); 
		ObjectVectorValue interfaces; 
		ip->GetAttribute("InterfaceList", interfaces); 
		for(ObjectVectorValue::Iterator j = interfaces.Begin(); j != 
				interfaces.End (); j ++) 
		{ 
			Ptr<Ipv4Interface> ipIface = (*j)->GetObject<Ipv4Interface> (); 
			NS_ASSERT(ipIface != 0); 
			Ptr<NetDevice> device = ipIface->GetDevice(); 
			NS_ASSERT(device != 0); 
			Mac48Address addr = Mac48Address::ConvertFrom(device->GetAddress ()); 
			for(uint32_t k = 0; k < ipIface->GetNAddresses (); k ++) 
			{ 
				Ipv4Address ipAddr = ipIface->GetAddress (k).GetLocal(); 
				if(ipAddr == Ipv4Address::GetLoopback()) 
					continue; 
				ArpCache::Entry * entry = arp->Add(ipAddr); 
				entry->MarkWaitReply(0); 
				entry->MarkAlive(addr); 
			} 
		} 
	} 
	for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i) 
	{ 
		Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> (); 
		//NS_ASSERT(ip !=0); 
		if (ip == 0) {
			std::cout << "no ip for " << (*i)->GetId() << std::endl;
			continue;
		}
		ObjectVectorValue interfaces; 
		ip->GetAttribute("InterfaceList", interfaces); 
		for(ObjectVectorValue::Iterator j = interfaces.Begin(); j != 
				interfaces.End (); j ++) 
		{ 
			Ptr<Ipv4Interface> ipIface = (*j)->GetObject<Ipv4Interface> (); 
			ipIface->SetAttribute("ArpCache", PointerValue(arp)); 
		} 
	} 
} 
void readDelay()
{
	topology.open("delay");
	if (!topology.is_open()) {
		std::cout << "can not open delay file" << std::endl;
		return;
	}
	if (!topology.eof()) {
		istringstream lineBuffer;
		string line;
		int idx, delay;

		while (!topology.eof ())
		{
			line.clear ();
			lineBuffer.clear ();

			getline (topology,line);
			lineBuffer.str (line);
			lineBuffer >> idx;
			lineBuffer >> delay;
			delayMap[idx] = delay;
		}
	}
	return;
}

bool readKingTopology(uint32_t* src, uint32_t* dest, uint64_t *delay, uint32_t num_terminal)
{
	if (!topology.is_open()) {
		std::cout << "network topology file not open\n";
		return false;
	}
	if (!topology.eof()) {
		istringstream lineBuffer;
		string line;

		while (!topology.eof ())
		{
			line.clear ();
			lineBuffer.clear ();

			getline (topology,line);
			lineBuffer.str (line);
			lineBuffer >> *src;
			if (*src > num_terminal) continue;
			lineBuffer >> *dest;
			if (*dest > num_terminal) continue;
			lineBuffer >> *delay;
			*src = *src - 1;
			*dest = *dest - 1; // zero base
			return true;
		}
	}
	return false;
}

void setAddressStrings(int num, char *ip_base, char* tap_base) {
	char ip_addr_str[20];
	char tap_name[100];
	char mac_addr[20];
	for (int i=0; i<num; i++) {
		sprintf(ip_addr_str, "%s.%d", ip_base, i+1);
		ip_addrs.push_back(string(ip_addr_str));
		sprintf(tap_name, "%s%d", tap_base, i+1);
		tap_names.push_back(string(tap_name));
		sprintf(mac_addr, "00:00:00:02:00:%02d", i+1);
		mac_addresses.push_back(string(mac_addr));
	}
}

void MalProxyTap(int i, char *ip_base, bool ifMalicious, char* tap_base, int runtime)
{
	const char * ip_addr_str = ip_addrs[i].c_str();
	string mac_addr = mac_addresses[i];
	string tap_name = tap_names[i];

	if (ifMalicious) {
		for (set<int>::iterator pi=udp_ports.begin(); pi!= udp_ports.end(); pi++) {
			MalProxyHelper server (Ipv4Address(ip_addr_str), *pi, 0);
			apps->Add(server.Install (terminals.Get(i)));
			if (i == 0) NS_LOG_INFO(" UDP port : " << *pi);
		}
		for (set<int>::iterator pi=tcp_ports.begin(); pi!= tcp_ports.end(); pi++) {
			MalProxyHelper server (Ipv4Address(ip_addr_str), 0, *pi);
			apps->Add(server.Install (terminals.Get(i)));
			if (i == 0) NS_LOG_INFO(" TCP port : " << *pi);
		}
	}

	TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
	// TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("UseLocal"));
  tapBridge.SetAttribute ("DeviceName", StringValue (tap_name));
	tapBridge.SetAttribute ("MacAddress", StringValue (mac_addr));
  tapBridge.Install (terminals.Get (i), terminalDevices.Get (i));
	if (ifMalicious) terminals.Get(i)->SetMalicious(); // this is how we set a node to be malicious
}


#ifndef CMD_PORT
#define CMD_PORT 8000
#endif

void commandListener(void)
{
	int sockfd, n, newsockfd, tr=1;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	void * savePtr = NULL;
	void * appPtr = NULL;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	NS_ABORT_IF(sockfd < 0);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(CMD_PORT);
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int))) {
    perror("setsockopt");
    exit(1);
  }
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		NS_ABORT_MSG("ERROR on binding");
	while (1) {
		string outbuffer;
		int out_len = 0;
		listen(sockfd,5);
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			NS_ABORT_MSG("ERROR on accept");
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);

		if (n < 0) NS_ABORT_MSG("ERROR reading from socket");

		if (buffer[0] == 'C') {
			int i = apps->Command(string(buffer+2));
			if (i) outbuffer = "command received\n";
			else outbuffer = "command fail\n";
		} else if (buffer[0] == 'F') {
			if(debug>1){std::cout << "NS-3: Pause" << std::endl;}
			out_len = Simulator::ToggleFreeze(true);
			if(debug>1){std::cout << "NS-3: Paused" << std::endl;}
		} else if (buffer[0] == 'S') {
			//save event queue
			if(debug>1){std::cout << "NS-3: Save" << std::endl;}
			savePtr = Simulator::Save(NULL);
			appPtr = apps->Save();
			if (savePtr != NULL){
				if(debug>1){std::cout << "NS-3: Saved" << std::endl;}
			}
			else{
				std::cout << "NS-3: Save Failed" << std::endl;
			}
		} else if (buffer[0] == 'L') {
			//save event queue
			if(debug>1){std::cout << "NS-3: Load" << std::endl;}
			out_len = Simulator::Load(savePtr);
			apps->Load(appPtr);
			if(debug>1){std::cout << "NS-3: Loaded" << std::endl;}
		} else if (buffer[0] == 'R') {
			if(debug>1){std::cout << "NS-3: Resume" << std::endl;}
			apps->Resume();
			out_len = Simulator::ToggleFreeze(false);
			if(debug>1){std::cout << "NS-3: Resumed" << std::endl;}
		} 
		if (out_len) {
			n = write(newsockfd, outbuffer.c_str(), outbuffer.length());
			if (n < 0) NS_ABORT_MSG("ERROR writing to socket");
		}
		close(newsockfd);
	}
	close(sockfd);
	return;
}

	int 
main (int argc, char *argv[])
{
	//
	// Set up some default values for the simulation.
	//
	GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
	LogComponentEnable("MalProxyApplication", LOG_INFO);
	//LogComponentEnable("MalProxyApplication", LOG_LOGIC);
	//LogComponentEnable("MalSimple", LOG_INFO);
	//LogComponentEnable("SimpleNetDevice", LOG_INFO);
	//LogComponentEnable("SimpleNetDevice", LOG_FUNCTION);
	//LogComponentEnable("Icmpv4L4Protocol", LOG_INFO);
	//LogComponentEnable("TapBridge", LOG_INFO);
	//LogComponentEnable("TapBridge", LOG_FUNCTION);
	//LogComponentEnable("TapBridge", LOG_LOGIC);
	LogComponentEnableAll(LOG_PREFIX_NODE);
	LogComponentEnableAll(LOG_PREFIX_FUNC);
	//LogComponentEnable("Ipv4Interface", LOG_FUNCTION);
	//LogComponentEnable("Ipv4Interface", LOG_INFO);

	//LogComponentEnable("Node", LOG_INFO);

	//LogComponentEnable("Ipv4L3Protocol", LOG_FUNCTION);
	//LogComponentEnable("Ipv4L3Protocol", LOG_INFO);

	//LogComponentEnable("Ipv4EndPointDemux", LOG_FUNCTION);
	//LogComponentEnable("Ipv4EndPointDemux", LOG_DEBUG);
	//LogComponentEnable("Ipv4EndPointDemux", LOG_LOGIC);
//
	//LogComponentEnable("Ipv4EndPoint", LOG_FUNCTION);
	//LogComponentEnable("Ipv4EndPoint", LOG_DEBUG);
	//LogComponentEnable("Ipv4EndPoint", LOG_LOGIC);

	//LogComponentEnable("UdpL4Protocol", LOG_INFO);
	//LogComponentEnable("UdpL4Protocol", LOG_FUNCTION);
	//LogComponentEnable("UdpL4Protocol", LOG_LOGIC);

  CommandLine cmd;
  //cmd.Parse (argc, argv);
	int num_terminal =4;
  //uint16_t port = 300;
  uint16_t tcp_port = 22;
  uint16_t udp_port = 5377;
	set<int> malNodes;
	char *ip_base = "10.1.1";
	char ip_base_str[20];
	char *tap_base = "tap";
	int runtime = 600;
	std::string network = "default";
	bool globalRouting = true;

	// command processing

	SystemThread commandProcessor = SystemThread(MakeCallback (&commandListener));
	commandProcessor.Start();

	for (int i=0; i<argc; i++) {
		if (strcmp(argv[i], "-tcp_port") == 0) {
			i++;
			tcp_port = atoi(argv[i]);
			tcp_ports.insert(tcp_port);
		}
		else if (strcmp(argv[i], "-udp_port") == 0) {
			i++;
			udp_port = atoi(argv[i]);
			udp_ports.insert(udp_port);
		}
		else if (strcmp(argv[i], "-mal") == 0) {
			i++;
			malNodes.insert(atoi(argv[i]));
		}
		else if (strcmp(argv[i], "-num_vms") == 0) {
			i++;
			num_terminal = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-runtime") == 0) {
			i++;
			runtime = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-ip_base") == 0) {
			i++;
			ip_base = strdup(argv[i]);
		}
		else if (strcmp(argv[i], "-tap_base") == 0) {
			i++;
			tap_base = strdup(argv[i]);
		}
		else if (strcmp(argv[i], "-network") == 0) {
			i++;
			network = string(argv[i]);
		}
	}
	sprintf(ip_base_str, "%s.0", ip_base);
  NS_LOG_INFO ("Build MalProxy topology.");

  terminals.Create (num_terminal);
  InternetStackHelper internet;
	setAddressStrings(num_terminal, ip_base, tap_base); // prepare address strings
  internet.Install (terminals);


	{
		for (int i = 0; i < num_terminal; i++) {
			Ptr<SimpleNetDevice> dev = CreateObject<SimpleNetDevice>(); 
			terminals.Get(i)->AddDevice(dev);
			dev->SetAddress(Mac48Address(mac_addresses[i].c_str()));
			terminalDevices.Add (dev);
			simpleDevices.push_back(dev);
		}
		Ipv4AddressHelper ipv4;
		ipv4.SetBase (ip_base_str, "255.255.255.0");
		ipv4.Assign (terminalDevices);
		PopulateArpCache();
		for (int i = 0; i < num_terminal; i++) {
			// let the device remember the pair of channel and IP
			for (int j = i+1; j < num_terminal; j++) {
				Ptr<SimpleNetDevice> devI = simpleDevices[i];
				Ptr<SimpleNetDevice> devJ = simpleDevices[j];
				Ptr<SimpleChannel> channel = CreateObject<SimpleChannel>();
				channel->SetDelay(1000);
				devI->AddChannel(channel, Mac48Address::ConvertFrom(devJ->GetAddress()));
				devJ->AddChannel(channel, Mac48Address::ConvertFrom(devI->GetAddress()));
			}
		}

	} 


	NS_LOG_INFO("====================================");
	for (set<int>::iterator it=malNodes.begin(); it != malNodes.end(); it++) {
	NS_LOG_INFO(" mal : " << *it);
	}
	NS_LOG_INFO(" number of vms : " << num_terminal);
	NS_LOG_INFO(" IP base : " << ip_base_str);
	NS_LOG_INFO(" Tap base : " << tap_base);
	NS_LOG_INFO(" Network : " << network);
	ApplicationContainer appCon = ApplicationContainer();
	apps = &appCon;
	for (int i=0; i<num_terminal; i++) {
		bool ifMalicious = false;
		if (malNodes.find(i) != malNodes.end()) ifMalicious = true;
		MalProxyTap(i, ip_base, ifMalicious, tap_base, runtime);
	}
	apps->Start (Seconds (0.0));
	apps->Stop (Seconds (runtime));
	NS_LOG_INFO("APPS: " << apps->GetN());

	NS_LOG_INFO("====================================");
	

  if (globalRouting) Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	/* Logging
  NS_LOG_INFO ("Enable pcap tracing.");
  csma.EnablePcapAll ("twonodes");
	AsciiTraceHelper ascii;
	csma.EnableAsciiAll(ascii.CreateFileStream("trace"));
	*/

  Simulator::Stop (Seconds (runtime));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
