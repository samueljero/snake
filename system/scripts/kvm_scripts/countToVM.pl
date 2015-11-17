#!/usr/bin/env perl

use strict;
use IO::Socket::INET;
use Sys::Hostname;

my $i = 0;
my $host = hostname;
my $addr = '10.1.1.1';
if ( $host =~ /^sound/ or $host =~ /^ocean1/ ) {
    $addr = "10.0.1.1";
}

if ($#ARGV >= 0) {
	$addr = $ARGV[0];
}
while (1) {
	my $sock = new IO::Socket::INET->new( PeerAddr => $addr.':7770',
			#PeerPort => '7777', 
			Proto => 'udp');
	die "Could not create socket: $!\n" unless $sock;
	print $sock "$host] $i\n";
	close $sock;
  $i = $i + 1;
  sleep 1;
}
