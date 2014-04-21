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
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include <unistd.h>
#include <vector>
#include <queue>
#include "message.h"
#include "ns3/stateMachine.h"

namespace ns3 {

class Socket;
class Packet;
enum ControllerType {GREEDY, STATEBASED};
enum MalDirection { TOTAP, FROMTAP };
enum MalAction {NONE, DROP, DUP, DELAY, DIVERT, REPLAY, LIE, BURST, INJECT, WINDOW, RETRY};
#define NUMDELIVERYACTIONS 11
#define DIRECTIONS 2

struct maloptions{
	int divert;
	double delay;
	int duptimes;
	int replay;
	int action;
	bool burst;
};

class lowerLayers {
public:
	Address ethsrc;
	Address ethdest;
	int		ethtype;
	Ipv4Header iph;
	MalDirection dir;
	void *obj;
};

class MalProxy : public Application 
{
public:
  static TypeId GetTypeId (void);

  MalProxy ();
  virtual ~MalProxy ();
	int MalMsg(Message *m, int dir);
	int MaliciousStrategy(Message *m, int dir, maloptions *res) ;
	int MalTCP(Ptr<Packet> packet, lowerLayers ll, maloptions *res);
	void StoreEvent(EventImpl *event);
	void InjectPacket(int type, const char *spec);
	void Burst(int type, int dir);
	void Window(int type, const char *spec);


protected:
  virtual void DoDispose (void);
  virtual int Command(std::string cmd, std::string &output);
  virtual void* Save();
  virtual void Load(void *ptr);
  virtual void Resume();

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ParseStrategy(std::string line);
  void AddStrategy(std::string line);
  void ClearStrategy();
  void ClearStrategyForMsg(int type);
  int CommunicateController(Message *m);
  void DoInjectPacket(Ptr<Packet> p,Ipv4Address src, Ipv4Address dest);
  void RunStateMachines(Message *m, lowerLayers *ll,maloptions *res);
  void ShouldInject();

  bool deliveryActions[MAX_STATES][DIRECTIONS][MSG][NUMDELIVERYACTIONS];
  double deliveryValues[MAX_STATES][DIRECTIONS][MSG][NUMDELIVERYACTIONS];
  char* lyingValues[MAX_STATES][DIRECTIONS][MSG][FIELD];
  char* injectStates [MAX_STATES];
  int 	injectMsg[MAX_STATES];
  char* windowStates [MAX_STATES];
  int windowMsg[MAX_STATES];

  uint64_t num_processed;
  uint16_t m_udp_port;
  uint16_t m_tcp_port;
  Ptr<Socket> m_udp_socket;
  Ptr<Socket> m_tcp_socket;
  Ipv4Address m_local;
  std::map<Ptr<Socket>, Ipv4Address> m_tcp_conn;
  std::map<Ptr<Socket>, Ptr<Socket> > m_pair;
  std::map<int, std::string> m_learned;
  bool m_if_tcp;
  EventImpl *sav_evt;
  bool evt_resume;
  std::map<std::pair<Ipv4Address,uint16_t>, Ptr<Socket> > m_udp_conn;
  std::vector<std::pair<Ptr<Packet>,lowerLayers> > burst[DIRECTIONS][MSG];
  bool burst_sched[DIRECTIONS][MSG];
  ControllerType ctrltype;
  StateMachine sm_server;
  StateMachine sm_client;

  /*Sequence number tracking stuff*/
  class seq_state{
  public:
	  SequenceNumber32 start_seq;
	  int offset;
  };

  class connection{
  public:
	Ipv4Address ip_src;
	Ipv4Address ip_dest;
	int port_src;
	int port_dest;
	std::vector<seq_state> d1;
	std::vector<seq_state> d2;
  };

 std::vector<connection> conn_list;

 connection* FindConnection(Ipv4Address src, Ipv4Address dest, int sport, int dport);
 int FindOffset(std::vector<seq_state> *lst, SequenceNumber32 seq);
 void AddOffset(std::vector<seq_state> *lst, SequenceNumber32 seq, int offset);

 std::queue<Message> m_messageQueue;
 void AddMessage(Message *m);
 void DumpMessages(ostream &os);
};

} // namespace ns3

#endif 

