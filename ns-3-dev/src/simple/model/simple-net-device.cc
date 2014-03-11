/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "simple-net-device.h"
#include "simple-channel.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"

#include "ns3/ipv4-header.h"
#include "ns3/arp-header.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/message.h"
#include "ns3/mal-proxy.h"
#include "ns3/ethernet-header.h"
#include "ns3/ethernet-trailer.h"
#include "ns3/llc-snap-header.h"
NS_LOG_COMPONENT_DEFINE ("SimpleNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SimpleNetDevice);

TypeId 
SimpleNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<SimpleNetDevice> ()
    .AddAttribute ("ReceiveErrorModel",
                   "The receiver error model used to simulate packet loss",
                   PointerValue (),
                   MakePointerAccessor (&SimpleNetDevice::m_receiveErrorModel),
                   MakePointerChecker<ErrorModel> ())
    .AddTraceSource ("PhyRxDrop",
                     "Trace source indicating a packet has been dropped by the device during reception",
                     MakeTraceSourceAccessor (&SimpleNetDevice::m_phyRxDropTrace))
  ;
  return tid;
}

SimpleNetDevice::SimpleNetDevice ()
  : m_channel (0),
    m_node (0),
    m_mtu (1500),
    //m_mtu (0xffff),
    m_ifIndex (0)
{
}

void
SimpleNetDevice::AddHeaderTrailer (Ptr<Packet> p,   Mac48Address source,  Mac48Address dest,  uint16_t protocolNumber)
{

	EthernetHeader header (false);
	header.SetSource (source);
	header.SetDestination (dest);

	EthernetTrailer trailer;

	uint16_t lengthType = 0;
	lengthType = protocolNumber;


	LlcSnapHeader llc;
	llc.SetType (protocolNumber);
	p->AddHeader (llc);

	//
	// This corresponds to the length interpretation of the lengthType 
	// field but with an LLC/SNAP header added to the payload as in 
	// IEEE 802.2
	//
	lengthType = p->GetSize ();

	//
	// All Ethernet frames must carry a minimum payload of 46 bytes.  The 
	// LLC SNAP header counts as part of this payload.  We need to padd out
	// if we don't have enough bytes.  These must be real bytes since they 
	// will be written to pcap files and compared in regression trace files.
	//
	if (p->GetSize () < 46)
	{
		uint8_t buffer[46];
		memset (buffer, 0, 46);
		Ptr<Packet> padd = Create<Packet> (buffer, 46 - p->GetSize ());
		p->AddAtEnd (padd);
	}

	NS_ASSERT_MSG (p->GetSize () <= GetMtu (),
			"CsmaNetDevice::AddHeader(): 802.3 Length/Type field with LLC/SNAP: "
			"length interpretation must not exceed device frame size minus overhead " << p->GetSize() << ":" << GetMtu());

	NS_LOG_LOGIC ("header.SetLengthType (" << lengthType << ")");
	header.SetLengthType (lengthType);
	p->AddHeader (header);

	if (Node::ChecksumEnabled ())
	{
		trailer.EnableFcs (true);
	}
	trailer.CalcFcs (p);
	p->AddTrailer (trailer);
}

Ipv4Header SimpleNetDevice::GetIPHeader(Ptr<Packet> packet_received)
{
  Ptr<Packet> packet = packet_received->Copy ();
  EthernetTrailer trailer;
  packet->RemoveTrailer (trailer);
  EthernetHeader header (false);
  packet->RemoveHeader (header);

	Ipv4Header ipHeader;
	packet->PeekHeader(ipHeader);
	return ipHeader;
}


bool 
SimpleNetDevice::GhostIntercept(Ptr<Packet> packet_received, bool sending, bool *writeToTap, Mac48Address from)
{
	ProfileFunction("GhostIntercept", true);
	NS_LOG_FUNCTION(this << " UID : " << packet_received->GetUid() << " NODE: " << m_node->GetId());
	MaliciousTag mtag;
	int protocol;
	packet_received->PeekPacketTag(mtag);
	Ptr<Packet> packet = packet_received->Copy ();
	Ipv4Header ipHeader;
	packet->PeekHeader(ipHeader);
	//uint32_t size = packet->PeekHeader(ipHeader);

	NS_LOG_INFO("Packet " << packet_received->GetUid() << " IP header " << ipHeader);
	protocol = ipHeader.GetProtocol();
	if (protocol != UdpL4Protocol::PROT_NUMBER && protocol != TcpL4Protocol::PROT_NUMBER) {
				NS_LOG_DEBUG( "GhostIntercept false 7: protocol" << protocol);
				packet->AddHeader(ipHeader);
		// Proxy only receives TCP and UDP packets
		ProfileFunction("GhostIntercept", false);
		return false;
	}

	// don't process UDP packets that you are receiving
	/* // REPLAY requires to look at packets being received
  	  if (protocol == UdpL4Protocol::PROT_NUMBER && !sending) {
		 return false;
	}
	*/


	if (sending) {
		if (mtag.GetMalStatus() == MaliciousTag::FROMPROXY && mtag.GetSrcNode() == m_node->GetId()) {
			ProfileFunction("GhostIntercept", false);
			return false;
			// if sending, and from my proxy, no point to intercept, but possibly to write tap...?
		}
	}
	Ptr<Packet> pcopy = packet->Copy();
	pcopy->RemoveHeader(ipHeader);
  if (ipHeader.GetPayloadSize () < pcopy->GetSize ())
    {
      pcopy->RemoveAtEnd (pcopy->GetSize () - ipHeader.GetPayloadSize ());
    }
	
	uint16_t destPort = 0;
	// check port: if the port is not a listening one, create a new one
	if (protocol == UdpL4Protocol::PROT_NUMBER) {
		UdpHeader udpHeader;
		pcopy->PeekHeader (udpHeader);
		destPort = udpHeader.GetDestinationPort() ;
		if (!m_node->IsLocalPort(false, destPort)) {
			ProfileFunction("GhostIntercept", false);
			return false;
		}

		//see if the malicious proxy cares about this message type
		/* XXX - Now this is done at TapDevice
		Ptr<Application> app = m_node->GetApplication(0);
		uint8_t *msg = new uint8_t[pcopy->GetSize()];
		pcopy->CopyData(msg, pcopy->GetSize());
		uint32_t offset = 8; //size of udp header
		Message *m = new Message(msg+offset);
		Application *ptr;
		ptr = GetPointer(app);
		Ptr<MalProxy> proxy = Ptr<MalProxy>((MalProxy*)ptr);
		
		if (!proxy->MalMsg(m)) {
				std::cout << "GhostIntercept false 3 type: " << m->type << std::endl;
			delete m;
			delete msg;
			ProfileFunction("GhostIntercept", false);
			return false;
		}
		delete m;
		delete msg;	
    */
	}
	if (protocol == TcpL4Protocol::PROT_NUMBER) {
		TcpHeader tcpHeader;
		pcopy->PeekHeader (tcpHeader);
		destPort = tcpHeader.GetDestinationPort() ;
		if (!m_node->IsLocalPort(true, destPort)) {
			ProfileFunction("GhostIntercept", false);
			return false;
		}
	}
	
	packet->RemovePacketTag(mtag);
	mtag.StartTimer();
	mtag.SetIPDest(ipHeader.GetDestination().Get());
	mtag.SetIPSource(ipHeader.GetSource().Get());
	if (mtag.GetSrcNode() != m_node->GetId()) { // from outside - intercept
		NS_LOG_INFO(" Packet from outside. destination should be me " << ipHeader.GetDestination()); // actually, the destination should be already me
	}
	else {
		if (mtag.GetMalStatus() == MaliciousTag::FROMTAP) {
			NS_LOG_INFO(" Intercept packet from my TAP: to handle this packet should muck destination from original "
			<< ipHeader.GetDestination() << " to myaddr " << ipHeader.GetSource()); // from my tap device. should intercept
			mtag.SetSpoof(true);	
		}
		else {
			// from my proxy, do not intercept and it should be written on my tap
			if (m_node->IsLocalAddress(ipHeader.GetDestination())) {
				NS_LOG_INFO("from proxy and me for mine - spoof: " << mtag.IsSpoof() << " to my tap");
				(*writeToTap) = true;
				packet->AddPacketTag(mtag);
				ProfileFunction("GhostIntercept", false);
				return false;
			} 
		}
	}
	packet->AddPacketTag(mtag);
	// add sending information

	// make ipv4 receive the packet
	ProfileFunction("GhostIntercept", false);

	if (m_node->IsMalicious()) m_rxCallback (this, packet, 2048, from);
	//if (m_node->IsMalicious()) m_rxCallback (this, packet, 2048, header.GetSource ()); 
	return true;
}

void
SimpleNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol,
                          Mac48Address to, Mac48Address from)
{
	ProfileFunction("SimpleReceive", true);
	NS_LOG_FUNCTION (m_node->GetId() << " pcaket: " << packet->GetUid() << protocol << to << from);
	NetDevice::PacketType packetType;
	
	if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet) )
	{
		m_phyRxDropTrace (packet);
		ProfileFunction("SimpleReceive", false);
		return;
	}

	Ptr<Packet> originalPacket = packet->Copy ();

	EthernetTrailer trailer;
	originalPacket->RemoveTrailer (trailer);
	if (Node::ChecksumEnabled ())
	{
		trailer.EnableFcs (true);
	}

	trailer.CheckFcs (originalPacket);
	bool crcGood = trailer.CheckFcs (originalPacket);
	if (!crcGood)
	{
		NS_LOG_INFO ("CRC error on originalPacket " << originalPacket);
		m_phyRxDropTrace (originalPacket);
	ProfileFunction("SimpleReceive", false);
		return;
	}

	EthernetHeader header (false);
	originalPacket->RemoveHeader (header);
	uint16_t protocol_new;

	NS_LOG_INFO("Ethernet Header: " << header);
	//
	// If the length/type is less than 1500, it corresponds to a length 
	// interpretation originalPacket.  In this case, it is an 802.3 originalPacket and 
	// will also have an 802.2 LLC header.  If greater than 1500, we
	// find the protocol number (Ethernet type) directly.
	//
	if (header.GetLengthType () <= 1500)
	{
    if (originalPacket->GetSize() < header.GetLengthType()) return;
		NS_ASSERT (originalPacket->GetSize () >= header.GetLengthType ());
		uint32_t padlen = originalPacket->GetSize () - header.GetLengthType ();
		if (padlen > 46) return;
		if (padlen > 0)
		{
			originalPacket->RemoveAtEnd (padlen);
		}

		LlcSnapHeader llc;
		originalPacket->RemoveHeader (llc);
		protocol_new = llc.GetType ();
	}
	else
	{
		protocol_new = header.GetLengthType ();
	}
	bool writeToTap = false;
	
	if (m_node->IsMalicious() && protocol == 2048) {
		//NS_LOG_INFO("try intercept while receiving at node " << m_node->GetId());
		//if (GhostIntercept(originalPacket, false, &writeToTap, from)) return;
	}


	if (to == m_address)
	{
		packetType = NetDevice::PACKET_HOST;
	}
	else if (to.IsBroadcast ())
	{
		packetType = NetDevice::PACKET_BROADCAST;
	}
	else if (to.IsGroup ())
	{
		packetType = NetDevice::PACKET_MULTICAST;
	}
	else 
	{
		packetType = NetDevice::PACKET_OTHERHOST;
	}

	/*
		 if (protocol_new == 2048 && m_node->IsMalicious()
		 && m_node->IsLocalAddress(GetIPHeader(packet).GetDestination())) {
		 writeToTap = true;
		 }
	 */

	if (!m_promiscCallback.IsNull ())
	{
		m_promiscCallback (this, originalPacket, protocol, from, to, packetType); // this writes to tap
	} 
	ProfileFunction("SimpleReceive", false);

	 /*if (writeToTap) return;*/
	 if (packetType != PACKET_OTHERHOST)
		 m_rxCallback (this, packet, protocol_new, from); // local up
}

void
SimpleNetDevice::AddChannel (Ptr<SimpleChannel> channel, Mac48Address dest_addr)
{
	uint64_t mac = 0;
	dest_addr.CopyTo((uint8_t*)&mac);
	m_channels[mac] = channel;
  channel->Add (this);
}

uint32_t
SimpleNetDevice::GetNChannels (void)
{
  return m_channels.size ();
}

/*
Ptr<Channel>
SimpleNetDevice::GetChannel (uint32_t i) 
{
  return m_channels[i];
}
*/

// hjlee deprecate later
void 
SimpleNetDevice::SetChannel (Ptr<SimpleChannel> channel)
{
  m_channel = channel;
  m_channel->Add (this);
}

void
SimpleNetDevice::SetReceiveErrorModel (Ptr<ErrorModel> em)
{
  m_receiveErrorModel = em;
}

void 
SimpleNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}
uint32_t 
SimpleNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}
Ptr<Channel> 
SimpleNetDevice::GetChannel (void) const
{
  return m_channel;
}
void
SimpleNetDevice::SetAddress (Address address)
{
  m_address = Mac48Address::ConvertFrom (address);
}
Address 
SimpleNetDevice::GetAddress (void) const
{
  //
  // Implicit conversion from Mac48Address to Address
  //
  return m_address;
}
bool 
SimpleNetDevice::SetMtu (const uint16_t mtu)
{
  m_mtu = mtu;
  return true;
}
uint16_t 
SimpleNetDevice::GetMtu (void) const
{
  return m_mtu;
}
bool 
SimpleNetDevice::IsLinkUp (void) const
{
  return true;
}
void 
SimpleNetDevice::AddLinkChangeCallback (Callback<void> callback)
{}
bool 
SimpleNetDevice::IsBroadcast (void) const
{
  return true;
}
Address
SimpleNetDevice::GetBroadcast (void) const
{
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}
bool 
SimpleNetDevice::IsMulticast (void) const
{
  return false;
}
Address 
SimpleNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address::GetMulticast (multicastGroup);
}

Address SimpleNetDevice::GetMulticast (Ipv6Address addr) const
{
  return Mac48Address::GetMulticast (addr);
}

bool 
SimpleNetDevice::IsPointToPoint (void) const
{
  return false;
}

bool 
SimpleNetDevice::IsBridge (void) const
{
  return false;
}

bool 
SimpleNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (" pcaket: " << packet->GetUid() << dest << protocolNumber);
  Mac48Address to = Mac48Address::ConvertFrom (dest);
  //m_channel->Send (packet, protocolNumber, to, m_address, this);
	SendFrom(packet, m_address, dest, protocolNumber);
  return true;
}

bool 
SimpleNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
	NS_LOG_FUNCTION (" pcaket: " << packet->GetUid() << source << dest << protocolNumber);
	ProfileFunction("SimpleSendFrom", true);
	Mac48Address to = Mac48Address::ConvertFrom (dest);
	Mac48Address from = Mac48Address::ConvertFrom (source);
	AddHeaderTrailer (packet, from, to, protocolNumber);

	//to avoid having to write a hash function for uint8_t[6], just sticking
	// the mac address into a uint64_t
	uint64_t mac = 0;
	to.CopyTo((uint8_t*)&mac);
	__gnu_cxx::hash_map<uint64_t, Ptr<SimpleChannel> >::iterator ch = m_channels.find(mac);
	ProfileFunction("SimpleSendFrom", false);
	if (ch != m_channels.end()) {
		(ch->second)->Send (packet, protocolNumber, to, from, this);
	} else {
		NS_LOG_INFO(" no matching address for " << to << ", broadcast ");
		for (ch = m_channels.begin(); ch != m_channels.end(); ch++) {
			(ch->second)->Send (packet, protocolNumber, to, from, this);
		}
	}
	return true;
}

Ptr<Node> 
SimpleNetDevice::GetNode (void) const
{
  return m_node;
}
void 
SimpleNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}
bool 
SimpleNetDevice::NeedsArp (void) const
{
  return true;
}
void 
SimpleNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  m_rxCallback = cb;
}

void
SimpleNetDevice::DoDispose (void)
{
  m_channel = 0;
  m_node = 0;
  m_receiveErrorModel = 0;
  NetDevice::DoDispose ();
}


void
SimpleNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  m_promiscCallback = cb;
}

bool
SimpleNetDevice::SupportsSendFrom (void) const
{
  return true;
}

} // namespace ns3
