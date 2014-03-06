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

require Attack;

share ($GatlingConfig::watch_ns3);
share ($GatlingConfig::watch_turret);
sub ns3_thread {
  my @args = @_;
  print "NS-3 start: [".(join (' ', @args))."]\n";
  system (join (' ', @args, "\n"));
  $GatlingConfig::watch_ns3 = 0;
  system ("pkill serverLog\n");
  system ("pkill clientLog\n");
  print "NS3 finished\n";
  exit();
}


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
