/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 University of Washington
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

#include "tap-bridge.h"
#include "tap-encode-decode.h"

#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/ethernet-header.h"
#include "ns3/ethernet-trailer.h"
#include "ns3/llc-snap-header.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/enum.h"
#include "ns3/ipv4.h"
#include "ns3/arp-header.h"
#include "ns3/simulator.h"
#include "ns3/realtime-simulator-impl.h"
#include "ns3/unix-fd-reader.h"
#include "ns3/uinteger.h"
#include "ns3/malicious-tag.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <limits>
#include <stdlib.h>

//
// Sometimes having a tap-creator is actually more trouble than solution.  In 
// these cases you can uncomment the define of TAP_CREATOR below and the 
// simulation will just use a device you have preconfigured.  This is useful
// if you are running in an environment where you have got to run as root,
// such as ORBIT or CORE.
//


// #define NO_CREATOR

#ifdef NO_CREATOR
#include <fcntl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#endif

#include <iostream>
#include <fstream>

NS_LOG_COMPONENT_DEFINE ("TapBridge");

namespace ns3 {

FdReader::Data TapBridgeFdReader::DoRead (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  uint32_t bufferSize = 65536;
  uint8_t *buf = (uint8_t *)malloc (bufferSize);
  NS_ABORT_MSG_IF (buf == 0, "malloc() failed");

  NS_LOG_LOGIC ("Calling read on tap device fd " << m_fd);
  ssize_t len = read (m_fd, buf, bufferSize);
  if (len <= 0)
    {
      NS_LOG_INFO ("TapBridgeFdReader::DoRead(): done");
      free (buf);
      buf = 0;
      len = 0;
    }

  return FdReader::Data (buf, len);
}

#define TAP_MAGIC 95549

NS_OBJECT_ENSURE_REGISTERED (TapBridge);

TypeId
TapBridge::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TapBridge")
    .SetParent<NetDevice> ()
    .AddConstructor<TapBridge> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TapBridge::SetMtu,
                                         &TapBridge::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("DeviceName", 
                   "The name of the tap device to create.",
                   StringValue (""),
                   MakeStringAccessor (&TapBridge::m_tapDeviceName),
                   MakeStringChecker ())
    .AddAttribute ("Gateway", 
                   "The IP address of the default gateway to assign to the host machine, when in ConfigureLocal mode.",
                   Ipv4AddressValue ("255.255.255.255"),
                   MakeIpv4AddressAccessor (&TapBridge::m_tapGateway),
                   MakeIpv4AddressChecker ())
    .AddAttribute ("IpAddress", 
                   "The IP address to assign to the tap device, when in ConfigureLocal mode.  "
                   "This address will override the discovered IP address of the simulated device.",
                   Ipv4AddressValue ("255.255.255.255"),
                   MakeIpv4AddressAccessor (&TapBridge::m_tapIp),
                   MakeIpv4AddressChecker ())
    .AddAttribute ("MacAddress", 
                   "The MAC address to assign to the tap device, when in ConfigureLocal mode.  "
                   "This address will override the discovered MAC address of the simulated device.",
                   Mac48AddressValue (Mac48Address ("ff:ff:ff:ff:ff:ff")),
                   MakeMac48AddressAccessor (&TapBridge::m_tapMac),
                   MakeMac48AddressChecker ())
    .AddAttribute ("Netmask", 
                   "The network mask to assign to the tap device, when in ConfigureLocal mode.  "
                   "This address will override the discovered MAC address of the simulated device.",
                   Ipv4MaskValue ("255.255.255.255"),
                   MakeIpv4MaskAccessor (&TapBridge::m_tapNetmask),
                   MakeIpv4MaskChecker ())
    .AddAttribute ("Start", 
                   "The simulation time at which to spin up the tap device read thread.",
                   TimeValue (Seconds (0.)),
                   MakeTimeAccessor (&TapBridge::m_tStart),
                   MakeTimeChecker ())
    .AddAttribute ("Stop", 
                   "The simulation time at which to tear down the tap device read thread.",
                   TimeValue (Seconds (0.)),
                   MakeTimeAccessor (&TapBridge::m_tStop),
                   MakeTimeChecker ())
    .AddAttribute ("Mode", 
                   "The operating and configuration mode to use.",
                   EnumValue (USE_LOCAL),
                   MakeEnumAccessor (&TapBridge::SetMode),
                   MakeEnumChecker (CONFIGURE_LOCAL, "ConfigureLocal",
                                    USE_LOCAL, "UseLocal",
                                    USE_BRIDGE, "UseBridge"))
  ;
  return tid;
}

TapBridge::TapBridge ()
  : m_node (0),
    m_ifIndex (0),
    m_sock (-1),
    m_startEvent (),
    m_stopEvent (),
    m_fdReader (0),
    m_ns3AddressRewritten (false)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_packetBuffer = new uint8_t[65536];
  Start (m_tStart);
}

TapBridge::~TapBridge()
{
  NS_LOG_FUNCTION_NOARGS ();

  StopTapDevice ();

  delete [] m_packetBuffer;
  m_packetBuffer = 0;

  m_bridgedDevice = 0;
}

void
TapBridge::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();
  NetDevice::DoDispose ();
}

void
TapBridge::Start (Time tStart)
{
  NS_LOG_FUNCTION (tStart);

  //
  // Cancel any pending start event and schedule a new one at some relative time in the future.
  //
  Simulator::Cancel (m_startEvent);
  m_startEvent = Simulator::Schedule (tStart, &TapBridge::StartTapDevice, this);
}

void
TapBridge::Stop (Time tStop)
{
  NS_LOG_FUNCTION (tStop);
  //
  // Cancel any pending stop event and schedule a new one at some relative time in the future.
  //
  Simulator::Cancel (m_stopEvent);
  m_startEvent = Simulator::Schedule (tStop, &TapBridge::StopTapDevice, this);
}

void
TapBridge::StartTapDevice (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ABORT_MSG_IF (m_sock != -1, "TapBridge::StartTapDevice(): Tap is already started");

  //
  // We're going to need a pointer to the realtime simulator implementation.
  // It's important to remember that access to that implementation may happen 
  // in a completely different thread than the simulator is running in (we're 
  // going to spin up that thread below).  We are talking about multiple threads
  // here, so it is very, very dangerous to do any kind of reference couning on
  // a shared object that is unaware of what is happening.  What we are going to 
  // do to address that is to get a reference to the realtime simulator here 
  // where we are running in the context of a running simulator scheduler --
  // recall we did a Simulator::Schedule of this method above.  We get the
  // simulator implementation pointer in a single-threaded way and save the
  // underlying raw pointer for use by the (other) read thread.  We must not
  // free this pointer or we may delete the simulator out from under us an 
  // everyone else.  We assume that the simulator implementation cannot be 
  // replaced while the tap bridge is running and so will remain valid through
  // the time during which the read thread is running.
  //
  Ptr<RealtimeSimulatorImpl> impl = DynamicCast<RealtimeSimulatorImpl> (Simulator::GetImplementation ());
  m_rtImpl = GetPointer (impl);

  //
  // A similar story exists for the node ID.  We can't just naively do a
  // GetNode ()->GetId () since GetNode is going to give us a Ptr<Node> which
  // is reference counted.  We need to stash away the node ID for use in the
  // read thread.
  //
  m_nodeId = GetNode ()->GetId ();

  //
  // Spin up the tap bridge and start receiving packets.
  //
  NS_LOG_LOGIC ("Creating tap device");

  //
  // Call out to a separate process running as suid root in order to get the 
  // tap device allocated and set up.  We do this to avoid having the entire 
  // simulation running as root.  If this method returns, we'll have a socket
  // waiting for us in m_sock that we can use to talk to the newly created 
  // tap device.
  //
  CreateTap ();

  //
  // Now spin up a read thread to read packets from the tap device.
  //
  NS_ABORT_MSG_IF (m_fdReader != 0,"TapBridge::StartTapDevice(): Receive thread is already running");
  NS_LOG_LOGIC ("Spinning up read thread");

  m_fdReader = Create<TapBridgeFdReader> ();
  m_fdReader->Start (m_sock, MakeCallback (&TapBridge::ReadCallback, this));
}

void
TapBridge::StopTapDevice (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_fdReader != 0)
    {
      m_fdReader->Stop ();
      m_fdReader = 0;
    }

  if (m_sock != -1)
    {
      close (m_sock);
      m_sock = -1;
    }
}

void
TapBridge::CreateTap (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // 
  // The TapBridge has three distinct operating modes.  At this point, the
  // differences revolve around who is responsible for creating and configuring
  // the underlying network tap that we use.  In ConfigureLocal mode, the 
  // TapBridge has the responsibility for creating and configuring the TAP.
  //
  // In UseBridge or UseLocal modes, the user will provide us a configuration
  // and we have to adapt to it.  For example, in UseLocal mode, the user will
  // be configuring a tap device outside the scope of the ns-3 simulation and
  // will be expecting us to work with it.  The user will do something like:
  //
  //   sudo tunctl -t tap0
  //   sudo ifconfig tap0 hw ether 00:00:00:00:00:01
  //   sudo ifconfig tap0 10.1.1.1 netmask 255.255.255.0 up
  //
  // The user will then set the "Mode" Attribute of the TapBridge to "UseLocal"
  // and the "DeviceName" Attribute to "tap0" in this case.
  //
  // In ConfigureLocal mode, the user is asking the TapBridge to do the 
  // configuration and create a TAP with the provided "DeviceName" with which 
  // the user can later do what she wants.  We need to extract values for the
  // MAC address, IP address, net mask, etc, from the simualtion itself and 
  // use them to initialize corresponding values on the created tap device.
  //
  // In UseBridge mode, the user is asking us to use an existing tap device
  // has been included in an OS bridge.  She is asking us to take the simulated
  // net device and logically add it to the existing bridge.  We expect that
  // the user has done something like:
  //
  //   sudo brctl addbr mybridge
  //   sudo tunctl -t mytap
  //   sudo ifconfig mytap hw ether 00:00:00:00:00:01
  //   sudo ifconfig mytap 0.0.0.0 up
  //   sudo brctl addif mybridge mytap
  //   sudo brctl addif mybridge ...
  //   sudo ifconfig mybridge 10.1.1.1 netmask 255.255.255.0 up
  //
  // The bottom line at this point is that we want to either create or use a 
  // tap device on the host based on the verb part "Use" or "Configure" of the 
  // operating mode.  Unfortunately for us you have to have root privileges to
  // do either.  Instead of running the entire simulation as root, we decided 
  // to make a small program who's whole reason for being is to run as suid 
  // root and do what it takes to create the tap.  We're just going to pass 
  // off the configuration information to that program and let it deal with
  // the situation.
  //
  // We're going to fork and exec that program soon, but first we need to have 
  // a socket to talk to it with.  So we create a local interprocess (Unix) 
  // socket for that purpose.
  //
  int sock = socket (PF_UNIX, SOCK_DGRAM, 0);
  NS_ABORT_MSG_IF (sock == -1, "TapBridge::CreateTap(): Unix socket creation error, errno = " << strerror (errno));

  //
  // Bind to that socket and let the kernel allocate an endpoint
  //
  struct sockaddr_un un;
  memset (&un, 0, sizeof (un));
  un.sun_family = AF_UNIX;
  int status = bind (sock, (struct sockaddr*)&un, sizeof (sa_family_t));
  NS_ABORT_MSG_IF (status == -1, "TapBridge::CreateTap(): Could not bind(): errno = " << strerror (errno));
  NS_LOG_INFO ("Created Unix socket");
  NS_LOG_INFO ("sun_family = " << un.sun_family);
  NS_LOG_INFO ("sun_path = " << un.sun_path);

  //
  // We have a socket here, but we want to get it there -- to the program we're
  // going to exec.  What we'll do is to do a getsockname and then encode the
  // resulting address information as a string, and then send the string to the
  // program as an argument.  So we need to get the sock name.
  //
  socklen_t len = sizeof (un);
  status = getsockname (sock, (struct sockaddr*)&un, &len);
  NS_ABORT_MSG_IF (status == -1, "TapBridge::CreateTap(): Could not getsockname(): errno = " << strerror (errno));

  //
  // Now encode that socket name (family and path) as a string of hex digits
  //
  std::string path = TapBufferToString ((uint8_t *)&un, len);
  NS_LOG_INFO ("Encoded Unix socket as \"" << path << "\"");

  //
  // Tom Goff reports the possiblility of a deadlock when trying to acquire the
  // python GIL here.  He says that this might be due to trying to access Python
  // objects after fork() without calling PyOS_AfterFork() to properly reset 
  // Python state (including the GIL).  Originally these next three lines were
  // done after the fork, but were moved here to work around the deadlock.
  //
  Ptr<NetDevice> nd = GetBridgedNetDevice ();
  Ptr<Node> n = nd->GetNode ();
  Ptr<Ipv4> ipv4 = n->GetObject<Ipv4> ();
	n->SetTapDevice(this);

  //
  // Fork and exec the process to create our socket.  If we're us (the parent)
  // we wait for the child (the creator) to complete and read the socket it 
  // created and passed back using the ancillary data mechanism.
  //
  pid_t pid = ::fork ();
  if (pid == 0)
    {
      NS_LOG_DEBUG ("Child process");

      //
      // build a command line argument from the encoded endpoint string that 
      // the socket creation process will use to figure out how to respond to
      // the (now) parent process.  We're going to have to give this program
      // quite a bit of information.
      //
      // -d<device-name> The name of the tap device we want to create;
      // -g<gateway-address> The IP address to use as the default gateway;
      // -i<IP-address> The IP address to assign to the new tap device;
      // -m<MAC-address> The MAC-48 address to assign to the new tap device;
      // -n<network-mask> The network mask to assign to the new tap device;
      // -o<operating mode> The operating mode of the bridge (1=ConfigureLocal, 2=UseLocal, 3=UseBridge)
      // -p<path> the path to the unix socket described above.
      //
      // Example tap-creator -dnewdev -g1.2.3.2 -i1.2.3.1 -m08:00:2e:00:01:23 -n255.255.255.0 -o1 -pblah
      //
      // We want to get as much of this stuff automagically as possible.
      //
      // For CONFIGURE_LOCAL mode only:
      // <IP-address> is the IP address we are going to set in the newly 
      // created Tap device on the Linux host.  At the point in the simulation
      // where devices are coming up, we should have all of our IP addresses
      // assigned.  That means that we can find the IP address to assign to 
      // the new Tap device from the IP address associated with the bridged
      // net device.
      //

      bool wantIp = (m_mode == CONFIGURE_LOCAL);

      if (wantIp
          && (ipv4 == 0)
          && m_tapIp.IsBroadcast ()
          && m_tapNetmask.IsEqual (Ipv4Mask::GetOnes ()))
        {
          NS_FATAL_ERROR ("TapBridge::CreateTap(): Tap device IP configuration requested but neither IP address nor IP netmask is provided");
        }

      // Some stub values to make tap-creator happy
      Ipv4Address ipv4Address ("255.255.255.255");
      Ipv4Mask ipv4Mask ("255.255.255.255");

      if (ipv4 != 0)
        {
          uint32_t index = ipv4->GetInterfaceForDevice (nd);
          if (ipv4->GetNAddresses (index) > 1)
            {
              NS_LOG_WARN ("Underlying bridged NetDevice has multiple IP addresses; using first one.");
            }
          ipv4Address = ipv4->GetAddress (index, 0).GetLocal ();

          //
          // The net mask is sitting right there next to the ipv4 address.
          //
          ipv4Mask = ipv4->GetAddress (index, 0).GetMask ();
        }

      //
      // The MAC address should also already be assigned and waiting for us in
      // the bridged net device.
      //
      Address address = nd->GetAddress ();
      Mac48Address mac48Address = Mac48Address::ConvertFrom (address);

      //
      // The device-name is something we may want the system to make up in 
      // every case.  We also rely on it being configured via an Attribute 
      // through the helper.  By default, it is set to the empty string 
      // which tells the system to make up a device name such as "tap123".
      //
      std::ostringstream ossDeviceName;
      ossDeviceName << "-d" << m_tapDeviceName;

      //
      // The gateway-address is something we can't derive, so we rely on it
      // being configured via an Attribute through the helper.
      //
      std::ostringstream ossGateway;
      ossGateway << "-g" << m_tapGateway;

      //
      // For flexibility, we do allow a client to override any of the values
      // above via attributes, so only use our found values if the Attribute
      // is not equal to its default value (empty string or broadcast address). 
      //
      std::ostringstream ossIp;
      if (m_tapIp.IsBroadcast ())
        {
          ossIp << "-i" << ipv4Address;
        }
      else
        {
          ossIp << "-i" << m_tapIp;
        }

      std::ostringstream ossMac;
      if (m_tapMac.IsBroadcast ())
        {
          ossMac << "-m" << mac48Address;
        }
      else
        {
          ossMac << "-m" << m_tapMac;
        }

      std::ostringstream ossNetmask;
      if (m_tapNetmask.IsEqual (Ipv4Mask::GetOnes ()))
        {
          ossNetmask << "-n" << ipv4Mask;
        }
      else
        {
          ossNetmask << "-n" << m_tapNetmask;
        }

      std::ostringstream ossMode;
      ossMode << "-o";
      if (m_mode == CONFIGURE_LOCAL)
        {
          ossMode << "1";
        }
      else if (m_mode == USE_LOCAL)
        {
          ossMode << "2";
        }
      else
        {
          ossMode << "3";
        }

      std::ostringstream ossPath;
      ossPath << "-p" << path;
      //
      // Execute the socket creation process image.
      //
      status = ::execlp ("tap-creator", 
                         "tap-creator",                       // argv[0] (filename)
                         ossDeviceName.str ().c_str (),       // argv[1] (-d<device name>)
                         ossGateway.str ().c_str (),          // argv[2] (-g<gateway>)
                         ossIp.str ().c_str (),               // argv[3] (-i<IP address>)
                         ossMac.str ().c_str (),              // argv[4] (-m<MAC address>)
                         ossNetmask.str ().c_str (),          // argv[5] (-n<net mask>)
                         ossMode.str ().c_str (),             // argv[6] (-o<operating mode>)
                         ossPath.str ().c_str (),             // argv[7] (-p<path>)
                         (char *)NULL);

      //
      // If the execlp successfully completes, it never returns.  If it returns it failed or the OS is
      // broken.  In either case, we bail.
      //
      NS_FATAL_ERROR ("TapBridge::CreateTap(): Back from execlp(), errno = " << ::strerror (errno));
    }
  else
    {
      NS_LOG_DEBUG ("Parent process");
      //
      // We're the process running the emu net device.  We need to wait for the
      // socket creator process to finish its job.
      //
      int st;
      pid_t waited = waitpid (pid, &st, 0);
      NS_ABORT_MSG_IF (waited == -1, "TapBridge::CreateTap(): waitpid() fails, errno = " << strerror (errno));
      NS_ASSERT_MSG (pid == waited, "TapBridge::CreateTap(): pid mismatch");

      //
      // Check to see if the socket creator exited normally and then take a 
      // look at the exit code.  If it bailed, so should we.  If it didn't
      // even exit normally, we bail too.
      //
      if (WIFEXITED (st))
        {
          int exitStatus = WEXITSTATUS (st);
          NS_ABORT_MSG_IF (exitStatus != 0, 
                           "TapBridge::CreateTap(): socket creator exited normally with status " << exitStatus);
        }
      else 
        {
          NS_FATAL_ERROR ("TapBridge::CreateTap(): socket creator exited abnormally");
        }

      //
      // At this point, the socket creator has run successfully and should 
      // have created our tap device, initialized it with the information we
      // passed and sent it back to the socket address we provided.  A socket
      // (fd) we can use to talk to this tap device should be waiting on the 
      // Unix socket we set up to receive information back from the creator
      // program.  We've got to do a bunch of grunt work to get at it, though.
      //
      // The struct iovec below is part of a scatter-gather list.  It describes a
      // buffer.  In this case, it describes a buffer (an integer) that will
      // get the data that comes back from the socket creator process.  It will
      // be a magic number that we use as a consistency/sanity check.
      // 
      struct iovec iov;
      uint32_t magic;
      iov.iov_base = &magic;
      iov.iov_len = sizeof(magic);

      //
      // The CMSG macros you'll see below are used to create and access control 
      // messages (which is another name for ancillary data).  The ancillary 
      // data is made up of pairs of struct cmsghdr structures and associated
      // data arrays.
      //
      // First, we're going to allocate a buffer on the stack to receive our 
      // data array (that contains the socket).  Sometimes you'll see this called
      // an "ancillary element" but the msghdr uses the control message termimology
      // so we call it "control."
      //
      size_t msg_size = sizeof(int);
      char control[CMSG_SPACE (msg_size)];

      //
      // There is a msghdr that is used to minimize the number of parameters
      // passed to recvmsg (which we will use to receive our ancillary data).
      // This structure uses terminology corresponding to control messages, so
      // you'll see msg_control, which is the pointer to the ancillary data and 
      // controllen which is the size of the ancillary data array.
      //
      // So, initialize the message header that describes the ancillary/control
      // data we expect to receive and point it to buffer.
      //
      struct msghdr msg;
      msg.msg_name = 0;
      msg.msg_namelen = 0;
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_control = control;
      msg.msg_controllen = sizeof (control);
      msg.msg_flags = 0;

      //
      // Now we can actually receive the interesting bits from the tap
      // creator process.  Lots of pain to get four bytes.
      //
      ssize_t bytesRead = recvmsg (sock, &msg, 0);
      NS_ABORT_MSG_IF (bytesRead != sizeof(int), "TapBridge::CreateTap(): Wrong byte count from socket creator");

      //
      // There may be a number of message headers/ancillary data arrays coming in.
      // Let's look for the one with a type SCM_RIGHTS which indicates it's the
      // one we're interested in.
      //
      struct cmsghdr *cmsg;
      for (cmsg = CMSG_FIRSTHDR (&msg); cmsg != NULL; cmsg = CMSG_NXTHDR (&msg, cmsg))
        {
          if (cmsg->cmsg_level == SOL_SOCKET &&
              cmsg->cmsg_type == SCM_RIGHTS)
            {
              //
              // This is the type of message we want.  Check to see if the magic 
              // number is correct and then pull out the socket we care about if
              // it matches
              //
              if (magic == TAP_MAGIC)
                {
                  NS_LOG_INFO ("Got SCM_RIGHTS with correct magic " << magic);
                  int *rawSocket = (int*)CMSG_DATA (cmsg);
                  NS_LOG_INFO ("Got the socket from the socket creator = " << *rawSocket);
                  m_sock = *rawSocket;
                  return;
                }
              else
                {
                  NS_LOG_INFO ("Got SCM_RIGHTS, but with bad magic " << magic);
                }
            }
        }
      NS_FATAL_ERROR ("Did not get the raw socket from the socket creator");
    }
}

void
TapBridge::ReadCallback (uint8_t *buf, ssize_t len)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT_MSG (buf != 0, "invalid buf argument");
  NS_ASSERT_MSG (len > 0, "invalid len argument");

  //
  // It's important to remember that we're in a completely different thread
  // than the simulator is running in.  We need to synchronize with that
  // other thread to get the packet up into ns-3.  What we will need to do
  // is to schedule a method to deal with the packet using the multithreaded
  // simulator we are most certainly running.  However, I just said it -- we
  // are talking about two threads here, so it is very, very dangerous to do
  // any kind of reference counting on a shared object.  Just don't do it.
  // So what we're going to do is pass the buffer allocated on the heap
  // into the ns-3 context thread where it will create the packet.
  //

  NS_LOG_INFO ("TapBridge::ReadCallback(): Received packet on node " << m_nodeId);
  NS_LOG_INFO ("TapBridge::ReadCallback(): Scheduling handler");
  NS_ASSERT_MSG (m_rtImpl, "TapBridge::ReadCallback(): Realtime simulator implementation pointer not set");
  m_rtImpl->ScheduleRealtimeNowWithContext (m_nodeId, MakeSpecialEvent (&TapBridge::ForwardToBridgedDevice, this, buf, len));
  free(buf);
  buf = 0;
  //ForwardToBridgedDevice(buf, len);
}

void TapBridge::PacketSend(Ptr<Packet> packet, Address src, Address dst, uint16_t type) {
	if (m_mode == USE_LOCAL) {
		if (m_ns3AddressRewritten == false) {
			Mac48Address learnedMac = Mac48Address::ConvertFrom (src);
			m_bridgedDevice->SetAddress (Mac48Address::ConvertFrom (learnedMac));
			m_ns3AddressRewritten = true;
		}
		m_bridgedDevice->Send (packet, dst, type);
		return;
	}
	if (m_mode == USE_BRIDGE)
		m_bridgedDevice->SendFrom (packet, src, dst, type);
	else
		m_bridgedDevice->Send (packet, dst, type);
}

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 University of Washington
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

#ifndef TAP_BRIDGE_H
#define TAP_BRIDGE_H

#include <string.h>
#include "ns3/address.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/unix-fd-reader.h"
#include "ns3/realtime-simulator-impl.h"

namespace ns3 {

class TapBridgeFdReader : public FdReader
{
private:
  FdReader::Data DoRead (void);
};

class Node;

/**
 * \ingroup tap-bridge
 * 
 * \brief A bridge to make it appear that a real host process is connected to 
 * an ns-3 net device.
 *
 * The Tap Bridge lives in a kind of a gray world somewhere between a
 * Linux host and an ns-3 bridge device.  From the Linux perspective,
 * this code appears as the user mode handler for a Tap net device.  That
 * is, when the Linux host writes to a /dev/tap device (that is either
 * manually or automatically created depending on basic operating mode 
 * -- more on this later), the write is redirected into the TapBridge that
 * lives in the ns-3 world; and from this perspective, becomes a read.
 * In other words, a Linux process writes a packet to a tap device and
 * this packet is redirected to an ns-3 process where it is received by
 * the TapBridge as a result of a read operation there.  The TapBridge
 * then sends the packet to the ns-3 net device to which it is bridged.
 * In the other direction, a packet received by an ns-3 net device is
 * bridged to the TapBridge (it appears via a callback from that net
 * device.  The TapBridge then takes that packet and writes it back to
 * the host using the Linux TAP mechanism.  This write to the device will
 * then appear to the Linux host as if a packet has arrived on its
 * device.
 * 
 * The upshot is that the Tap Bridge appears to bridge a tap device on a
 * Linux host in the "real world" to an ns-3 net device in the simulation
 * and make is appear that a ns-3 net device is actually installed in the
 * Linux host.  In order to do this on the ns-3 side, we need a "ghost
 * node" in the simulation to hold the bridged ns-3 net device and the
 * TapBridge.  This node should not actually do anything else in the
 * simulation since its job is simply to make the net device appear in
 * Linux.  This is not just arbitrary policy, it is because:
 *
 * - Bits sent to the Tap Bridge from higher layers in the ghost node (using
 *   the TapBridge Send() method) are completely ignored.  The Tap Bridge is 
 *   not, itself, connected to any network, neither in Linux nor in ns-3;
 * - The bridged ns-3 net device is has had its receive callback disconnected
 *   from the ns-3 node and reconnected to the Tap Bridge.  All data received 
 *   by a bridged device will be sent to the Linux host and will not be 
 *   received by the node.  From the perspective of the ghost node, you can 
 *   send over this device but you cannot ever receive.
 *
 * Of course, if you understand all of the issues you can take control of
 * your own destiny and do whatever you want -- we do not actively
 * prevent you from using the ghost node for anything you decide.  You
 * will be able to perform typical ns-3 operations on the ghost node if
 * you so desire.  The internet stack, for example, must be there and
 * functional on that node in order to participate in IP address
 * assignment and global routing.  However, as mentioned above,
 * interfaces talking any Tap Bridge or associated bridged net devices
 * will not work completely.  If you understand exactly what you are
 * doing, you can set up other interfaces and devices on the ghost node
 * and use them; or take advantage of the operational send side of the
 * bridged devices to create traffic generators.  We generally recommend
 * that you treat this node as a ghost of the Linux host and leave it to
 * itself, though.
 */
class TapBridge : public NetDevice
{
public:
  static TypeId GetTypeId (void);

  /**
   * Enumeration of the operating modes supported in the class.
   *
   */
  enum Mode {
    ILLEGAL,         /**< mode not set */
    CONFIGURE_LOCAL, /**< ns-3 creates and configures tap device */
    USE_LOCAL,       /**< ns-3 uses a pre-created tap, without configuring it */
    USE_BRIDGE, /**< ns-3 uses a pre-created tap, and bridges to a bridging net device */
  };

  TapBridge ();
  virtual ~TapBridge ();

  /**
   * \brief Get the bridged net device.
   *
   * The bridged net device is the ns-3 device to which this bridge is connected,
   *
   * \returns the bridged net device.
   */
  Ptr<NetDevice> GetBridgedNetDevice (void);

  /**
   * \brief Set the ns-3 net device to bridge.
   *
   * This method tells the bridge which ns-3 net device it should use to connect
   * the simulation side of the bridge.
   *
   * \param bridgedDevice device to set
   *
   * \attention The ns-3 net device that is being set as the device must have an
   * an IP address assigned to it before the simulation is run.  This address 
   * will be used to set the hardware address of the host Linux device.
   */
  void SetBridgedNetDevice (Ptr<NetDevice> bridgedDevice);

  /**
   * \brief Set a start time for the device.
   *
   * The tap bridge consumes a non-trivial amount of time to start.  It starts
   * up in the context of a scheduled event to ensure that all configuration
   * has been completed before extracting the configuration (IP addresses, etc.)
   * In order to allow a more reasonable start-up sequence than a thundering 
   * herd of devices, the time at which each device starts is also configurable
   * bot via the Attribute system and via this call.
   *
   * \param tStart the start time
   */
  void Start (Time tStart);

  /**
   * Set a stop time for the device.
   *
   * @param tStop the stop time
   *
   * \see TapBridge::Start
   */
  void Stop (Time tStop);

  /**
   * Set the operating mode of this device.
   *
   * \param mode The operating mode of this device.
   */
  void SetMode (TapBridge::Mode mode);

  /**
   * Get the operating mode of this device.
   *
   * \returns The operating mode of this device.
   */
  TapBridge::Mode  GetMode (void);

  //
  // The following methods are inherited from NetDevice base class and are
  // documented there.
  //
  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual void SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom () const;
  virtual Address GetMulticast (Ipv6Address addr) const;
	virtual void PacketSend(Ptr<Packet> packet, Address src, Address dst, uint16_t type);
	virtual int MaliciousProcess (Ptr<Packet> p, Address src, Address dest, uint16_t type, bool direction);

protected:
  /**
   * \internal
   *
   * Call out to a separate process running as suid root in order to get our
   * tap device created.  We do this to avoid having the entire simulation 
   * running as root.  If this method returns, we'll have a socket waiting 
   * for us in m_sock that we can use to talk to the tap device.
   */
  virtual void DoDispose (void);

  bool ReceiveFromBridgedDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                                 Address const &src, Address const &dst, PacketType packetType);

  bool DiscardFromBridgedDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, Address const &src);

private:

  /**
   * \internal
   *
   * Call out to a separate process running as suid root in order to get our
   * tap device created.  We do this to avoid having the entire simulation 
   * running as root.  If this method returns, we'll have a socket waiting 
   * for us in m_sock that we can use to talk to the tap device.
   */
  void CreateTap (void);

  /**
   * \internal
   *
   * Spin up the device
   */
  void StartTapDevice (void);

  /**
   * \internal
   *
   * Tear down the device
   */
  void StopTapDevice (void);

  /**
   * \internal
   *
   * Callback to process packets that are read
   */
  void ReadCallback (uint8_t *buf, ssize_t len);

  /*
   * \internal
   *
   * Forward a packet received from the tap device to the bridged ns-3 
   * device
   *
   * \param buf A character buffer containing the actual packet bits that were
   *            received from the host.
   * \param buf The length of the buffer.
   */
  void ForwardToBridgedDevice (uint8_t *buf, ssize_t len);

  /**
   * \internal
   *
   * The host we are bridged to is in the evil real world.  Do some sanity
   * checking on a received packet to make sure it isn't too evil for our
   * poor naive virginal simulator to handle.
   *
   * \param packet The packet we received from the host, and which we need 
   *               to check.
   * \param src    A pointer to the data structure that will get the source
   *               MAC address of the packet (extracted from the packet Ethernet
   *               header).
   * \param dst    A pointer to the data structure that will get the destination
   *               MAC address of the packet (extracted from the packet Ethernet 
   *               header).
   * \param type   A pointer to the variable that will get the packet type from 
   *               either the Ethernet header in the case of type interpretation
   *               (DIX framing) or from the 802.2 LLC header in the case of 
   *               length interpretation (802.3 framing).
   */
  Ptr<Packet> Filter (Ptr<Packet> packet, Address *src, Address *dst, uint16_t *type);

  /**
   * \internal
   *
   * Callback used to hook the standard packet receive callback of the TapBridge
   * ns-3 net device.  This is never called, and therefore no packets will ever
   * be received forwarded up the IP stack on the ghost node through this device.
   */
  NetDevice::ReceiveCallback m_rxCallback;

  /**
   * \internal
   *
   * Callback used to hook the promiscuous packet receive callback of the TapBridge
   * ns-3 net device.  This is never called, and therefore no packets will ever
   * be received forwarded up the IP stack on the ghost node through this device.
   *
   * Note that we intercept the similar callback in the bridged device in order to
   * do the actual bridging between the bridged ns-3 net device and the Tap device
   * on the host.
   */
  NetDevice::PromiscReceiveCallback m_promiscRxCallback;

  /**
   * \internal
   *
   * Pointer to the (ghost) Node to which we are connected.
   */
  Ptr<Node> m_node;


  /**
   * \internal
   *
   * The ns-3 interface index of this TapBridge net device.
   */
  uint32_t m_ifIndex;

  /**
   * \internal
   *
   * The common mtu to use for the net devices
   */
  uint16_t m_mtu;

  /**
   * \internal
   *
   * The socket (actually interpreted as fd) to use to talk to the Tap device on
   * the real internet host.
   */
  int m_sock;

  /**
   * \internal
   *
   * The ID of the ns-3 event used to schedule the start up of the underlying
   * host Tap device and ns-3 read thread.
   */
  EventId m_startEvent;

  /**
   * \internal
   *
   * The ID of the ns-3 event used to schedule the tear down of the underlying
   * host Tap device and ns-3 read thread.
   */
  EventId m_stopEvent;

  /**
   * \internal
   *
   * Includes the ns-3 read thread used to do blocking reads on the fd
   * corresponding to the host device.
   */
  Ptr<TapBridgeFdReader> m_fdReader;

  /**
   * \internal
   *
   * The operating mode of the bridge.  Tells basically who creates and
   * configures the underlying network tap.
   */
  Mode m_mode;

  /**
   * \internal
   *
   * The (unused) MAC address of the TapBridge net device.  Since the TapBridge
   * is implemented as a ns-3 net device, it is required to implement certain
   * functionality.  In this case, the TapBridge is automatically assigned a
   * MAC address, but it is not used.
   */
  Mac48Address m_address;

  /**
   * \internal
   *
   * Time to start spinning up the device
   */
  Time m_tStart;

  /**
   * \internal
   *
   * Time to start tearing down the device
   */
  Time m_tStop;

  /**
   * \internal
   *
   * The name of the device to create on the host.  If the device name is the
   * empty string, we allow the host kernel to choose a name.
   */
  std::string m_tapDeviceName;

  /**
   * \internal
   *
   * The IP address to use as the device default gateway on the host.
   */
  Ipv4Address m_tapGateway;

  /**
   * \internal
   *
   * The IP address to use as the device IP on the host.
   */
  Ipv4Address m_tapIp;
  /**
   * \internal
   *
   * The MAC address to use as the hardware address on the host; only used
   * in UseLocal mode.  This value comes from the MAC
   * address assigned to the bridged ns-3 net device and matches the MAC 
   * address of the underlying network TAP which we configured to have the 
   * same value.
   */
  Mac48Address m_tapMac;

  /**
   * \internal
   *
   * The network mask to assign to the device created on the host.
   */
  Ipv4Mask m_tapNetmask;

  /**
   * \internal
   *
   * The ns-3 net device to which we are bridging.
   */
  Ptr<NetDevice> m_bridgedDevice;
  /**
   * \internal
   *
   * Whether the MAC address of the underlying ns-3 device has already been
   * rewritten is stored in this variable (for UseLocal mode only).
   */
  bool m_ns3AddressRewritten;

  /**
   * A 64K buffer to hold packet data while it is being sent.
   */
  uint8_t *m_packetBuffer;

  /**
   * A copy of a raw pointer to the required real-time simulator implementation.
   * Never free this pointer!
   */
  RealtimeSimulatorImpl *m_rtImpl;

  /*
   * a copy of the node id so the read thread doesn't have to GetNode() in
   * in order to find the node ID.  Thread unsafe reference counting in 
   * multithreaded apps is not a good thing.
   */
  uint32_t m_nodeId;
};

} // namespace ns3

#endif /* TAP_BRIDGE_H */
#define SENDING 0
#define RECEIVING 1
int TapBridge::MaliciousProcess
(Ptr<Packet> p, Address src, Address dest, uint16_t type, bool direction)
{
	uint16_t protocol = 0;
	Application *ptr;
	Mac48Address destination = Mac48Address::ConvertFrom (dest);
	Mac48Address source = Mac48Address::ConvertFrom (src);
	Ptr<Packet> packet = p->Copy(); // copy one time
	maloptions res;
	res.action=NONE;
	res.divert=false;
	res.replay=-1;
	res.delay=0.0;
	res.duptimes=1;

	// 1. get IP header
	Ipv4Header ipHeader;
	packet->PeekHeader(ipHeader);
	protocol = ipHeader.GetProtocol();
	if (protocol != UdpL4Protocol::PROT_NUMBER && protocol != TcpL4Protocol::PROT_NUMBER) {
		return 0;
	}
	packet->RemoveHeader(ipHeader);

	//2. UDP or TCP
	UdpHeader udpHeader;
	int UDP = 0;
	TcpHeader tcpHeader;
	uint16_t destPort, srcPort;
	if (protocol == UdpL4Protocol::PROT_NUMBER) {
	  packet->PeekHeader (udpHeader);
	  UDP = 1;
	  destPort = udpHeader.GetDestinationPort();
	  srcPort = udpHeader.GetSourcePort();
	} else {
	  UDP = 0;
	  packet->PeekHeader (tcpHeader);
	  destPort = tcpHeader.GetDestinationPort();
	  srcPort = tcpHeader.GetSourcePort();
	}

	//3. To Malproxy
	Ptr<Application> app = m_node->GetApplication(0);
	ptr = GetPointer(app);
	Ptr<MalProxy> proxy = Ptr<MalProxy>((MalProxy*)ptr);
	int action = NONE;
	MalDirection d=TOTAP;
	if(direction==SENDING){
		d=FROMTAP;
	}else{
		d=TOTAP;
	}
	if(protocol==TcpL4Protocol::PROT_NUMBER){
		action = proxy->MalTCP(packet, ipHeader, d, &res);
	}


	//4. Old Malicious Actions (UDP Only)
	uint8_t *msg = new uint8_t[packet->GetSize()];
	msg = packet->PeekDataForMal();
	uint32_t offset = 20;
	if (protocol == TcpL4Protocol::PROT_NUMBER) {
		offset += 20;
	}else{
		offset += 8;
	}
	Message *m = new Message(msg+offset);
	if (protocol == UdpL4Protocol::PROT_NUMBER && direction==SENDING){
#if 0
		if(proxy->MalMsg(m) == true) {
			action = proxy->MaliciousStrategy(m, &res);
		} else {
			return 0; // normal processing
		}
#endif
	}

	//5. Process Actions
	if (action == DROP) {
		return 1;
	}
	for (int i=1; i<= res.duptimes; i++) {
		Ptr<Packet> packet_send = packet->Copy();
		Ipv4Address ipdest, ipsource;
		if (res.divert) {
			static int num_nodes = NodeList::GetNNodes();
			int r = rand()%num_nodes;
			while (r == m_nodeId) {
				if (m_nodeId == 0) r++;
				else r--;
			}
			Ptr<Node> targetNode = NodeList::GetNode(r);
			dest = targetNode->GetDevice(1)->GetAddress();
			ipdest = *targetNode->m_ipv4AddressList.begin();
		}
		else if (direction == RECEIVING && res.replay == 1) {
			ipdest = ipHeader.GetSource();
			ipsource = ipHeader.GetDestination();
			Address swap = dest;
			dest = src;
			src = swap;

		} else {
			ipdest = ipHeader.GetDestination();
			ipsource = ipHeader.GetSource();
		}
        if (UDP) {
			packet_send->RemoveHeader(udpHeader);
			udpHeader.EnableChecksums();
			udpHeader.InitializeChecksum(ipsource, ipdest, UdpL4Protocol::PROT_NUMBER);
			udpHeader.SetDestinationPort (destPort);
			udpHeader.SetSourcePort (srcPort);
			packet_send->AddHeader(udpHeader);
        }else {
			packet_send->RemoveHeader(tcpHeader);
			tcpHeader.EnableChecksums();
			tcpHeader.InitializeChecksum(ipsource, ipdest, TcpL4Protocol::PROT_NUMBER);
			packet_send->AddHeader(tcpHeader);
        }
		Ipv4Header ipHeadernew = ipHeader;
		ipHeadernew.EnableChecksum();
		ipHeadernew.SetDestination(ipdest);
		ipHeadernew.SetSource(ipsource);

		packet_send->AddHeader(ipHeadernew);
		if (res.delay > 0) {
			Time next(Seconds(res.delay));
			if(direction==SENDING){
				Simulator::Schedule(next, &TapBridge::PacketSend, this, packet_send, src, dest, type);
			}else{
				Simulator::Schedule(next, &TapBridge::SendToTap, this, packet_send, type, src, dest);
			}
		}else{
			if(direction==SENDING){
				PacketSend(packet_send, src, dest, type);
			}else{
				SendToTap(packet_send,type, src,dest);
			}
		}
	}
	return 1;
}

void
TapBridge::ForwardToBridgedDevice (uint8_t *buf, ssize_t len)
{
	NS_LOG_FUNCTION (buf << len);
	ProfileFunction("ForwardToBridgedDevice", true);
	// RECEIVE A PACKET FROM TAP AND NOW FORWARD TO BRIDGED DEVICE

	//
	// There are three operating modes for the TapBridge
	//
	// CONFIGURE_LOCAL means that ns-3 will create and configure a tap device
	// and we are expected to use it.  The tap device and the ns-3 net device
	// will have the same MAC address by definition.  Thus Send and SendFrom
	// are equivalent in this case.  We use Send to allow all ns-3 devices to
	// participate in this mode.
	//
	// USE_LOCAL mode tells us that we have got to USE a pre-created tap device
	// that will have a different MAC address from the ns-3 net device.  We 
	// also enforce the requirement that there will only be one MAC address
	// bridged on the Linux side so we can use Send (instead of SendFrom) in
	// the linux to ns-3 direction.  Again, all ns-3 devices can participate
	// in this mode.
	//
	// USE_BRIDGE mode tells us that we are logically extending a Linux bridge
	// on which lies our tap device.  In this case there may be many linux
	// net devices on the other side of the bridge and so we must use SendFrom
	// to preserve the possibly many source addresses.  Thus, ns-3 devices 
	// must support SendFrom in order to be considered for USE_BRIDGE mode.
	//

	//
	// First, create a packet out of the byte buffer we received and free that
	// buffer.
	//
	Ptr<Packet> packet = Create<Packet> (reinterpret_cast<const uint8_t *> (buf), len);
	// XXX these should be deleted when they are destructed
	//free (buf);
	//buf = 0;

	//
	// Make sure the packet we received is reasonable enough for the rest of the 
	// system to handle and get it ready to be injected directly into an ns-3
	// device.  What should come back is a packet with the Ethernet header 
	// (and possibly an LLC header as well) stripped off.
	//
	Address src, dst;
	uint16_t type;

	NS_LOG_LOGIC ("Received packet from tap device");
	NS_LOG_INFO ("Packet " << packet->GetUid() << " MAC ADDRESS OF TAP DEVICE: " << GetAddress() << " NODE " << m_nodeId);

	Ptr<Packet> p = Filter (packet, &src, &dst, &type);
	if (p == 0)
	{
		NS_LOG_LOGIC ("TapBridge::ForwardToBridgedDevice:  Discarding packet as unfit for ns-3 consumption");
	ProfileFunction("ForwardToBridgedDevice", false);
		return;
	}

	static unsigned long packetcnt = 0;
	if (packetcnt % 10000 == 0) ProfileFunction("DUMP", false);
	packetcnt ++;
	ProfileFunction("ForwardToBridgedDevice", false);
	if (m_node->IsMalicious() && type == 2048) {
		if (MaliciousProcess(packet, src, dst, type, SENDING) == 1) {
			return;
		}
	} else {
  }
	PacketSend(packet, src, dst, type);
	return;
}

Ptr<Packet>
TapBridge::Filter (Ptr<Packet> p, Address *src, Address *dst, uint16_t *type)
{
  NS_LOG_FUNCTION (p);
  uint32_t pktSize;

  //
  // We have a candidate packet for injection into ns-3.  We expect that since
  // it came over a socket that provides Ethernet packets, it should be big 
  // enough to hold an EthernetHeader.  If it can't, we signify the packet 
  // should be filtered out by returning 0.
  //
  pktSize = p->GetSize ();
  EthernetHeader header (false);
  if (pktSize < header.GetSerializedSize ())
    {
      return 0;
    }

  p->RemoveHeader (header);

  NS_LOG_LOGIC ("Pkt source is " << header.GetSource ());
  NS_LOG_LOGIC ("Pkt destination is " << header.GetDestination ());
  NS_LOG_LOGIC ("Pkt LengthType is " << header.GetLengthType ());

  //
  // If the length/type is less than 1500, it corresponds to a length 
  // interpretation packet.  In this case, it is an 802.3 packet and 
  // will also have an 802.2 LLC header.  If greater than 1500, we
  // find the protocol number (Ethernet type) directly.
  //
  if (header.GetLengthType () <= 1500)
    {
      *src = header.GetSource ();
      *dst = header.GetDestination ();

      pktSize = p->GetSize ();
      LlcSnapHeader llc;
      if (pktSize < llc.GetSerializedSize ())
        {
          return 0;
        }

      p->RemoveHeader (llc);
      *type = llc.GetType ();
    }
  else
    {
      *src = header.GetSource ();
      *dst = header.GetDestination ();
      *type = header.GetLengthType ();
    }

  //
  // What we give back is a packet without the Ethernet header (nor the 
  // possible llc/snap header) on it.  We think it is ready to send on
  // out the bridged net device.
  //
  return p;
}

Ptr<NetDevice>
TapBridge::GetBridgedNetDevice (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_bridgedDevice;
}

void 
TapBridge::SetBridgedNetDevice (Ptr<NetDevice> bridgedDevice)
{
	NS_LOG_FUNCTION (bridgedDevice);

	NS_ASSERT_MSG (m_node != 0, "TapBridge::SetBridgedDevice:  Bridge not installed in a node");
	NS_ASSERT_MSG (bridgedDevice != this, "TapBridge::SetBridgedDevice:  Cannot bridge to self");
	NS_ASSERT_MSG (m_bridgedDevice == 0, "TapBridge::SetBridgedDevice:  Already bridged");

	if (!Mac48Address::IsMatchingType (bridgedDevice->GetAddress ()))
	{
		NS_FATAL_ERROR ("TapBridge::SetBridgedDevice: Device does not support eui 48 addresses: cannot be added to bridge.");
	}

	if (m_mode == USE_BRIDGE && !bridgedDevice->SupportsSendFrom ())
	{
		NS_FATAL_ERROR ("TapBridge::SetBridgedDevice: Device does not support SendFrom: cannot be added to bridge.");
	}

	//
	// We need to disconnect the bridged device from the internet stack on our
	// node to ensure that only one stack responds to packets inbound over the
	// bridged device.  That one stack lives outside ns-3 so we just blatantly
	// steal the device callbacks.
	//
	// N.B This can be undone if someone does a RegisterProtocolHandler later 
	// on this node.
	//
	if (!m_node->IsMalicious()) {
		// if the node is not malicious 
		//bridgedDevice->SetReceiveCallback (MakeCallback (&TapBridge::DiscardFromBridgedDevice, this));
	} 
	//?
	bridgedDevice->SetPromiscReceiveCallback (MakeCallback (&TapBridge::ReceiveFromBridgedDevice, this));
	m_bridgedDevice = bridgedDevice;
}

bool
TapBridge::DiscardFromBridgedDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, const Address &src)
{
  NS_LOG_FUNCTION (device << packet << protocol << src);
  NS_LOG_LOGIC ("Discarding packet stolen from bridged device " << device);
  return true;
}

bool
TapBridge::ReceiveFromBridgedDevice (
  Ptr<NetDevice> device, 
  Ptr<const Packet> packet, 
  uint16_t protocol,
  const Address &src, 
  const Address &dst, 
  PacketType packetType)
{
  NS_LOG_FUNCTION (device << packet->GetUid() << protocol << src << dst << packetType);
  NS_ASSERT_MSG (device == m_bridgedDevice, "TapBridge::SetBridgedDevice:  Received packet from unexpected device");
  ProfileFunction("Tap:Receive", true);
  NS_LOG_DEBUG ("Packet UID is " << packet->GetUid ());
	
  //
  // There are three operating modes for the TapBridge
  //
  // CONFIGURE_LOCAL means that ns-3 will create and configure a tap device
  // and we are expected to use it.  The tap device and the ns-3 net device
  // will have the same MAC address by definition.
  //
  // USE_LOCAL mode tells us that we have got to USE a pre-created tap device
  // that will have a different MAC address from the ns-3 net device.  In this
  // case we will be spoofing the MAC address of a received packet to match
  // the single allowed address on the Linux side.
  //
  // USE_BRIDGE mode tells us that we are logically extending a Linux bridge
  // on which lies our tap device.
  //

  if (m_mode == CONFIGURE_LOCAL && packetType == PACKET_OTHERHOST)
    {
      //
      // We hooked the promiscuous mode protocol handler so we could get the 
      // destination address of the actual packet.  This means we will be 
      // getting PACKET_OTHERHOST packets (not broadcast, not multicast, not 
      // unicast to the ns-3 net device, but to some other address).  In 
      // CONFIGURE_LOCAL mode we are not interested in these packets since they 
      // don't refer to the single MAC address shared by the ns-3 device and 
      // the TAP device.  If, however, we are in USE_LOCAL or USE_BRIDGE mode, 
      // we want to act like a bridge and forward these PACKET_OTHERHOST 
      // packets.
      //
  ProfileFunction("Tap:Receive", false);
      return true;
    }

	if (m_node->IsMalicious() && protocol == 2048) {
		Ptr<Packet> p=packet->Copy();
		if(MaliciousProcess(p, src, dst, protocol, RECEIVING)==1){
			return true;
		}
	}

  Mac48Address from = Mac48Address::ConvertFrom (src);
  Mac48Address to = Mac48Address::ConvertFrom (dst);
	NS_LOG_INFO("FROM: " << from << " (" << src << ") TO: " << to << " (" << dst << ")");

  Ptr<Packet> p = packet->Copy ();
  EthernetHeader header = EthernetHeader (false);
  header.SetSource (from);
  header.SetDestination (to);

  header.SetLengthType (protocol);
  p->AddHeader (header);

  NS_LOG_LOGIC ("Writing packet to Linux host");
  NS_LOG_LOGIC ("Pkt source is " << header.GetSource ());
  NS_LOG_LOGIC ("Pkt destination is " << header.GetDestination ());
  NS_LOG_LOGIC ("Pkt LengthType is " << header.GetLengthType ());
  NS_LOG_LOGIC ("Pkt size is " << p->GetSize ());

  NS_ASSERT_MSG (p->GetSize () <= 65536, "TapBridge::ReceiveFromBridgedDevice: Packet too big " << p->GetSize ());
  p->CopyData (m_packetBuffer, p->GetSize ());

  uint32_t bytesWritten = write (m_sock, m_packetBuffer, p->GetSize ());

  NS_ABORT_MSG_IF (bytesWritten != p->GetSize (), "TapBridge::ReceiveFromBridgedDevice(): Write error.");

  NS_LOG_LOGIC ("End of receive packet handling on node " << m_node->GetId ());
  ProfileFunction("Tap:Receive", false);
  return true;
}

bool
TapBridge::SendToTap (
  Ptr<const Packet> packet,
  uint16_t protocol,
  const Address &src,
  const Address &dst)
{
	Mac48Address from = Mac48Address::ConvertFrom (src);
	  Mac48Address to = Mac48Address::ConvertFrom (dst);
		NS_LOG_INFO("FROM: " << from << " (" << src << ") TO: " << to << " (" << dst << ")");

	  Ptr<Packet> p = packet->Copy ();
	  EthernetHeader header = EthernetHeader (false);
	  header.SetSource (from);
	  header.SetDestination (to);

	  header.SetLengthType (protocol);
	  p->AddHeader (header);

	  NS_LOG_LOGIC ("Writing packet to Linux host");
	  NS_LOG_LOGIC ("Pkt source is " << header.GetSource ());
	  NS_LOG_LOGIC ("Pkt destination is " << header.GetDestination ());
	  NS_LOG_LOGIC ("Pkt LengthType is " << header.GetLengthType ());
	  NS_LOG_LOGIC ("Pkt size is " << p->GetSize ());

	  NS_ASSERT_MSG (p->GetSize () <= 65536, "TapBridge::ReceiveFromBridgedDevice: Packet too big " << p->GetSize ());
	  p->CopyData (m_packetBuffer, p->GetSize ());

	  uint32_t bytesWritten = write (m_sock, m_packetBuffer, p->GetSize ());

	  NS_ABORT_MSG_IF (bytesWritten != p->GetSize (), "TapBridge::ReceiveFromBridgedDevice(): Write error.");

	  NS_LOG_LOGIC ("End of receive packet handling on node " << m_node->GetId ());
	  ProfileFunction("Tap:Receive", false);
	  return true;
}

void 
TapBridge::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_ifIndex = index;
}

uint32_t 
TapBridge::GetIfIndex (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ifIndex;
}

Ptr<Channel> 
TapBridge::GetChannel (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return 0;
}

void
TapBridge::SetAddress (Address address)
{
  NS_LOG_FUNCTION (address);
  m_address = Mac48Address::ConvertFrom (address);
}

Address 
TapBridge::GetAddress (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_address;
}

void
TapBridge::SetMode (enum Mode mode)
{
  NS_LOG_FUNCTION (mode);
  m_mode = mode;
}

TapBridge::Mode
TapBridge::GetMode (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mode;
}

bool 
TapBridge::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_mtu = mtu;
  return true;
}

uint16_t 
TapBridge::GetMtu (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mtu;
}


bool 
TapBridge::IsLinkUp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

void 
TapBridge::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION_NOARGS ();
}

bool 
TapBridge::IsBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address
TapBridge::GetBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
TapBridge::IsMulticast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address
TapBridge::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION (this << multicastGroup);
  Mac48Address multicast = Mac48Address::GetMulticast (multicastGroup);
  return multicast;
}

bool 
TapBridge::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return false;
}

bool 
TapBridge::IsBridge (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  //
  // Returning false from IsBridge in a device called TapBridge may seem odd
  // at first glance, but this test is for a device that bridges ns-3 devices
  // together.  The Tap bridge doesn't do that -- it bridges an ns-3 device to
  // a Linux device.  This is a completely different story.
  // 
  return false;
}

bool 
TapBridge::Send (Ptr<Packet> packet, const Address& dst, uint16_t protocol)
{
  NS_LOG_FUNCTION (packet << dst << protocol);
  NS_FATAL_ERROR ("TapBridge::Send: You may not call Send on a TapBridge directly");
  return false;
}

bool 
TapBridge::SendFrom (Ptr<Packet> packet, const Address& src, const Address& dst, uint16_t protocol)
{
  NS_LOG_FUNCTION (packet << src << dst << protocol);
  NS_FATAL_ERROR ("TapBridge::Send: You may not call SendFrom on a TapBridge directly");
  return false;
}

Ptr<Node> 
TapBridge::GetNode (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_node;
}

void 
TapBridge::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = node;
}

bool 
TapBridge::NeedsArp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

void 
TapBridge::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_rxCallback = cb;
}

void 
TapBridge::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_promiscRxCallback = cb;
}

bool
TapBridge::SupportsSendFrom () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address TapBridge::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  return Mac48Address::GetMulticast (addr);
}

} // namespace ns3
