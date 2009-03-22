/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2009 IITP RAS
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
 * Author: Kirill Andreev <andreev@iitp.ru>
 */

#include "ns3/mesh-wifi-interface-mac.h"
#include "ie-dot11s-configuration.h"
#include "ie-dot11s-peer-management.h"
#include "peer-manager-plugin.h"
#include "ns3/mesh-wifi-mac-header.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("PeerManager");
namespace ns3 {
Dot11sPeerManagerMacPlugin::Dot11sPeerManagerMacPlugin (uint32_t interface, Ptr<Dot11sPeerManagerProtocol> protocol)
{
  m_ifIndex = interface;
  m_protocol = protocol;
}

Dot11sPeerManagerMacPlugin::~Dot11sPeerManagerMacPlugin ()
{
}

void
Dot11sPeerManagerMacPlugin::SetParent (Ptr<MeshWifiInterfaceMac> parent)
{
  NS_LOG_UNCOND("ADD PARENT");
  m_parent = parent;
}

bool
Dot11sPeerManagerMacPlugin::Receive (Ptr<Packet> packet, const WifiMacHeader & header)
{
  NS_LOG_UNCOND("received a frame");
  if(header.IsBeacon())
  {
    NS_LOG_UNCOND("Beacon recevied by PM from"<<header.GetAddr2 ());
    IeDot11sBeaconTiming beaconTiming;
    Ptr<Packet> myBeacon = packet->Copy();
    MgtBeaconHeader beacon_hdr;
    myBeacon->RemoveHeader(beacon_hdr);
    if(myBeacon->GetSize () == 0)
      NS_LOG_UNCOND("Empty");
    bool meshBeacon = false;
    if(beaconTiming.FindMyInformationElement(myBeacon))
      meshBeacon = true;
    m_protocol->ReceiveBeacon(
        m_ifIndex,
        meshBeacon,
        beaconTiming,
        header.GetAddr2(),
        Simulator::Now(),
        MicroSeconds(beacon_hdr.GetBeaconIntervalUs())
        );

#if 0
      packet->RemoveHeader (beacon);
      m_peerManager->SetReceivedBeaconTimers (
        GetAddress (),
        from,
        Simulator::Now (),
        MicroSeconds (beacon.GetBeaconIntervalUs()),
        beacon.GetIeDot11sBeaconTiming ()
      );
#endif
  }
  if(header.IsMultihopAction())
  {
    WifiMeshHeader meshHdr;
    packet->RemoveHeader (meshHdr);
    WifiMeshMultihopActionHeader multihopHdr;
      
    //parse multihop action header:
      
    packet->RemoveHeader (multihopHdr);
      
    WifiMeshMultihopActionHeader::ACTION_VALUE actionValue = multihopHdr.GetAction ();
    if(multihopHdr.GetCategory () != WifiMeshMultihopActionHeader::MESH_PEER_LINK_MGT)
      return false;
#if 0
        {
        case WifiMeshMultihopActionHeader::MESH_PEER_LINK_MGT:
        {
          Mac48Address peerAddress;
          MeshMgtPeerLinkManFrame peer_frame;
          if (hdr->GetAddr1 () != GetAddress ())
            return;
          peerAddress = hdr->GetAddr2 ();
          packet->RemoveHeader (peer_frame);
          if (actionValue.peerLink != WifiMeshMultihopActionHeader::PEER_LINK_CLOSE)
            {
              //check Supported Rates
              SupportedRates rates = peer_frame.GetSupportedRates ();
              for (uint32_t i = 0; i < m_stationManager->GetNBasicModes (); i++)
                {
                  WifiMode mode = m_stationManager->GetBasicMode (i);
                  if (!rates.IsSupportedRate (mode.GetDataRate ()))
                    {
                      m_peerManager->ConfigurationMismatch (GetAddress(), peerAddress);
                      return;
                    }
                }
              //Check SSID
              if (!peer_frame.GetMeshId ().IsEqual(GetSsid()))
                {
                  m_peerManager->ConfigurationMismatch (GetAddress(), peerAddress);
                  return;
                }
            }
          switch (actionValue.peerLink)
            {
            case WifiMeshMultihopActionHeader::PEER_LINK_CONFIRM:
              m_peerManager->SetConfirmReceived (
                GetAddress (),
                peerAddress,
                peer_frame.GetAid (),
                peer_frame.GetIeDot11sPeerManagement (),
                m_meshConfig
              );
              return;
            case WifiMeshMultihopActionHeader::PEER_LINK_OPEN:
              m_peerManager->SetOpenReceived (
                GetAddress (),
                peerAddress,
                peer_frame.GetIeDot11sPeerManagement (),
                m_meshConfig
              );
              return;
            case WifiMeshMultihopActionHeader::PEER_LINK_CLOSE:
              m_peerManager->SetCloseReceived (
                GetAddress (),
                peerAddress,
                peer_frame.GetIeDot11sPeerManagement ()
              );
              return;
            default:
              return;
            }
          break;
#endif
        }
  return false;
}

bool
Dot11sPeerManagerMacPlugin::UpdateOutcomingFrame (Ptr<Packet> packet, WifiMacHeader & header, Mac48Address from, Mac48Address to) const
{
  return false;
}

void
Dot11sPeerManagerMacPlugin::UpdateBeacon (MeshWifiBeacon & beacon) const
{
#if 0
  NS_LOG_UNCOND("I am sending a beacon");
  Ptr<IeDot11sPrep>  prep = Create<IeDot11sPrep> ();
#endif
  Ptr<IeDot11sBeaconTiming>  beaconTiming = 
    m_protocol->SendBeacon(
        m_ifIndex,
        Simulator::Now(),
        MicroSeconds(beacon.BeaconHeader().GetBeaconIntervalUs()));
  beacon.AddInformationElement(beaconTiming);
}
}//namespace ns3
