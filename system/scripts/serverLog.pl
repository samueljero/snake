#!/usr/bin/env perl

#

#use strict;
use warnings;
use IO::Socket::INET;
#use Time::Local;
use threads;

my $socket = new IO::Socket::INET (
		LocalHost => '10.0.0.1',
		LocalPort => '7777',
		Proto => 'udp',
		#Listen => 50,
		#Reuse => 1
		) or die "Error in Socket Creation for serverLog : $!\n";


print "Controller listens on port 7777\n";

#sub printLog($)
#{
	#my ($logsock) = @_;
	#my $line;
	#while (1) {
	#$logsock->recv($line, 1024);
	#print $line;
	#}
	#close $logsock;
#}

my $line;
while (1) 
{
	#my $sock = $socket->accept();
	$socket->recv($line, 1024);
	print $line;
	#close $sock;
	#my $thr = threads->new(\&printLog, $sock);
}
