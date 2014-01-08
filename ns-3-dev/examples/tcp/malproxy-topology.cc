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

NS_LOG_COMPONENT_DEFINE ("MalProxyTopology");

NodeContainer terminals;
NetDeviceContainer terminalDevices;
set<int> udp_ports;
set<int> tcp_ports;
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
void MalProxyTap(int i, char *ip_base, bool ifMalicious, char* tap_base, int runtime)
{
	char ip_addr_str[20];
	char tap_name[100];
	char mac_addr[20];

	sprintf(ip_addr_str, "%s.%d", ip_base, i+1);
	sprintf(tap_name, "%s%d", tap_base, i+1);
	sprintf(mac_addr, "%02x:ff:ff:ff:ff:ff", i+1);
	
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

	std::cout << "ip: " << ip_addr_str << " tap: " << tap_name << std::endl;
	TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("UseLocal"));
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
	int sockfd, n, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	void * savePtr = NULL;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	NS_ABORT_IF(sockfd < 0);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(CMD_PORT);
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
		printf("Here is the message: %s\n",buffer);

		if (buffer[0] == 'C') {
			int i = apps->Command(string(buffer+2));
			if (i) outbuffer = "command received\n";
			else outbuffer = "command fail\n";
		} else if (buffer[0] == 'F') {
			if(debug>1){std::cout << "NS-3: Pause" << std::endl;}
			out_len = Simulator::ToggleFreeze(true);
			if(debug>1){std::cout << "NS-3: Pause" << std::endl;}
		} else if (buffer[0] == 'S') {
			//save event queue
			if(debug>1){std::cout << "NS-3: Save" << std::endl;}
			savePtr = Simulator::Save(NULL);
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
			if(debug>1){std::cout << "NS-3: Loaded" << std::endl;}
		} else if (buffer[0] == 'R') {
			if(debug>1){std::cout << "NS-3: Resume" << std::endl;}
			out_len = Simulator::ToggleFreeze(false);
			if(debug>1){std::cout << "NS-3: Resumed" << std::endl;}
		} 
    std::cout <<"send response " << out_len << std::endl;
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
	//LogComponentEnableAll(LOG_PREFIX_NODE);
	//LogComponentEnableAll(LOG_PREFIX_FUNC);
	//LogComponentEnable("FreezeTimeSynchronizer", LOG_INFO);
	//LogComponentEnable("WallClockSynchronizer", LOG_INFO);
	//LogComponentEnableAll(LOG_FUNCTION);
	//LogComponentEnableAll(LOG_INFO);
	//LogComponentEnableAll(LOG_LOGIC);
	LogComponentEnable("MalProxyTopology", LOG_INFO);
	//LogComponentEnableAll(LOG_DEBUG);
	//LogComponentEnable("CsmaNetDevice", LOG_INFO);
	//LogComponentEnable("BridgeNetDevice", LOG_FUNCTION);
	//LogComponentEnable("PointToPointNetDevice", LOG_FUNCTION);
	//LogComponentEnable("TapBridge", LOG_INFO);
	
	//LogComponentEnable("CsmaNetDevice", LOG_FUNCTION);
	//LogComponentEnable("CsmaNetDevice", LOG_LOGIC);
	//LogComponentEnable("CsmaNetDevice", LOG_INFO);
	//LogComponentEnable("BridgeNetDevice", LOG_FUNCTION);

	//LogComponentEnable("TapBridge", LOG_FUNCTION);
	//LogComponentEnable("TapBridge", LOG_LOGIC);
	//LogComponentEnable("TapBridge", LOG_INFO);

	//LogComponentEnable("Ipv4Interface", LOG_FUNCTION);
	//LogComponentEnable("Ipv4Interface", LOG_INFO);

	//LogComponentEnable("Node", LOG_INFO);

	//LogComponentEnable("Ipv4L3Protocol", LOG_FUNCTION);
	//LogComponentEnable("Ipv4L3Protocol", LOG_INFO);

	//LogComponentEnable("Ipv4EndPointDemux", LOG_FUNCTION);
	//LogComponentEnable("Ipv4EndPointDemux", LOG_DEBUG);
	//LogComponentEnable("Ipv4EndPointDemux", LOG_LOGIC);

	//LogComponentEnable("Ipv4EndPoint", LOG_FUNCTION);
	//LogComponentEnable("Ipv4EndPoint", LOG_DEBUG);
	//LogComponentEnable("Ipv4EndPoint", LOG_LOGIC);

	//LogComponentEnable("MalProxyApplication", LOG_FUNCTION);
	//LogComponentEnable("MalProxyApplication", LOG_LOGIC);
	//LogComponentDisable("WallClockSynchronizer", LOG_FUNCTION);

	//LogComponentEnable("TcpNewReno", LOG_INFO);
	//LogComponentEnable("TcpNewReno", LOG_FUNCTION);
	//LogComponentEnable("TcpNewReno", LOG_LOGIC);

	//LogComponentEnable("UdpL4Protocol", LOG_INFO);
	//LogComponentEnable("UdpL4Protocol", LOG_FUNCTION);
	//LogComponentEnable("UdpL4Protocol", LOG_LOGIC);

	//LogComponentEnable("UdpSocketImpl", LOG_INFO);
	//LogComponentEnable("UdpSocketImpl", LOG_FUNCTION);
	//LogComponentEnable("UdpSocketImpl", LOG_LOGIC);

	//LogComponentEnable("TcpL4Protocol", LOG_INFO);
	//LogComponentEnable("TcpL4Protocol", LOG_FUNCTION);
	//LogComponentEnable("TcpL4Protocol", LOG_LOGIC);

	//LogComponentEnable("TcpSocketBase", LOG_INFO);
	//LogComponentEnable("TcpSocketBase", LOG_FUNCTION);
	//LogComponentEnable("TcpSocketBase", LOG_LOGIC);
	
	//LogComponentEnable("MalProxyApplication", LOG_INFO);
	//LogComponentEnable("MalProxyApplication", LOG_LOGIC);
	//LogComponentEnable("MalProxyApplication", LOG_FUNCTION);

	//Packet::EnablePrinting();
	//Packet::EnableChecking();

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

	NS_LOG_INFO("HJLEE: create terminal nodes");
  terminals.Create (num_terminal);

	NS_LOG_INFO("HJLEE: create a switch node");


	NS_LOG_INFO("HJLEE: CSMA");
	
  InternetStackHelper internet;

	//Ipv4NixVectorHelper nixRouting;
  //Ipv4StaticRoutingHelper staticRouting;

  //Ipv4ListRoutingHelper listRH;
  //listRH.Add (staticRouting, 0);
  //listRH.Add (nixRouting, 10);

  //internet.SetRoutingHelper (listRH);  // has effect on the next Install ()
  internet.Install (terminals);
  //internet.Install (terminals.Get(2));
  //internet.Install (terminals.Get(3));

	if (network.compare("default")==0) {

		CsmaHelper csma;
		csma.SetChannelAttribute ("DataRate", StringValue ("10Mbps"));
		csma.SetChannelAttribute ("Delay", StringValue ("0.05ms"));
		csma.Install(terminals);
		NodeContainer csmaSwitch;
		NetDeviceContainer switchDevices;
		csmaSwitch.Create (1);
		internet.Install (csmaSwitch);
		for (int i = 0; i < num_terminal; i++) {
			NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
			terminalDevices.Add (link.Get (0));
			switchDevices.Add (link.Get (1));
		}
		// Create the bridge netdevice, which will do the packet switching
		Ptr<Node> switchNode = csmaSwitch.Get (0);
		BridgeHelper bridge;
		bridge.Install (switchNode, switchDevices);
	} else if (network.compare("simple")==0) {
	

	}  
/*
	else if (network.compare("delay")==0) {
		NodeContainer csmaSwitch;
		NetDeviceContainer switchDevices;
		csmaSwitch.Create (1);
		internet.Install (csmaSwitch);
		readDelay();
		for (int i = 0; i < num_terminal; i++) {
			int delay = 250;
			if (delayMap.find(i) != delayMap.end()) {
				delay = delayMap[i] * 1000;
			}
			std::cout << "DELAY: " << delay << std::endl;
			NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
			csma.SetChannelAttribute ("Delay", TimeValue(MicroSeconds(delay)));
			terminalDevices.Add (link.Get (0));
			switchDevices.Add (link.Get (1));
		}
		// Create the bridge netdevice, which will do the packet switching
		Ptr<Node> switchNode = csmaSwitch.Get (0);
		BridgeHelper bridge;
		bridge.Install (switchNode, switchDevices);
	} else {
		NS_LOG_INFO("USE NETWORK PARAM " << network);
		//NodeContainer csmaSwitch;
		NetDeviceContainer terminalLinks[num_terminal];
		NodeContainer routers;
		NetDeviceContainer routerLinks[num_terminal];
		NetDeviceContainer rtLinks[num_terminal];
		NetDeviceContainer trLinks[num_terminal];
		//csmaSwitch.Create (num_terminal);
		routers.Create(num_terminal);
		
		for (int i = 0; i < num_terminal; i++) {
			terminalLinks[i] = csma.Install (NodeContainer (terminals.Get (i), routers.Get(i)));
			terminalDevices.Add (terminalLinks[i].Get (0));
			rtLinks[i].Add (terminalLinks[i].Get (1));
			trLinks[i].Add (terminalLinks[i].Get (0));
		}
		uint64_t delay = 0;
		uint32_t src, dest;
		topology.open(network.c_str());
		NetDeviceContainer ndc[num_terminal*num_terminal];
		int links = 0;

		while (readKingTopology(&src, &dest, &delay, num_terminal)) {
			csma.SetChannelAttribute ("Delay", TimeValue(MicroSeconds(delay)));
			ndc[links] = csma.Install( NodeContainer (routers.Get(src), routers.Get(dest) ));
			routerLinks[dest].Add(ndc[links].Get(0));
			routerLinks[src].Add(ndc[links].Get(1));
			links++;
			std::cout << "adding a link " << links-1 << " from " << src+1 << " to " << dest+1 << " delay: " << delay << std::endl;
		}

		internet.Install (routers);
		Ipv4AddressHelper address;
		Ipv4InterfaceContainer ipic[links];
		address.SetBase ("10.1.1.0", "255.255.255.0"); // 24
		//address.SetBase ("10.1.0.0", "255.255.0.0"); // 24
		for (int i = 0; i < num_terminal; i++) {
			char basestr[20];
			//address.NewNetwork();
			sprintf(basestr, "10.%d.%d.0", i+1, i+1);
			address.SetBase (basestr, "255.255.255.0"); // 24
			ipic[i] = address.Assign(rtLinks[i]);
			address.NewNetwork();
			sprintf(basestr, "10.%d.0.0", i+1);
			address.SetBase (basestr, "255.255.0.0"); // 16
			std::cout << "assigning address to rt link " << i << " : " << ipic[i].GetAddress(0, 0) << std::endl;
			ipic[i] = address.Assign(trLinks[i]);
			std::cout << "assigning address to tr link " << i << " : " << ipic[i].GetAddress(0, 0) << std::endl;
			ipic[i] = address.Assign(routerLinks[i]);
			std::cout << "assigning address to router link " << i << " : " << ipic[i].GetAddress(0, 0) << std::endl;
			address.NewNetwork();
			std::cout << "next" << std::endl;
		}

		PopulateArpCache();
		globalRouting = false;
		//Ipv4StaticRoutingHelper staticRouting;
		//staticRouting.AddMulticastRoute(routers, source, group, input, output);
		std::cout << " pupulate routing tables " << std::endl;
		Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	}
*/
	
  // Add internet stack to the terminals

	if (globalRouting) {
		Ipv4AddressHelper ipv4;
		ipv4.SetBase (ip_base_str, "255.255.255.0");
		ipv4.Assign (terminalDevices);
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
