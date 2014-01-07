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
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"

#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv4-header.h"
#include "ns3/malicious-tag.h"

#include "udp-echo-server.h"
#include <iostream>
#include <fstream>
#include <netinet/in.h>

using namespace std;
enum TypeField {UNIQUE, BYTE, INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, DOUBLE, FLOAT, BOOL};
#define MSG 10
#define FIELD 40

int messageFormat[MSG][FIELD];
int uniq[MSG];

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UdpEchoServerApplication");
NS_OBJECT_ENSURE_REGISTERED (UdpEchoServer);
void parseFormat(char* filename) {
	ifstream infile;
	infile.open(filename, ifstream::in);
	int msgcount = 0;
	while (!infile.eof()) {
		string line;
		getline(infile, line);
		if (line.length() <= 0 || line.at(0) == '#') {
			continue;
		}
		int numToken = 0;
		int cur = 0;
		while((cur = line.find(":")) != (int) string::npos) {
			string type = line.substr(0, cur);
			line = line.substr(cur+1);
			//cout << type << " " << line << "\n";
			if (type == "UNIQUE") {
				uniq[msgcount] = numToken;
			} else if (type == "BYTE") {
				//cout << "byte" << "\n";
				messageFormat[msgcount][numToken] = BYTE;
				numToken++;
			} else if (type == "INT8") {
				//cout << "int8" << "\n";
				messageFormat[msgcount][numToken] = INT8;
				numToken++;
			} else if (type == "UINT8") {
				//cout << "uint8" << "\n";
				messageFormat[msgcount][numToken] = UINT8;
				numToken++;
			} else if (type == "INT16") {
				//cout << "int16" << "\n";
				messageFormat[msgcount][numToken] = INT16;
				numToken++;
			} else if (type == "UINT16") {
				//cout << "uint16" << "\n";
				messageFormat[msgcount][numToken] = UINT16;
				numToken++;
			} else if (type == "INT32") {
				//cout << "int32" << "\n";
				messageFormat[msgcount][numToken] = INT32;
				numToken++;
			} else if (type == "UINT32") {
				//cout << "uint32" << "\n";
				messageFormat[msgcount][numToken] = UINT32;
				numToken++;
			} else if (type == "INT64") {
				//cout << "int64" << "\n";
				messageFormat[msgcount][numToken] = INT64;
				numToken++;
			} else if (type == "UINT64") {
				//cout << "uint64" << "\n";
				messageFormat[msgcount][numToken] = UINT64;
				numToken++;
			} else if (type == "DOUBLE") {
				//cout << "double" << "\n";
				messageFormat[msgcount][numToken] = DOUBLE;
				numToken++;
			} else if (type == "FLOAT") {
				//cout << "float" << "\n";
				messageFormat[msgcount][numToken] = FLOAT;
				numToken++;
			} else if (type == "BOOL") {
				//cout << "bool" << "\n";
				messageFormat[msgcount][numToken] = BOOL;
				numToken++;
			} else {
				//type should be a number...
				messageFormat[msgcount][numToken] = atoi(type.c_str());
				//cout << numToken << " " << messageFormat[msgcount][numToken] << "\n";
				numToken++;
			}

		}
		msgcount++;
	}
	infile.close(); 
}

int sizeOfType (TypeField t) {
	switch (t) {
		case BYTE: 
			return 1;
		case INT8:
			return 1;
		case UINT8:
			return 1;
		case INT16:
			return 2;
		case UINT16:
			return 2;
		case INT32:
			return 4;
		case UINT32: 
			return 4;
		case INT64:
			return 8;
		case UINT64:
			return 8;
		case DOUBLE:
			return 8;
		case FLOAT:
			return 8;
		case BOOL:
			return 1;
		default:
			return 0;
	}
}


int findMsgType(uint8_t *msg) {
	int offset = 0;
	int i;
	for (i = 0; i <= FIELD; i+=2) {
		if (i == uniq[0]) {
			break;
		}
		offset += sizeOfType((TypeField)messageFormat[0][i])*messageFormat[0][i+1];
	}
	int j;
	for (j = 0; j <= MSG; j++) {
		if (*(uint8_t*)(msg+offset) == messageFormat[j][i+1]) {
			return j;
		}
	}
	return -1;

}

int findFieldOffset(uint8_t *msg, int msgType, int fieldNum, uint32_t *times) {
	int offset = 0;
	int i;

	for (i = 0; i <= FIELD; i+=2) {
		if (i == uniq[msgType]) {
			*times = 1;
		} else if (messageFormat[msgType][i+1] >= 0) {
			*times = messageFormat[msgType][i+1];
		} else { //if (messageFormat[msgType][i+1] == -1) {
			*times = ntohl(*(uint32_t*)(msg+offset));
			printf("offset %d times %x\n", offset, *(uint32_t*)(msg+offset));
			offset += 4;
		}
		if (i/2 == fieldNum) {
			return offset;
		}
		cout << sizeOfType((TypeField)messageFormat[msgType][i]) << " " << *times << "\n";
		offset += sizeOfType((TypeField)messageFormat[msgType][i])*(*times);
		}
		return -1; 
	}



	void changeDOUBLE(uint8_t* msg, int offset) {
		double value = 100000.0;
		double *ptr = (double*)(msg + offset);
		*ptr = value;

	}

	void changeFLOAT(uint8_t* msg, int offset) {
		float value = 10.0;
		float *ptr = (float*)(msg + offset);
		*ptr = value;

	}

	void changeINT8(uint8_t* msg, int offset) {
		int32_t value = 10;
		int32_t *ptr = (int32_t*)(msg + offset);
		*ptr = value;
	}

	void changeUINT8(uint8_t* msg, int offset) {
		uint32_t value = 10;
		uint32_t *ptr = (uint32_t*)(msg + offset);
		*ptr = value;
	}

	void changeINT16(uint8_t* msg, int offset) {
		int32_t value = 10;
		int32_t *ptr = (int32_t*)(msg + offset);
		*ptr = value;
	}

	void changeUINT16(uint8_t* msg, int offset) {
		uint32_t value = 10;
		uint32_t *ptr = (uint32_t*)(msg + offset);
		*ptr = value;
	}

	void changeINT32(uint8_t* msg, int offset) {
		int32_t value = 10;
		int32_t *ptr = (int32_t*)(msg + offset);
		*ptr = value;
	}

	void changeUINT32(uint8_t* msg, int offset) {
		uint32_t value = 10;
		uint32_t *ptr = (uint32_t*)(msg + offset);
		*ptr = value;
	}

	void changeINT64(uint8_t* msg, int offset) {
		int32_t value = 10;
		int32_t *ptr = (int32_t*)(msg + offset);
		*ptr = value;
	}

	void changeUINT64(uint8_t* msg, int offset) {
		uint32_t value = 0x12345678;
		uint32_t *ptr = (uint32_t*)(msg + offset);
		*ptr = value;
	}


	void changeBOOL(uint8_t* msg, int offset) {
		bool value = true;
		bool *ptr = (bool*)(msg + offset);
		*ptr = value;
	}

	bool changeValue(uint8_t* msg, int fieldNum) {

		int msgType = findMsgType(msg);
		if (msgType == -1) {
			cout << "no message format found\n";
			return false;
		}
		uint32_t times;
		int offset = findFieldOffset(msg, msgType, fieldNum, &times); 
		int type = messageFormat[msgType][fieldNum*2];

		cout << "MsgType: " << msgType << " Offset: " << offset << " Type: " << type << " Times: " << times << "\n";

		for (uint32_t i = 0; i < times; i++) {
			switch (type) {
				case BYTE:
					break;
				case INT8:
					changeINT8(msg, offset);
					break;
				case UINT8:
					changeUINT8(msg, offset);
					break;
				case INT16:
					changeINT16(msg, offset);
					break;
				case UINT16:
					changeUINT16(msg, offset);
					break;
				case INT32:
					changeINT32(msg, offset);
					break;
				case UINT32:
					changeUINT32(msg, offset);
					break;
				case INT64:
					changeINT64(msg, offset);
					break;
				case UINT64:
					changeUINT64(msg, offset);
					break;
				case DOUBLE:
					changeDOUBLE(msg, offset);
					break;
				case FLOAT:
					changeFLOAT(msg, offset);
					break;
				case BOOL:
					changeBOOL(msg, offset);
					break;
			}
			offset += sizeOfType((TypeField)type);
		}
		return true;
	}



TypeId
UdpEchoServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UdpEchoServer")
    .SetParent<Application> ()
    .AddConstructor<UdpEchoServer> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&UdpEchoServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

UdpEchoServer::UdpEchoServer ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

UdpEchoServer::~UdpEchoServer()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_socket = 0;
}

void
UdpEchoServer::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void 
UdpEchoServer::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
	//parseFormat("/home/lee747/test/macevivaldi.format");
  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
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

  m_socket->SetRecvCallback (MakeCallback (&UdpEchoServer::HandleRead, this));
}

void 
UdpEchoServer::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

void 
UdpEchoServer::HandleRead (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Ptr<Packet> packet2;
	uint8_t msg[1500];
  Address from;
  while (packet = socket->RecvFrom (from))
    {
			NS_LOG_INFO("=======================================");
			NS_LOG_INFO("CAPTURED THE CONTENTS AT MAL PROXY APP =======================================" << packet->PeekData());
			MaliciousTag mtag_new;
			memcpy(msg, packet->PeekData(), packet->GetSize());
			changeValue(msg, 3);
			Ipv4Address dst;
      if (InetSocketAddress::IsMatchingType (from))
			{
				NS_LOG_INFO ("Received " << packet->GetSize () << " bytes from " <<
						InetSocketAddress::ConvertFrom (from).GetIpv4 ());
				MaliciousTag mtag_old;
				packet->RemovePacketTag(mtag_old);
				mtag_old.Print(std::cerr);
				
				if (mtag_old.GetMalStatus() == MaliciousTag::FROMTAP
					&& mtag_old.GetSrcNode() == GetNode()->GetId()) { // from my tap
					dst = mtag_old.GetIPDestination();
					NS_LOG_INFO("ADDRESS :       ==== " << dst);
					NS_LOG_INFO("MAC ADDRESS :       ==== " << mtag_old.GetHardwareDestination());
					mtag_new.SetHardwareDestination(mtag_old.GetHardwareDestination());
					m_local = mtag_old.GetIPSource();

					packet->RemoveAllPacketTags ();
					packet->RemoveAllByteTags ();
					packet2 = Create<Packet> (msg, packet->GetSize());

					mtag_new.SetMalStatus(MaliciousTag::FROMPROXY);
					mtag_new.SetIPSource(InetSocketAddress::ConvertFrom(from).GetIpv4().Get());
					mtag_new.SetIPDest(dst.Get());
					mtag_new.SetDestPort(m_port);
					mtag_new.SetSpoof(false);
					packet2->AddPacketTag(mtag_new);

					NS_LOG_INFO ("Echoing packet to " << packet2->GetUid() << " " << dst << " :" << m_port);
					if (m_conn.find(dst) == m_conn.end()) {
						Ptr<Socket> new_socket = Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
						new_socket->Bind(InetSocketAddress(m_local, m_port));
						new_socket->Connect(InetSocketAddress(dst, m_port));
						m_conn[dst] = new_socket;
					}
					m_conn[dst]->Send(packet2);
				} else {
					int tagstatus = mtag_old.GetMalStatus();
					Ipv4Address fromIP = InetSocketAddress::ConvertFrom(from).GetIpv4();
					NS_LOG_INFO("NEED TO SEND BACK TO MY TAP SINCE THE STATUS WAS " << tagstatus << " packet " << packet->GetUid());
					packet->RemoveAllPacketTags ();
					packet->RemoveAllByteTags ();
					packet2 = Create<Packet> (msg, packet->GetSize());
					mtag_new.SetMalStatus(MaliciousTag::FROMPROXY);
					mtag_new.SetIPSource(InetSocketAddress::ConvertFrom(from).GetIpv4().Get());
					mtag_new.SetSpoof(true);
					mtag_new.SetIPDest(m_local.Get());
					mtag_new.Print(std::cerr);
					packet2->AddPacketTag(mtag_new);
					if (m_conn.find(fromIP) == m_conn.end()) {
						NS_LOG_INFO("NOW SEND TO TAP packet " << packet2->GetUid());
						Ptr<Socket> new_socket = Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
						new_socket->Bind();
						new_socket->Connect(InetSocketAddress(m_local, m_port));
						m_conn[fromIP] = new_socket;
					}
					m_conn[fromIP]->Send(packet2);
				}
			}
    }
}

} // Namespace ns3
