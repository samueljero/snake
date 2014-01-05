#!/usr/bin/perl -w

use strict;

my $system = "prime";

if ($#ARGV >= 0) {
	$system = $ARGV[0];
}

print "Switching to $system\n";


system("./createMessageLying.pl $system.format");
system("cp strategy $system.strategy");
system("cp message.h ../../ns-3-dev/src/applications/model/");
system("cp message.cc ../../ns-3-dev/src/applications/model/");

=pod
if ($system eq "steward") {
	system("cd ../ns-3-dev/src/csma/model; cp simple-csma-channel.cc csma-channel.cc");
	system("cd ../ns-3-dev/src/csma/model; cp simple-csma-net-device.cc csma-net-device.cc");
} else {
	system("cd ../ns-3-dev/src/csma/model; cp full-csma-channel.cc csma-channel.cc");
	system("cd ../ns-3-dev/src/csma/model; cp full-csma-net-device.cc csma-net-device.cc");
}
=cut

system("cd ../../ns-3-dev/; ./waf -j4");


