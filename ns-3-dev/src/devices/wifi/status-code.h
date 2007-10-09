/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 */
#ifndef STATUS_CODE_H
#define STATUS_CODE_H

#include <stdint.h>

namespace ns3 {

class StatusCode {
public:
  StatusCode ();
  void SetSuccess (void);
  void SetFailure (void);

  bool IsSuccess (void) const;
  uint16_t PeekCode (void) const;
  void SetCode (uint16_t code);
  uint32_t GetSize (void) const;
private:
  uint16_t m_code;
};

} // namespace ns3

#endif /* STATUS_CODE_H */
