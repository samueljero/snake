#ifndef MALICIOUS_TAG_H
#define MALICIOUS_TAG_H

#include "ns3/tag.h"
#include "ns3/ipv4-address.h"
#include "ns3/mac48-address.h"
#include <sys/time.h>

namespace ns3 {


class Node;
class Packet;

class MaliciousTag : public Tag
{
public:
	enum MalStatus {
		FROMTAP = 1, // only malicious has this tag
		FROMPROXY = 2 // from mal proxy
	};
  MaliciousTag ();
	void SetMalStatus(int32_t c);
	int8_t GetMalStatus(void) const;
	void SetSpoof(bool b);
	bool IsSpoof() const;

  Ipv4Address GetIPDestination (void) const;
  uint32_t GetIntDestination(void) const;
	Mac48Address GetHardwareDestination(void) const;
  void SetIPDest (uint32_t addr);
	void SetIPSource(uint32_t addr);
	void SetHardwareDestination(Mac48Address addr) ;
	void SetHardwareSource(Mac48Address addr);
	void SetDestPort(uint16_t port);
	void SetSrcPort(uint16_t port);
	void SetSrcNode(uint16_t node);
	uint16_t GetSrcNode(void) const;
	uint16_t GetSrcPort(void) const;
	uint16_t GetDestPort(void) const;
  
  Ipv4Address GetIPSource (void) const;
  uint32_t GetIntSource(void) const;
	Mac48Address GetHardwareSource(void) const;

	void StartTimer(void);
	void StopTimer(char *str);
	void SetTimer(time_t sec, useconds_t usec);
	time_t GetSec();
	useconds_t GetUSec();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
	int8_t m_malstatus;
	uint16_t m_srcPort;
	uint16_t m_destPort;
  uint32_t m_dest;
  uint32_t m_src;
	Mac48Address m_hwdest;
	Mac48Address m_hwsrc;
	uint16_t m_srcNode;
	struct timeval start;
	/*
  Address m_spec_dst;
	*/
};
} // namespace ns3

#endif /* IPV4_PACKET_INFO_TAG_H */
