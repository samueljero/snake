#!/usr/bin/env perl

use IO::Socket::INET;

$| = 1;
my $addr = '10.0.0.1';
if ($#ARGV>=0) {
  $addr = $ARGV[0];
}
my ($socket,$received_data);
my ($peeraddress,$peerport);

$socket = new IO::Socket::INET (
LocalHost => $addr,
LocalPort => '7770',
Proto => 'udp',
) or die "ERROR in Socket Creation : $!\n";

while(1)
{
  $socket->recv($recieved_data,1024);

  $peer_address = $socket->peerhost();
  $peer_port = $socket->peerport();
  print "($peer_address , $peer_port) said : $recieved_data";

  $data = "server received $recieved_data\n";
  $socket->send($data);
}

$socket->close();
