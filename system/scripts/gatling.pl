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

#use Attack;

require MsgParse;
require Utils;
require GatlingConfig;

no warnings 'once';

$GatlingConfig::systemname = "BFT";
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

# prepare VMs clean
#Attack::start_Listener();
while (1) {
  system("./startNclean.sh"); # prepare VMs clean
  GatlingConfig::movePrevPerf();
# as this will parse messages, it should be after setting the system
  Attack::prepareMessages();
  GatlingConfig::prepare(); # start log servers

  print "startNS3\n"; # Start NS-3
  my $ns3_thread;
  if ($GatlingConfig::startNS3 == 1) {
    $GatlingConfig::watch_ns3 = 1;
    $ns3_thread = threads->create('ns3_thread', "./run_command.sh \"malproxy_simple $GatlingConfig::mal -num_vms 5 -ip_base 10.1.2 -tap_base tap-ns $GatlingConfig::watchPort -runtime $GatlingConfig::runTime\"");  
  }

# ping
  sleep 3;
  system("pssh -P -h pssh_all.txt -t 5 \"ping 10.1.2.5 -c 5\"");

  if ($GatlingConfig::watch_ns3 == 0) {
    print "Gatling suspended due to NS3 termination\n";
    exit;
  }

  my $gatling_thread = threads->create('Attack::start_Listener');
  while ($GatlingConfig::watch_ns3 == 1 && $GatlingConfig::watch_turret) {
    sleep 1;
  }
  print "out $GatlingConfig::watch_ns3\n";
  if ($GatlingConfig::watch_ns3 == 0) {
    $gatling_thread->kill('SIGTERM');
  } else {
    $ns3_thread->kill('SIGTERM');
  }
  exit;
  #print "out2\n";
  #close $GatlingConfig::socket;
  #print "NOW prepare to start again\n";
  #sleep 2; # binding
}
