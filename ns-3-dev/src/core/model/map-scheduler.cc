/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 INRIA
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
 * The idea to use a std c++ map came from GTNetS
 */

#include "map-scheduler.h"
#include "event-impl.h"
#include "assert.h"
#include "log.h"
#include <string>

NS_LOG_COMPONENT_DEFINE ("MapScheduler");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MapScheduler);

TypeId
MapScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MapScheduler")
    .SetParent<Scheduler> ()
    .AddConstructor<MapScheduler> ()
  ;
  return tid;
}

MapScheduler::MapScheduler ()
{
}
MapScheduler::~MapScheduler ()
{
}

void* MapScheduler::Save(void *ptr, int *eventCount) {
  NS_LOG_FUNCTION (this);
	std::cout << "save map event" << std::endl;
	//std::cout << "size : " << m_list.size() << std::endl;
  int evCnt = 0;
	EventMap *m_saved = new EventMap();
	while (m_list.size() > 1) {
		EventMapI i = m_list.begin ();
		Event ev;
		ev.impl = i->second->clone();
		ev.key = i->first;
		if (ev.impl != NULL) {
      m_saved->insert(std::make_pair (ev.key, ev.impl));
      evCnt++;
    }
		m_list.erase (i);
	}
  *eventCount = evCnt;
	//std::cout << "remaining size : " << m_list.size() << std::endl;
	return m_saved;
}

int MapScheduler::Load(void *ptr) {
	EventMap* m_saved = static_cast<EventMap*>(ptr);
  int count = 0;
	std::cout << "saved size: " << m_saved->size() << std::endl;
	// current m_list has events that are created after the snapshot has taken
	// should remove 
	while (m_list.size() > 1) {
		EventMapI i = m_list.begin ();
		m_list.erase (i);
	}
	for(EventMapI si = m_saved->begin(); si != m_saved->end(); si++) {
		Event ev;
		if (si->second) {
			ev.impl = si->second->clone();
			ev.key = si->first;
			m_list.insert(std::make_pair (ev.key, ev.impl));
      count++;
		}
	}
	//m_list.insert(m_saved->begin(), m_saved->end());
	return 0;
}

void
MapScheduler::Insert (const Event &ev)
{
  NS_LOG_FUNCTION (this << ev.impl << ev.key.m_ts << ev.key.m_uid);
  std::pair<EventMapI,bool> result;
  result = m_list.insert (std::make_pair (ev.key, ev.impl));
	//std::cout << "size: " << m_list.size() << std::endl;
  NS_ASSERT (result.second);
}

bool
MapScheduler::IsEmpty (void) const
{
  return m_list.empty ();
}

Scheduler::Event
MapScheduler::PeekNext (void) const
{
  NS_LOG_FUNCTION (this);
  EventMapCI i = m_list.begin ();
  NS_ASSERT (i != m_list.end ());

  Event ev;
  ev.impl = i->second;
  ev.key = i->first;
  NS_LOG_DEBUG (this << ev.impl << ev.key.m_ts << ev.key.m_uid);
  return ev;
}
Scheduler::Event
MapScheduler::RemoveNext (void)
{
  NS_LOG_FUNCTION (this);
  EventMapI i = m_list.begin ();
  NS_ASSERT (i != m_list.end ());
  Event ev;
  ev.impl = i->second;
  ev.key = i->first;
  m_list.erase (i);
	//std::cout << "erase - size : " << m_list.size() << std::endl;
  NS_LOG_DEBUG (this << ev.impl << ev.key.m_ts << ev.key.m_uid);
  return ev;
}

void
MapScheduler::Remove (const Event &ev)
{
  NS_LOG_FUNCTION (this << ev.impl << ev.key.m_ts << ev.key.m_uid);
  EventMapI i = m_list.find (ev.key);
  NS_ASSERT (i->second == ev.impl);
  m_list.erase (i);
}

} // namespace ns3
