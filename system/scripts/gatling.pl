#!/usr/bin/env perl
# FYI, PERF SCORE IS MEASURED SO THAT BIGGER IS WORSE

# PERF SCORE - BIGGER IS WORSE

use warnings;
use strict;
use IO::Socket;
use Time::Local;
use threads (
	'yield',
	'stack_size' => 64 * 4096,
	'exit'       => 'threads_only',
	'stringify'
);
use threads::shared;
use lib ("./Gatling/");

require Utils;
require GatlingConfig;
require Executor;

no warnings 'once';

$GatlingConfig::systemname = "TCP";
if ( $#ARGV != -1 ) {
    if (not $ARGV[0] =~ /-/) {
        $GatlingConfig::systemname = $ARGV[0];
    }
}

for (my $i = 0; $i < $#ARGV; $i++) {
    if ($ARGV[$i] =~ "-offset") {
        $GatlingConfig::offset = $ARGV[$i+1];
    }
    if ($ARGV[$i] =~ "-special") {
        $GatlingConfig::specialVMoption = "-special $ARGV[$i+1]";
    }
}

print "Target system name: $GatlingConfig::systemname offset: $GatlingConfig::offset\n";

GatlingConfig::setSystem();
share($GatlingConfig::watch_ns3);
share($GatlingConfig::watch_turret);

#Start VMs
system("./startNclean.sh offset $GatlingConfig::offset pssh \"$GatlingConfig::allListString\"");

#Initialize Turret system
GatlingConfig::prepare();

Executor::start();
exit;
