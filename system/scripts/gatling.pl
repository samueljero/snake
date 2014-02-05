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
  Attack::prepareMessages();
  GatlingConfig::prepare();

  print "Starting NS-3\n";
  my $ns3_thread;
  if ($GatlingConfig::startNS3 == 1) {
    $GatlingConfig::watch_ns3 = 1;
    $ns3_thread = threads->create('ns3_thread', "./run_command.sh \"malproxy_simple $GatlingConfig::mal -num_vms 5 -ip_base 10.1.2 -tap_base tap-ns $GatlingConfig::watchPort -runtime $GatlingConfig::runTime\"");  
  }
  
  #Verify that VMs and NS-3 are up
  sleep 3;
  system("pssh -P -h pssh_all.txt -t 5 \"ping 10.1.2.5 -c 5\"");

  if ($GatlingConfig::watch_ns3 == 0) {
    print "Gatling suspended due to NS3 termination\n";
    exit;
  }
  
  #Begin Attack!
  my $gatling_thread = threads->create('Attack::start_Listener');
  while ($GatlingConfig::watch_ns3 == 1 && $GatlingConfig::watch_turret) {
    sleep 1;
  }
  
  #Sombody died...
  if ($GatlingConfig::watch_ns3 == 0) {
    $gatling_thread->kill('SIGTERM');
  } else {
    $ns3_thread->kill('SIGTERM');
  }
  exit;
}
