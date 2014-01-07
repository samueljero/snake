/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Hajime Tazaki
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
 * Authors: Hajime Tazaki <tazaki@sfc.wide.ad.jp>
 */


#include <stdint.h>
#include "ns3/ipv4-address.h"
#include "malicious-tag.h"

namespace ns3 {

MaliciousTag::MaliciousTag ()
{
	m_malstatus = 0;
	m_srcPort = 0;
	m_destPort = 0;
	m_dest = m_src = 0;
	m_srcNode = 9999;
	start.tv_sec = 0;
	start.tv_usec = 0;
}

/*

void MaliciousTag::SetLocalAddress (Address addr) {
  m_spec_dst = addr;
}

Address MaliciousTag::GetLocalAddress (void) const {
  return m_spec_dst;
}
*/
void MaliciousTag::SetMalStatus(int32_t c) {
	m_malstatus = (c & 0x3) | (m_malstatus & 0xc);
}

int8_t MaliciousTag::GetMalStatus(void) const {
	return m_malstatus & 0x3;
}

void MaliciousTag::SetSpoof(bool b) {
	m_malstatus = (m_malstatus & 0x3) | (b << 2);
}

bool MaliciousTag::IsSpoof() const {
	if (m_malstatus >> 2) return true;
	return false;
}

void MaliciousTag::SetIPDest (uint32_t addr) {
  m_dest = addr;
}
void MaliciousTag::SetIPSource (uint32_t addr) {
  m_src = addr;
}

Ipv4Address MaliciousTag::GetIPSource (void) const {
  return Ipv4Address(m_src);
}

Ipv4Address MaliciousTag::GetIPDestination (void) const {
  return Ipv4Address(m_dest);
}


uint32_t MaliciousTag::GetIntSource (void) const {
  return m_src;
}

uint32_t MaliciousTag::GetIntDestination (void) const {
  return m_dest;
}

void MaliciousTag::SetDestPort(uint16_t port) {
	m_destPort = port;
}

void MaliciousTag::SetSrcPort(uint16_t port) {
	m_srcPort = port;
}

void MaliciousTag::SetSrcNode(uint16_t node) {
	m_srcNode = node;
}

uint16_t MaliciousTag::GetSrcNode(void) const {
	return m_srcNode;
}

uint16_t MaliciousTag::GetSrcPort(void) const {
	return m_srcPort;
}

uint16_t MaliciousTag::GetDestPort(void) const {
	return m_destPort;
}

Mac48Address MaliciousTag::GetHardwareSource(void) const {
	return m_hwsrc;
}

Mac48Address MaliciousTag::GetHardwareDestination(void) const {
	return m_hwdest;
}

void MaliciousTag::SetHardwareSource(Mac48Address addr) {
	uint8_t buffer[6];
	addr.CopyTo(buffer);
	m_hwsrc.CopyFrom(buffer);
}

void MaliciousTag::SetHardwareDestination(Mac48Address addr) {
	uint8_t buffer[6];
	addr.CopyTo(buffer);
	m_hwdest.CopyFrom(buffer);
}

void MaliciousTag::StartTimer() {
	return;
	/*
	struct timezone tz;
	gettimeofday(&start, &tz);
	//std::cout << "Start Timer " << start.tv_sec << " " << start.tv_usec << std::endl;
	*/
}


void MaliciousTag::StopTimer(char *str) {
	return;
	/*
	struct timezone tz;
	struct timeval stop;
	gettimeofday(&stop, &tz);
	//std::cout << "Start Timer " << start.tv_sec << " " << start.tv_usec << std::endl;
  //std::cout << "Stop Timer " << stop.tv_sec << " " << stop.tv_usec << std::endl;
  struct timeval diff;
	diff.tv_sec  = stop.tv_sec  - start.tv_sec;
	diff.tv_usec = stop.tv_usec - start.tv_usec;
	if ( diff.tv_usec < 0 )
	{
		diff.tv_usec = diff.tv_usec + 1000000;
		diff.tv_sec--;
	} 
	//std::cout << str << " processed packet in " << diff.tv_sec << " " << diff.tv_usec << std::endl;
	*/
}

void MaliciousTag::SetTimer(time_t sec, useconds_t usec) {
	start.tv_sec = sec;
	start.tv_usec = usec;
}

time_t MaliciousTag::GetSec() {
	return start.tv_sec;
}

useconds_t MaliciousTag::GetUSec() {
	return start.tv_usec;
}

NS_OBJECT_ENSURE_REGISTERED (MaliciousTag);

TypeId
MaliciousTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MaliciousTag")
    .SetParent<Tag> ()
    .AddConstructor<MaliciousTag> ()
  ;
  return tid;
}
TypeId
MaliciousTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
MaliciousTag::GetSerializedSize (void) const
{ 
  //return m_dest.GetSerializedSize() + m_spec_dst.GetSerializedSize();
	return (sizeof(uint32_t) + sizeof(int32_t) + 1 + 6 + 6 + 1 + 2 + 2 + 2 + 16);
}
void 
MaliciousTag::Serialize (TagBuffer i) const
{ 
	i.WriteU8(m_malstatus);
	i.WriteU32(m_dest);
	i.WriteU32(m_src);
	i.WriteU16(m_destPort);
	i.WriteU16(m_srcPort);
	i.WriteU16(m_srcNode);
  uint8_t buf[6];
  m_hwdest.CopyTo(buf);
  i.Write (buf, 6);
  m_hwsrc.CopyTo(buf);
  i.Write (buf, 6);
	i.WriteU64(start.tv_sec);
	i.WriteU64(start.tv_usec);
	
}

void 
MaliciousTag::Deserialize (TagBuffer i)
{ 
	m_malstatus = i.ReadU8();
	m_dest = i.ReadU32();
	m_src = i.ReadU32();
	m_destPort = i.ReadU16();
	m_srcPort = i.ReadU16();
	m_srcNode = i.ReadU16();
  uint8_t buf[6];
  i.Read (buf, 6);
	m_hwdest.CopyFrom(buf);
  i.Read (buf, 6);
	m_hwsrc.CopyFrom(buf);
	/*
	start.tv_sec = i.ReadU64();
	start.tv_usec = i.ReadU64();
	*/
	//std::cout << "timer deserialize " << start.tv_sec << " " << start.tv_usec << std::endl;
}

void
MaliciousTag::Print (std::ostream &os) const
{
	return;
/*
  os << "MALICIOUS INFO [Src: " << Ipv4Address(m_src) << ", port: " << m_srcPort << " (node " << m_srcNode << "), Dst: " << Ipv4Address(m_dest) << ", port: " << m_destPort << ", MalStatus: ";
  if (GetMalStatus() == MaliciousTag::FROMPROXY) {
	os << "FROMPROXY";
  } else if (GetMalStatus() == MaliciousTag::FROMTAP) {
	os << "FROMTAP";
  } else {
	os << "NOT DEFINED " << (int)GetMalStatus();
  }
	if (IsSpoof()) os << " SPOOF " ;
  os << "]\n";
	*/
	return;
}
} // namespace ns3

