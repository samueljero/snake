#ifndef PROXY_H
#define PROXY_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"

namespace ns3 {

class Address;
class Socket;
class Packet;

/**
 * \ingroup applications 
 * \defgroup proxy Proxy
 *
 * This application was written to complement OnOffApplication, but it
 * is more general so a Proxy name was selected.  Functionally it is
 * important to use in multicast situations, so that reception of the layer-2
 * multicast frames of interest are enabled, but it is also useful for
 * unicast as an example of how you can write something simple to receive
 * packets at the application layer.  Also, if an IP stack generates 
 * ICMP Port Unreachable errors, receiving applications will be needed.
 */

/**
 * \ingroup proxy
 *
 */
class Proxy : public Application 
{
public:
  static TypeId GetTypeId (void);
  Proxy ();

  virtual ~Proxy ();

  /**
   * \return the total bytes received in this sink app
   */
  uint32_t GetTotalRx () const;

  /**
   * \return pointer to listening socket
   */
  Ptr<Socket> GetListeningSocket (void) const;

  /**
   * \return list of pointers to accepted sockets
   */
  std::list<Ptr<Socket> > GetAcceptedSockets (void) const;
 
protected:
  virtual void DoDispose (void);
private:
  // inherited from Application base class.
  virtual void StartApplication (void);    // Called at time specified by Start
  virtual void StopApplication (void);     // Called at time specified by Stop

  void HandleRead (Ptr<Socket>);
  void HandleAccept (Ptr<Socket>, const Address& from);
  void HandlePeerClose (Ptr<Socket>);
  void HandlePeerError (Ptr<Socket>);

  // In the case of TCP, each socket accept returns a new socket, so the 
  // listening socket is stored seperately from the accepted sockets
  Ptr<Socket>     m_socket;       // Listening socket
  std::list<Ptr<Socket> > m_socketList; //the accepted sockets

  Address         m_local;        // Local address to bind to
  uint32_t        m_totalRx;      // Total bytes received
  TypeId          m_tid;          // Protocol TypeId
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

};

} // namespace ns3

#endif /* PROXY_H */

