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

require MsgParse;
require Utils;
require GatlingConfig;

no warnings 'once';

#$GatlingConfig::systemname = "TCP";
#if ( $#ARGV != -1 ) {
	#$GatlingConfig::systemname = $ARGV[0];
#}
#print "Target system name: $GatlingConfig::systemname\n";

require StateBasedController;

# XXX will need to be updated
#GatlingConfig::movePrevPerf();
#GatlingConfig::prepare();

#Do Attack
#GreedyAttack::start();
StateBasedController::start();
exit;
