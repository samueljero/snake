#!/usr/bin/env perl

use strict;
use warnings;
use IO::Socket::INET;

package GreedyAttack;
require Strategy;
require Utils;
require GatlingConfig;
require Perf;

my $fieldsPerMsgRef = Strategy::parseMessage();
my $msgNameRef = $Strategy::msgNameRef;
my $passone = 0;

my $messageCount = Strategy::getMsgNameClount();
my $msgTypeRef = $Strategy::msgType;
my $typeStrategyRef = Strategy::getStrategyCount();
my $typeStrategyListRef = Strategy::getTypeStrategyList();
my $msgFlenList = Strategy::getMsgFlenList();
my $FlenList = Strategy::getFlenList();
my $FlenNumList = Strategy::getFlenNumList();

print "Total $messageCount message types\n";

my %strategyCount;
my %strategyList;
my @curActionIndex;
my @learned;
my %tally; # how many times chosen
my %excludeStrategy; # actions to exclude
my %perfScore; # latest performance recorded
my $learnedStrategyString = "";

$SIG{TERM} = 'term_handler';
$SIG{KILL} = 'term_handler';

sub term_handler {
  print "term_handler\n";
  close $GreedyAttack::socket;
  print "socket closed\n";
  $GatlingConfig::watch_ns3 = 2;
  exit;
}

sub perf_collection_thread {
	GatlingConfig::offsetScoreFile();
	my $socknumber = 7779 + $GatlingConfig::offset;

	my $socket = new IO::Socket::INET (
			LocalHost => '10.0.0.1',
			LocalPort => $socknumber,
			Proto => 'udp',
			) or die "Error in Socket Creation for perfMonitor : $!\n";

	open (PERFFILE, ">$GatlingConfig::scoreFile");
	print PERFFILE "0\n";
	close (PERFFILE);
	print "perfMonitor listens on port $socknumber\n";

	my $line;
	while (1)
	{
		$socket->recv($line, 1024);
		my @lines = split /\n/, $line;
		foreach my $eachline (@lines) {
			if($GatlingConfig::systemname eq "Prime"){
				Perf::Prime_bugPerf($eachline);
				#Perf::PrimePerf($eachline);
			}elsif($GatlingConfig::systemname eq "BFT"){
				Perf::BFTPerf($eachline);
			}elsif($GatlingConfig::systemname eq "Steward"){
				Perf::StewardPerf($eachline);
			}elsif($GatlingConfig::systemname eq "TCP"){
				Perf::TCP_Perf($eachline);
			}elsif($GatlingConfig::systemname eq "DCCP"){
				Perf::DCCP_Perf($eachline);
			}else{
				print "ERROR: Unknown system!\n";
			}
		}
	}
}

sub prepareMessages {
  #Prepare perfLog
  open PERF_LOG, "+>", $GatlingConfig::perfMeasured or die "Can't create $GatlingConfig::perfMeasured $!";
  print PERF_LOG "# MsgName MsgType action Index perfValue (Chosen)\n";
  
  #Prepare decision log
  open NEW_LEARNED, "+>", $GatlingConfig::newlyLearned or die "Can't create $GatlingConfig::newlyLearned $!";
  
  #Create data structure of all possible attacks
  for (my $i = 0; $i <= $messageCount; $i++) {
  	#for each message
    if ($GatlingConfig::debug > 1) {
      my $nameStr = $Strategy::msgName[$i];
      print "$i] - $nameStr - $Strategy::msgType[$i] $Strategy::msgTypeList{$nameStr}\n";
    }
    my @strategyListForMessage;
    my @score;
    my @selected;
    my @excluded;
    my $strategyForMessage = 0;
    $learned[$i] = -1;
    
    #Add NONE, Drop, Dup, Delay, Divert commands for this message
    $strategyListForMessage[0]= "*?*?$Strategy::msgName[$i] NONE 0";
    $strategyListForMessage[1]= "*?*?$Strategy::msgName[$i] DROP 100";
    $strategyListForMessage[1]= "*?*?$Strategy::msgName[$i] DROP 50";
    $strategyListForMessage[2]= "*?*?$Strategy::msgName[$i] DUP 10";
    $strategyListForMessage[3]= "*?*?$Strategy::msgName[$i] DELAY 1.0";
    $strategyListForMessage[4]= "*?*?$Strategy::msgName[$i] DIVERT 1.0";
    for (my $j = 0; $j < 5; $j++) { # dup, delay, divert, benign, drop
      push(@score, 9999);
      push(@selected, 0);
      push(@excluded, 0);
      $strategyForMessage = $strategyForMessage+1;
    }
    
    #For each field in this message
    for (my $j = 0; $j  <= $#{$fieldsPerMsgRef->{$i}}; $j++) {
      if($msgFlenList->{$i}[$j]>0){
      	#Bit fields
      	#For each value it makes sense to lie on based on field type
      	for (my $k = 0; $k <= $FlenNumList->{$msgFlenList->{$i}[$j]}; $k++) {
          #Add this lie command for message
	  #print "$Strategy::msgName[$i] LIE $FlenList->{$msgFlenList->{$i}[$j]}[$k] $j\n";
	  push(@strategyListForMessage, "*?*?$Strategy::msgName[$i] LIE $FlenList->{$msgFlenList->{$i}[$j]}[$k] $j");
          push(@score, 9999);
          push(@selected, 0);
          push(@excluded, 0); 
          $strategyForMessage = $strategyForMessage+1;
        }
      }elsif ($typeStrategyRef->{$fieldsPerMsgRef->{$i}[$j]} > 0) {
      	#For each value it makes sense to lie on based on field type
        for (my $k = 0; $k <= $typeStrategyRef->{$fieldsPerMsgRef->{$i}[$j]}; $k++) {
          #Add this lie command for message
          push(@strategyListForMessage, "*?*?$Strategy::msgName[$i] LIE $typeStrategyListRef->{$fieldsPerMsgRef->{$i}[$j]}[$k] $j");
          push(@score, 9999);
          push(@selected, 0);
          push(@excluded, 0); 
          $strategyForMessage = $strategyForMessage+1;
        }
      }
    }
    
    #Final totals
    $strategyCount{$i} = $strategyForMessage;
    $strategyList{$i} = \@strategyListForMessage;
    $curActionIndex[$i] = 0;
    $tally{$i} = \@selected;
    $excludeStrategy{$i} = \@excluded;
    $perfScore{$i} = \@score;
  }

##LOAD STRATEGIES LEARNED BEFORE THIS EXPERIMENT
  my $res = open PRELEARNED, "<", $GatlingConfig::alreadyLearned;
  if ($res) {
    while (my $learnedLine = <PRELEARNED>) {
      chomp($learnedLine);
      my @token = split(/ /, $learnedLine);
      if ($#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/)
      {
        next;
      }
      my $name = $token[0];
      
      my $msgType = $Strategy::msgTypeList{$name};
      for (my $i = 0; $i <= $messageCount; $i++) {
      	if($Strategy::msgName[$i] eq $name){
      		$msgType=$i;
      	}
      }	
      my $strategy = join (' ', @token);
      #print "PRE LEARNED $msgType for $name strategy $strategy\n";
      my $actionIndex = 0;
      FIND_STRATEGY:
      for (my $i = 0; $i < $strategyCount{$msgType}; $i++) {
        if ($strategy eq $strategyList{$msgType}[$i]) {
          $actionIndex = $i;
          #print "FOUND $actionIndex = $i for $strategy = $strategyList{$msgType}[$i]\n";
          last FIND_STRATEGY;
        }
      }
      $learned[$msgType] = $actionIndex;
      $strategyList{$msgType}[$actionIndex] = $strategy;
      learnAction($msgType, $actionIndex, $strategy, 0);
    }
    close PRELEARNED;
    if ($GatlingConfig::debug > 0) {
      print "Prelearned Strategies:\n";
      for (my $i = 0; $i <= $messageCount; $i++) {
        if ($learned[$i] >= 0) {
          my $learnedStrategy = $learned[$i];
          print "Learned Msg $i strategy # [$learnedStrategy]\n";
        }
      }
    }
  }

##LOAD PREFORMANCE MEASURED IN PREV RUN (IF ANY)
  $res = open PRE_PERF, "<", $GatlingConfig::prePerf;
  if ($res) {
    while (my $line = <PRE_PERF>) {
      chomp($line);
      my @token = split(/ /, $line);
      if ($#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/)
      {
        next;
      }
      my $msgName = $token[0];
      my $msgType = $token[1];
      my $actionIndex = $token[2];
      my $perfValue = $token[3];
      if ($learned[$msgType] == -1) {
        if ($#token > 3 and $token[4] eq "Chosen") {
          chooseAction($msgType, $actionIndex, 0, $msgName);
        } elsif ($#token > 3 and ($token[4] eq "Exclude" || $token[5] eq "Exclude")) {
          excludeAction($msgType, $actionIndex, 0, $msgName);
        } else {
          $perfScore{$msgType}[$actionIndex] = $perfValue;
          $curActionIndex[$msgType] = $actionIndex + 1;
        }
      }
    }
    close PRE_PERF;
  }

  if ($GatlingConfig::debug > 1) {
  	print "Current Strategy Performance:\n";
    for (my $i = 0; $i <= $messageCount; $i++) {
      for (my $j =0 ; $j < $strategyCount{$i}; $j++) {
        print "For Msg $Strategy::msgName[$i] strategy $j -- tally: $tally{$i}[$j], ";
        print " perf: $perfScore{$i}[$j], ";
        print " exclude: $excludeStrategy{$i}[$j]\n";
      }
      print "Current Action Index for $Strategy::msgName[$i] : $curActionIndex[$i]\n";
    }
  }


}

sub excludeAction {
  my $curMsgType = shift;
  my $actionIndex = shift;
  my $now = shift;
  my $msgName = shift;
  my $strategy = $strategyList{$curMsgType}[$actionIndex];
  $excludeStrategy{$curMsgType}[$actionIndex] = 1;
  print NEW_LEARNED "#EXCLUDE $curMsgType $actionIndex ($msgName, $strategy)\n";
}

sub learnAction {
  my $curMsgType = shift;
  my $actionIndex = shift;
  my $strategy = shift;
  my $now = shift; #tell Mal-proxy right now. Needed to handle pre-load before mal-proxy is running.
  
  $learned[$curMsgType] = $actionIndex;
  my $curMsgName=$Strategy::msgName[$curMsgType];
  $learnedStrategyString = $learnedStrategyString.",".$strategy;
  print "LEARNED STRATEGY FOR MSG $curMsgType action $actionIndex ($strategyList{$curMsgType}[$actionIndex])\n";
  
  if ($now == 1) {
    Utils::directTopology("C Learned $strategy");
    print "LEARNED STRATEGY FOR MSG $curMsgType action $actionIndex ($strategyList{$curMsgType}[$actionIndex])\n";
	print NEW_LEARNED "$strategy\n";
    print PERF_LOG "$curMsgName $curMsgType $actionIndex 9999 Exclude\n";
  }
}

sub chooseAction {
  my $curMsgType = shift;
  my $actionIndex = shift;
  my $now = shift;
  my $curMsgName = shift;
  
  print PERF_LOG "#tally fro $curMsgName $actionIndex increase from $tally{$curMsgType}[$actionIndex] ";
  $tally{$curMsgType}[$actionIndex] = $tally{$curMsgType}[$actionIndex] + 1;
  print PERF_LOG "to $tally{$curMsgType}[$actionIndex]\n";
  
  if ($tally{$curMsgType}[$actionIndex] >= $GatlingConfig::learning_threashold) {
    	learnAction($curMsgType, $actionIndex, $strategyList{$curMsgType}[$actionIndex], $now);
  }
}

sub decisionMaker
{
	my $curMsgType=shift;

	if ($curMsgType < 0 || $curMsgType > $messageCount) { # message not interested in
    	print PERF_LOG "#Not interested for $curMsgType\n";
		return;
	}
	
	print "decisionMaker for MsgType $curMsgType ($Strategy::msgName[$curMsgType])\n";
	
	if ($learned[$curMsgType] >= 0) { # message we've already picked a strategy for!
		Utils::directTopology("C Learned $learnedStrategyString");
		print "Resending learned strategy: $learnedStrategyString\n";
    	return;
	}
	
	#Freeze and snapshot system
	Utils::logTime("end do gatling");
  	Utils::freezeNS3();
	Utils::pauseVMs();
	Utils::takeTime();
	Utils::updateSnapshot(-1);
	Utils::snapshotVMs();
	Utils::saveNS3();
	Utils::logTime("start do gatling");
	
	#Loop over all strategies for this message
	my $curMsgName=$Strategy::msgName[$curMsgType];
	my $repeat = 0;
	my $worstScore = 0;
  	my $actionIndex = 0;
ACTION_TEST:
  for (my $i = $curActionIndex[$curMsgType]; $i < $strategyCount{$curMsgType}; $i++) {
    
    #Ignore excluded strategies
    if ($excludeStrategy{$curMsgType}[$i] == 1) {
      print PERF_LOG "#skip action $i as it's excluded\n";
      $repeat = 0;
      next;
    }
    
    print "Trying strategy $strategyList{$curMsgType}[$i]...\n";
    
    #Load strategy into NS-3 malproxy
    my $command = "C $learnedStrategyString,$strategyList{$curMsgType}[$i]";
    Utils::logTime("command $command");
    Utils::directTopology($command);
    Utils::logTime("end do gatling");
    
    #Load snapshot and continue until window expires
    Utils::killVMs(); 
    Utils::reloadNS3(); 
    Utils::restoreVMs(-1); 
    Utils::resumeNS3();
    if ($repeat==1) {
		#We're retrying this because we got a bad perf score the first time.
		#Restart the client in case it crashed.
		GatlingConfig::runClient();
    }
    my $exclude = 0;
    print "Running for $GatlingConfig::window_size seconds...\n";
    sleep $GatlingConfig::window_size;
    
    #Freeze system
    Utils::pauseVMs();
    Utils::freezeNS3();
    Utils::logTime("start do gatling");
    
    #Examine Performance score
    my $curPerf = Utils::getPerfScore();
    $perfScore{$curMsgType}[$i] = $curPerf;
    
    #Invalid performance score (may indicate something's broken)
    if (0 and $curPerf == $GatlingConfig::brokenPerf) {
      if ($repeat == 0) { 
      	#Try again
        $repeat = 1;
        print PERF_LOG "#repeating action $i to make sure for perf $curPerf\n";
        print "Bad Perf Score. Restarting clients and retrying...\n";
        $i = $i - 1;
        next;
      } else { 
      	#Well, that didn't help. Record and exclude this action.
        print PERF_LOG "$curMsgName $curMsgType $i $perfScore{$curMsgType}[$i] Exclude $strategyList{$curMsgType}[$i] \n";
        excludeAction($curMsgType, $i, 1, $curMsgName);
        $repeat = 0;
        $exclude = 1;
        Utils::logTime("Exclude: $curMsgName $actionIndex");
        
        if ($GatlingConfig::exit_when_break) {
          $GatlingConfig::watch_turret = 0;
          exit;
        }
      }
    } 
    
    #If this action produces a worse score than our current worst...
    if ($curPerf > $worstScore && $curPerf != $GatlingConfig::brokenPerf) {
      $worstScore = $curPerf;
      $actionIndex = $i;
      Utils::logTime("Select new: $curMsgName $actionIndex");
      print "Selecting action $i ($strategyList{$curMsgType}[$i]) for message $curMsgName\n";
    }
    $repeat = 0;

	#Log perf score
    print "=== perfScore $i: $perfScore{$curMsgType}[$i] for strategy $strategyList{$curMsgType}[$i] ===\n";
    Utils::resetPerfScore();
    if ($exclude != 1) {
      print PERF_LOG "$curMsgName $curMsgType $i $perfScore{$curMsgType}[$i] $strategyList{$curMsgType}[$i]\n";
    }
  }
  
  
	#Dump all perf scores if debugging
	for (my $i = 0; $GatlingConfig::debug > 0 && $i < $strategyCount{$curMsgType}; $i++) {
		print "perfScore $i: $perfScore{$curMsgType}[$i] for strategy: $strategyList{$curMsgType}[$i]\n";
	}

	#Choose strategy
	my $chosen = "$strategyList{$curMsgType}[$actionIndex]";
	print "Chosen strategy: $chosen\n";
	print PERF_LOG "$curMsgName $curMsgType $actionIndex $perfScore{$curMsgType}[$actionIndex] Chosen\n";
  	Utils::logTime("Chosen: $curMsgType $chosen");
	chooseAction($curMsgType, $actionIndex, 1, $curMsgName);
	
	#Reset strategy
	$curActionIndex[$curMsgType] = 0;
    Utils::logTime("command C $learnedStrategyString");
    Utils::directTopology("C $learnedStrategyString");

  #Reload system
  Utils::logTime("end do gatling");
  Utils::killVMs(); # restore from checkpoint
  Utils::reloadNS3(); # we will back to the original status
  Utils::restoreVMs(-1); # restore from checkpoint
  Utils::resumeNS3();
  Utils::logTime("start do gatling");
  #print "Message Type: $curMsgType $Strategy::msgName[$curMsgType] $learned[$curMsgType]\n";
  $passone = 1;
  return;
}

my $socket;


sub start_Listener {
## LISTENER - malproxy will talk to this module
## Entry point to begin an attack

  #Open Socket for NS-3 malproxy responses
  $Attack::socket = new IO::Socket::INET (
      LocalHost => '127.0.0.1',
      LocalPort => '8888',
      Proto => 'udp',
      ) or die "Error in Listener Socket Creation : $!\n";
  print "Controller listens on port 8888\n";

  #Load Currently Learned strategies into the NS-3 malproxy
  for (my $i = 0; $i <= $messageCount; $i++) {
		if ($learned[$i] >= 0) {
      Utils::directTopology("C Learned $strategyList{$i}[$learned[$i]]");
    }
  }
  Utils::directTopology("C Gatling Pause");
   
## TODO: add options to change system

  print "Starting System\n";
  GatlingConfig::runSystem();
  sleep (1);
  GatlingConfig::runClient();
  Utils::logTime("system start");
  
  sleep ($GatlingConfig::start_attack);
  print "Start Attack\n";
  Utils::directTopology("C Gatling Resume"); 
  
  #Main Loop
  while ($GatlingConfig::watch_ns3) {
  	
  	#Wait for request from NS-3 malproxy
    my $line;
    print "Waiting to listen\n";
    $Attack::socket->recv($line, 1024);
    Utils::logTime("start do gatling");
    print "Got query from malproxy\n";
    Utils::directTopology("C Gatling Pause"); 
    my $com = $line;
    if ($GatlingConfig::debug > 1) {
    	print "Rcvd from NS-3 Malproxy: $com\n";
    }
    Utils::logTime("message asked");
    
    #Determine action
    Attack::decisionMaker($com);
    
    #Resume
    Utils::directTopology("C Gatling Resume"); 
    Utils::logTime("end do gatling");
  }
  
  #Exiting...
  close $Attack::socket;
  print "Listener stopped\n";
}

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

sub start {
  prepareMessages();
  my $perfListener = threads->create('perf_collection_thread','');
  $perfListener->detach();

  print "Starting NS-3\n";
  my $ns3_thread;
  if ($GatlingConfig::startNS3 == 1) {
    $GatlingConfig::watch_ns3 = 1;
    $ns3_thread = threads->create('ns3_thread', "./run_command.sh \"malproxy_simple $GatlingConfig::mal -num_vms $GatlingConfig::num_vms -ip_base 10.1.2 -tap_base tap-ns $GatlingConfig::watchPort -runtime $GatlingConfig::runTime\"");  
  }
  
  #Verify that VMs and NS-3 are up
  sleep 3;
  system("pssh -P -h $GatlingConfig::allList -t 5 \"ping 10.1.2.5 -c 5\"");

  if ($GatlingConfig::watch_ns3 == 0) {
    print "Gatling suspended due to NS3 termination\n";
    exit;
  }
  
  #Begin Attack!
  my $gatling_thread = threads->create('GreedyAttack::start_Listener');
  while ($GatlingConfig::watch_ns3 == 1 && $GatlingConfig::watch_turret) {
    sleep 1;
  }
  
  #Sombody died...
  if ($GatlingConfig::watch_ns3 == 0) {
    $gatling_thread->kill('SIGTERM');
  } else {
    $ns3_thread->kill('SIGTERM');
  }
}

1;