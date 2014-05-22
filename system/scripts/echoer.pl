#!/usr/bin/env perl

use strict;
use warnings;
use IO::Socket::INET;
#use Time::Local;
use threads;
use Scalar::Util qw(looks_like_number);
use lib ("./Gatling/");
require GatlingConfig;

my $socket = new IO::Socket::INET (
		LocalHost => "0.0.0.0",
		LocalPort => '7779',
		Proto => 'tcp',
		Listen => 50,
		Reuse => 1
		) or die "Error in Socket Creation for perfMonitor : $!\n";
print "Listen on 7779\n";

while (1) 
{
	my $sock = $socket->accept();
	my $line;
        $sock->recv($line, 1024);
	chop($line);
        print "received : $line\n";
        print $sock "echo\n";
}
