#!/usr/bin/env perl

use warnings;
use strict;
use IO::Socket::INET;
use Time::Local;
use Fcntl;
use Sys::Hostname;
use threads;
my $host = hostname;


my $flags = O_NONBLOCK;
fcntl(STDIN, F_GETFL, $flags);
$flags |= O_NONBLOCK;
fcntl(STDIN, F_SETFL, $flags);

my $offset = 0;
if ($#ARGV >= 0) {
    $offset = $ARGV[0];
}

my $rin = '';
vec($rin,fileno(STDIN),1) = 1;
my $rout;

my $sockBase = 7779 + $offset;
while (1) {
	select($rout=$rin, undef, undef, undef);
	last if eof();

	my $buffer = '';

	my $sock = new IO::Socket::INET->new( PeerAddr => "10.0.0.1:$sockBase",
			#PeerPort => '7777', 
			Proto => 'udp');
	die "Could not create socket: $!\n" unless $sock;
	while (defined(my $c = getc())) {
		$buffer .= $c;
	}
	
	print $sock "$buffer";
	close $sock;
}
