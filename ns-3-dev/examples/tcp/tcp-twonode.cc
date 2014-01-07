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

// Network topology (default)
//
//     n1--- n0---n2            .
//


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TwoNode");

int 
main (int argc, char *argv[])
{

  //
  // Set up some default values for the simulation.
  //
  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
	//LogComponentEnableAll(LOG_FUNCTION);
	//LogComponentEnableAll(LOG_INFO);
	//LogComponentEnableAll(LOG_LOGIC);
	LogComponentEnableAll(LOG_PREFIX_NODE);
	LogComponentEnableAll(LOG_PREFIX_FUNC);
	LogComponentEnable("TwoNode", LOG_INFO);
	//LogComponentEnableAll(LOG_DEBUG);
	/*
	LogComponentEnable("CsmaNetDevice", LOG_FUNCTION);
	LogComponentEnable("CsmaNetDevice", LOG_LOGIC);
	LogComponentEnable("CsmaNetDevice", LOG_INFO);
	*/
	//LogComponentEnable("BridgeNetDevice", LOG_FUNCTION);

	LogComponentEnable("TapBridge", LOG_FUNCTION);
	//LogComponentEnable("TapBridge", LOG_LOGIC);
	LogComponentEnable("TapBridge", LOG_INFO);

	LogComponentEnable("Ipv4Interface", LOG_FUNCTION);
	LogComponentEnable("Ipv4Interface", LOG_INFO);

	LogComponentEnable("Node", LOG_INFO);

	LogComponentEnable("Ipv4L3Protocol", LOG_FUNCTION);
	LogComponentEnable("Ipv4L3Protocol", LOG_INFO);

	LogComponentEnable("Ipv4EndPointDemux", LOG_FUNCTION);
	LogComponentEnable("Ipv4EndPointDemux", LOG_DEBUG);
	LogComponentEnable("Ipv4EndPointDemux", LOG_LOGIC);

	LogComponentEnable("TcpEchoServerApplication", LOG_FUNCTION);
	LogComponentEnable("TcpEchoServerApplication", LOG_INFO);
	//LogComponentDisable("WallClockSynchronizer", LOG_FUNCTION);

	LogComponentEnable("TcpNewReno", LOG_INFO);
	LogComponentEnable("TcpNewReno", LOG_FUNCTION);
	LogComponentEnable("TcpNewReno", LOG_LOGIC);

	LogComponentEnable("TcpL4Protocol", LOG_INFO);
	LogComponentEnable("TcpL4Protocol", LOG_FUNCTION);
	LogComponentEnable("TcpL4Protocol", LOG_LOGIC);

	LogComponentEnable("TcpSocketBase", LOG_INFO);
	LogComponentEnable("TcpSocketBase", LOG_FUNCTION);
	LogComponentEnable("TcpSocketBase", LOG_LOGIC);

	LogComponentEnable("Ipv4EndPoint", LOG_INFO);
	LogComponentEnable("Ipv4EndPoint", LOG_FUNCTION);
	LogComponentEnable("Ipv4EndPoint", LOG_LOGIC);

	Packet::EnablePrinting();
	//Packet::EnableChecking();

	int32_t malNode = 0;
  CommandLine cmd;
	cmd.AddValue("Mal", "Malicious Node", malNode);
  //cmd.Parse (argc, argv);
	int num_terminal =4;

  NS_LOG_INFO ("Build TwoNode topology.");
	NS_LOG_INFO ("Malicious : " << malNode);

	NS_LOG_INFO("HJLEE: create terminal nodes");
  NodeContainer terminals;
  terminals.Create (num_terminal);

	NS_LOG_INFO("HJLEE: create a switch node");
  NodeContainer csmaSwitch;
  csmaSwitch.Create (1);


	NS_LOG_INFO("HJLEE: CSMA");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("1ms"));

	csma.Install(terminals);
	
  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices;

  for (int i = 0; i < num_terminal; i++) {
		NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
		terminalDevices.Add (link.Get (0));
		switchDevices.Add (link.Get (1));
	}

  // Create the bridge netdevice, which will do the packet switching
  Ptr<Node> switchNode = csmaSwitch.Get (0);
  BridgeHelper bridge;
  bridge.Install (switchNode, switchDevices);
	NS_LOG_INFO("SWITCH ID: " << switchNode->GetId());
	
  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);
  //internet.Install (terminals.Get(2));
  //internet.Install (terminals.Get(3));
  internet.Install (csmaSwitch);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);
  //ipv4.Assign (terminalDevices.Get(3));

  //uint16_t port = 300;
  uint16_t port = 24;
	for (int i=0; i<argc; i++) {
		if (strcmp(argv[i], "-port") == 0) {
			i++;
			port = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-mal") == 0) {
			i++;
			malNode = atoi(argv[i]);
		}
	}
	NS_LOG_INFO("====================================");
	NS_LOG_INFO(" port : " << port);
	NS_LOG_INFO(" mal : " << malNode);
	NS_LOG_INFO("====================================");
  ApplicationContainer apps;
  TcpEchoServerHelper server (Ipv4Address("10.1.1.1"), port);
  apps = server.Install (terminals.Get(0));
  apps.Start (Seconds (10.0));
  apps.Stop (Seconds (600.0));
  TcpEchoServerHelper server2 (Ipv4Address("10.1.1.2"), port);
  apps = server2.Install (terminals.Get(1));
  apps.Start (Seconds (10.0));
  apps.Stop (Seconds (600.0));

	//TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("CONFIGURE_LOCAL"));
	//TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("ConfigureLocal"));
	TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("UseLocal"));
	//TapBridgeHelper tapBridge;tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-left"));
	tapBridge.SetAttribute ("MacAddress", StringValue("01:ff:ff:ff:ff:ff"));
  tapBridge.Install (terminals.Get (0), terminalDevices.Get (0));
	if (malNode >= 0)
	terminals.Get(malNode)->SetMalicious(); // this is how we set a node to be malicious
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-right"));
	tapBridge.SetAttribute ("MacAddress", StringValue("02:ff:ff:ff:ff:ff"));
  tapBridge.Install (terminals.Get (1), terminalDevices.Get (1));
	
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	/* Logging
  NS_LOG_INFO ("Enable pcap tracing.");
  csma.EnablePcapAll ("twonodes");
	AsciiTraceHelper ascii;
	csma.EnableAsciiAll(ascii.CreateFileStream("trace"));
	*/


  //Simulator::Stop (Seconds (600.));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
