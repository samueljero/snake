#!/usr/bin/perl -w

# PERF SCORE - BIGGER IS WORSE

use strict;
use IO::Socket; 
use Time::Local;
use threads ('yield',
     'stack_size' => 64*4096,
     'exit' => 'threads_only',
     'stringify');
use threads::shared;
use lib ("./Gatling/");

require MsgParse;
require Utils;
require GatlingConfig;

no warnings 'once';

$GatlingConfig::systemname = "TCP";
if ($#ARGV != -1) {
  $GatlingConfig::systemname = $ARGV[0];
}
print "Target system name: $GatlingConfig::systemname\n";

Utils::useKVM();
GatlingConfig::setSystem();

require GreedyAttack;

share ($GatlingConfig::watch_ns3);
share ($GatlingConfig::watch_turret);


while (1) {
  #Start VMs
  system("./startNclean.sh");
  
  #Initialize Turret system
  GatlingConfig::movePrevPerf();
  GatlingConfig::prepare();

  #Do greedy Attack
  GreedyAttack::start();
  exit;
}
