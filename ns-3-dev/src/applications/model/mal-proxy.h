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

#ifndef MAL_PROXY_SERVER_H
#define MAL_PROXY_SERVER_H

#include <map>
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include "ns3/malicious-tag.h"
#include "message.h"

namespace ns3 {

class Socket;
class Packet;

enum MalAction {NONE, DROP, DUP, DELAY, DIVERT, REPLAY, LIE};
#define NUMDELIVERYACTIONS 6

typedef struct {
  uint8_t *buffer;
  uint32_t size;
  uint32_t cur;
} MessageState;


class MalProxy : public Application 
{
public:
  static TypeId GetTypeId (void);
  MalProxy ();
  virtual ~MalProxy ();
	bool MalMsg(Message *m);
	int MaliciousStrategyUDP(Ptr<Packet> packet, Message *m, uint32_t size,
			int *divert, double *delay, int *duptimes, int *replay) ;

protected:
  virtual void DoDispose (void);
  virtual int Command(std::string cmd);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void HandleAccept (Ptr<Socket> socket, const Address& from);
  void HandleTCPRead (Ptr<Socket> socket);
  void HandleUDPRead (Ptr<Socket> socket);
  void HandleClose(Ptr<Socket> socket);

  void ParseStrategy(std::string line);
  void AddStrategy(std::string line);
  void ClearStrategy();
  void ClearStrategyForMsg(int type);

  void MalSendMsg(Ptr<Socket> socket, Message *m, MaliciousTag mtag, uint32_t size);
  void MalSendUDPMsg(Ptr<Socket> socket, Ptr<Packet> packet, Message *m, MaliciousTag mtag, uint32_t size);

  void SendDelayMsg(Ptr<Socket> socket, Ptr<Packet> packet);
  void PrintPairs();
	int CommunicateController(Message *m);

  bool deliveryActions[MSG][NUMDELIVERYACTIONS]; //6
  double deliveryValues[MSG][NUMDELIVERYACTIONS]; //6
  char* lyingValues[MSG][FIELD];


	uint64_t num_processed;
  uint16_t m_udp_port;
  uint16_t m_tcp_port;
  Ptr<Socket> m_udp_socket;
  Ptr<Socket> m_tcp_socket;
  Ipv4Address m_local;
  std::map<Ptr<Socket>, MessageState> m_state;
  std::map<Ptr<Socket>, Ipv4Address> m_tcp_conn;
  std::map<Ptr<Socket>, Ptr<Socket> > m_pair;
	std::map<int, std::string> m_learned;
  bool m_if_tcp;

  std::map<std::pair<Ipv4Address,uint16_t>, Ptr<Socket> > m_udp_conn;

};

} // namespace ns3

#endif 

