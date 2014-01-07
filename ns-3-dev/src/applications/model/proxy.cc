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
 *
 * Author:  Tom Henderson (tomhend@u.washington.edu)
 */
#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/udp-header.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "proxy.h"

using namespace std;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Proxy");
NS_OBJECT_ENSURE_REGISTERED (Proxy);

TypeId 
Proxy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Proxy")
    .SetParent<Application> ()
    .AddConstructor<Proxy> ()
    .AddAttribute ("Local", "The Address on which to Bind the rx socket.",
                   AddressValue (),
                   MakeAddressAccessor (&Proxy::m_local),
                   MakeAddressChecker ())
    .AddAttribute ("Protocol", "The type id of the protocol to use for the rx socket.",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&Proxy::m_tid),
                   MakeTypeIdChecker ())
    .AddTraceSource ("Rx", "A packet has been received",
                     MakeTraceSourceAccessor (&Proxy::m_rxTrace))
  ;
  return tid;
}

Proxy::Proxy ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_totalRx = 0;
}

Proxy::~Proxy()
{
  NS_LOG_FUNCTION (this);
}

uint32_t Proxy::GetTotalRx () const
{
  return m_totalRx;
}

Ptr<Socket>
Proxy::GetListeningSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

std::list<Ptr<Socket> >
Proxy::GetAcceptedSockets (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socketList;
}

void Proxy::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socketList.clear ();

  // chain up
  Application::DoDispose ();
}


// Application Methods
void Proxy::StartApplication ()    // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      m_socket->Bind (m_local);
      m_socket->Listen ();
      m_socket->ShutdownSend ();
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: joining multicast on a non-UDP socket");
            }
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&Proxy::HandleRead, this));
  m_socket->SetAcceptCallback (
    MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
    MakeCallback (&Proxy::HandleAccept, this));
  m_socket->SetCloseCallbacks (
    MakeCallback (&Proxy::HandlePeerClose, this),
    MakeCallback (&Proxy::HandlePeerError, this));
	NS_LOG_INFO("==================");
}

void Proxy::StopApplication ()     // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  while(!m_socketList.empty ()) //these are accepted sockets, close them
    {
      Ptr<Socket> acceptedSocket = m_socketList.front ();
      m_socketList.pop_front ();
      acceptedSocket->Close ();
    }
  if (m_socket) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

void Proxy::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
	NS_LOG_INFO(this << socket);
  Ptr<Packet> packet;
  Address from;
  while (packet = socket->RecvFrom (from))
    {
      if (packet->GetSize () == 0)
        { //EOF
          break;
        }
			UdpHeader header;
			packet->PeekHeader(header);
			NS_LOG_INFO("heder" << header);
			header.Print(std::cout);
      if (InetSocketAddress::IsMatchingType (from))
        {
          m_totalRx += packet->GetSize ();
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
          NS_LOG_INFO ("Received " << packet->GetSize () << " bytes from " <<
                       address.GetIpv4 () << " [" << address << "]"
                                   << " total Rx " << m_totalRx);
          //cast address to void , to suppress 'address' set but not used 
          //compiler warning in optimized builds
          (void) address;
        }
      m_rxTrace (packet, from);
    }
}

void Proxy::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_INFO ("Proxy, peerClose");
}
 
void Proxy::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_INFO ("Proxy, peerError");
}
 

void Proxy::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
	NS_LOG_INFO("ACCEPT from " << from);
  s->SetRecvCallback (MakeCallback (&Proxy::HandleRead, this));
  m_socketList.push_back (s);
	// because it's proxy, create a new socket to the destination
}

} // Namespace ns3
