#!/usr/bin/perl

package Attack;
require MsgParse;
require Utils;
require GatlingConfig;

my $fieldsPerMsgRef = MsgParse::parseMessage();
my $msgNameRef = $MsgParse::msgNameRef;
my $passone = 0;

my $messageCount = MsgParse::getMsgNameClount();
my $msgTypeRef = $MsgParse::msgType;
my $typeStrategyRef = MsgParse::getStrategyCount();
my $typeStrategyListRef = MsgParse::getTypeStrategyList();

print "Total $messageCount messages\n";

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
  close $Attack::socket;
  print "socket closed\n";
  $GatlingConfig::watch_ns3 = 2;
  exit;
}

sub prepareMessages {
# prepare perfLog
  open PERF_LOG, "+>", $GatlingConfig::perfMeasured or die "Can't create $GatlingConfig::perfMeasured $!";
  print PERF_LOG "# MsgName MsgType action Index perfValue (Chosen)\n";

# prepare decision log
  open NEW_LEARNED, "+>", $GatlingConfig::newlyLearned or die "Can't create $GatlingConfig::newlyLearned $!";
# prepare strategy for message types
  for (my $i = 0; $i <= $messageCount; $i++) {
    if ($GatlingConfig::debug > 1) {
      my $nameStr = $MsgParse::msgName[$i];
      print "$i] - $nameStr - $MsgParse::msgType[$i] $MsgParse::msgTypeList{$nameStr}\n";
    }
# Message delivery attacks
    my @strategyListForMessage;
    my @score;
    my @selected;
    my @excluded;
    my $strategyForMessage = 0;
    $learned[$i] = -1;
    $strategyListForMessage[0]= "$MsgParse::msgName[$i] NONE 0";
    $strategyListForMessage[1]= "$MsgParse::msgName[$i] DROP 100";
    $strategyListForMessage[1]= "$MsgParse::msgName[$i] DROP 50";
    $strategyListForMessage[2]= "$MsgParse::msgName[$i] DUP 10";
    $strategyListForMessage[3]= "$MsgParse::msgName[$i] DELAY 1.0";
    $strategyListForMessage[4]= "$MsgParse::msgName[$i] DIVERT 1.0";
    for (my $j = 0; $j < 5; $j++) { # dup, delay, divert, benign, drop
      push(@score, 9999);
      push(@selected, 0);
      push(@excluded, 0);
      $strategyForMessage = $strategyForMessage+1;
    }
    for (my $j = 0; $j  <= $#{$fieldsPerMsgRef->{$i}}; $j++) {
      #$strategyForMessage = $strategyForMessage + $typeStrategyRef->{$fieldsPerMsgRef->{$i}[$j]};
      if ($typeStrategyRef->{$fieldsPerMsgRef->{$i}[$j]} > 0) {
        for (my $k = 0; $k <= $typeStrategyRef->{$fieldsPerMsgRef->{$i}[$j]}; $k++) {
          push(@strategyListForMessage, "$MsgParse::msgName[$i] LIE $typeStrategyListRef->{$fieldsPerMsgRef->{$i}[$j]}[$k] $j");
          push(@score, 9999);
          push(@selected, 0);
          push(@excluded, 0); 
          $strategyForMessage = $strategyForMessage+1;
        }
      }
    }
    $strategyCount{$i} = $strategyForMessage;
    $strategyList{$i} = \@strategyListForMessage;
    $curActionIndex[$i] = 0;
    $tally{$i} = \@selected;
    $excludeStrategy{$i} = \@excluded;
    $perfScore{$i} = \@score;
  }

## STRATEGIES LEARNED BEFORE THIS EXPERIMENT
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
      
      my $msgType = $MsgParse::msgTypeList{$name};
      for (my $i = 0; $i <= $messageCount; $i++) {
      	if($MsgParse::msgName[$i] eq $name){
      		$msgType=$i;
      	}
      }	
      my $strategy = join (' ', @token);
      print "PRE LEARNED $msgType for $name strategy $strategy\n";
      my $actionIndex = 0;
      FIND_STRATEGY:
      for (my $i = 0; $i < $strategyCount{$msgType}; $i++) {
        if ($strategy eq $strategyList{$msgType}[$i]) {
          $actionIndex = $i;
          print "FOUND $actionIndex = $i for $strategy = $strategyList{$msgType}[$i]\n";
          last FIND_STRATEGY;
        }
      }
      $learned[$msgType] = $actionIndex;
      $strategyList{$msgType}[$actionIndex] = $strategy;
      learnAction($msgType, $actionIndex, $strategy, 0);
    }
    close PRELEARNED;
    if ($GatlingConfig::debug > 0) {
      for (my $i = 0; $i <= $messageCount; $i++) {
        if ($learned[$i] >= 0) {
          my $learnedStrategy = $learned[$i];
          print "Learned Msg $i strategy # [$learnedStrategy]";
        }
      }
      print "All learned: $learnedStrategyString\n";
    }
  }

## LOAD PREFORMANCE MEASURED IN PREV RUN (IF ANY)
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
    print "LOADING PERF MEASURED FROM PREV RUN\n";
    close PRE_PERF;
  }

  if ($GatlingConfig::debug > 1) {
    for (my $i = 0; $i <= $messageCount; $i++) {
      for (my $j =0 ; $j < $strategyCount{$i}; $j++) {
        print "For Msg $MsgParse::msgName[$i] strategy $j tally: $tally{$i}[$j], ";
        print " perf: $perfScore{$i}[$j]";
        print " Exclude: $excludeStrategy{$i}[$j]\n";
      }
      print "Index for $MsgParse::msgName[$i] : $curActionIndex[$i]\n"
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
  my $now = shift;
  $learned[$curMsgType] = $actionIndex;
  my $curMsgName=$MsgParse::msgName[$curMsgType];
  $learnedStrategyString = $learnedStrategyString.$strategy." ";
  print "LEARNED FOR MSG $curMsgType action $actionIndex\n";
  if ($now == 1) {
    Utils::directTopology("C Learned $learnedStrategyString");
    print "LEARNED FOR MSG $curMsgType action $actionIndex\n";
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
	} else {
    if ($now == 1) {
      #print "Commanding Once for $curMsgName\n";
      #Utils::directTopology("C Once $strategyList{$curMsgType}[$actionIndex]");
    }
  }
}

sub decisionMaker
{
	my $curMsgType=shift;
	print "decisionMaker for MsgType [$curMsgType]\n";

	if ($curMsgType < 0 || $curMsgType > $messageCount) { # message not interested in
    print PERF_LOG "#Not interested for $curMsgType\n";
		return;
	}
	if ($learned[$curMsgType] >= 0) {
		#my $learnedStrategy = $learned[$curMsgType];
		#print NEW_LEARNED "$strategyList{$curMsgType}[$learned[$curMsgType]]\n";
		Utils::directTopology("C Learned $learnedStrategyString");
		print "Resend learned $learnedStrategyString\n";
    return;
	}
	
	Utils::logTime("end do gatling");
  Utils::freezeNS3(); #already frozen
	Utils::pauseVMs();
	Utils::takeTime();
	Utils::updateSnapshot(-1);
	Utils::snapshotVMs();
	Utils::saveNS3();
	Utils::logTime("start do gatling");
	my $curMsgName=$MsgParse::msgName[$curMsgType];
	my $repeat = 0;
	my $worstScore = 0;
  my $actionIndex = 0;
ACTION_TEST:
  for (my $i = $curActionIndex[$curMsgType]; $i < $strategyCount{$curMsgType}; $i++) {
    if ($excludeStrategy{$curMsgType}[$i] == 1) {
      print PERF_LOG "#skip action $i as it's excluded\n";
      $repeat = 0;
      next;
    }
    my $command = "C $learnedStrategyString$strategyList{$curMsgType}[$i]";
    Utils::logTime("command $command");
    Utils::directTopology($command);
    Utils::logTime("end do gatling");
    Utils::killVMs(); 
    Utils::reloadNS3(); 
    Utils::restoreVMs(-1); 
    Utils::resumeNS3();
    my $exclude = 0;

    print "run for window $GatlingConfig::window_size\n";
    sleep $GatlingConfig::window_size;
    Utils::pauseVMs();
    Utils::freezeNS3();
    Utils::logTime("start do gatling");
    my $curPerf = Utils::getPerfScore();
    $perfScore{$curMsgType}[$i] = $curPerf;
    if ($curPerf == $GatlingConfig::brokenPerf) {
      if ($repeat == 0) { # try again
        $repeat = 1;
        print PERF_LOG "#repeating action $i to make sure for perf $curPerf\n";
        $i = $i - 1;
        GatlingConfig::runClient();
        next;
      } else { # Learn the benign and report separately
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
    if ($curPerf > $worstScore && $curPerf != $GatlingConfig::brokenPerf) {
      $worstScore = $curPerf;
      $actionIndex = $i;
      Utils::logTime("Select new: $curMsgName $actionIndex");
      print "actionIndex change: $i\n";
    }
    $repeat = 0;

    print "=== perfScore $i: $perfScore{$curMsgType}[$i] for straategy $strategyList{$curMsgType}[$i]\n";
    Utils::resetPerfScore();
    if ($exclude != 1) {
      print PERF_LOG "$curMsgName $curMsgType $i $perfScore{$curMsgType}[$i] $strategyList{$curMsgType}[$i]\n";
    }
  }
# pick the worst score and mark tally
	for (my $i = 0; $GatlingConfig::debug > 0 && $i < $strategyCount{$curMsgType}; $i++) {
		print "perfScore $i: $perfScore{$curMsgType}[$i]\n";
	}

# once decided, need to go back and follow the selection
	my $chosen = "$strategyList{$curMsgType}[$actionIndex]";
	print "Chosen: $learnedStrategyString $chosen\n";
	print PERF_LOG "$curMsgName $curMsgType $actionIndex $perfScore{$curMsgType}[$actionIndex] Chosen\n";
  Utils::logTime("Chosen: $curMsgType $chosen");
	chooseAction($curMsgType, $actionIndex, 1, $curMsgName);
  $curActionIndex[$curMsgType] = 0;
	
 Utils::logTime("end do gatling");
	Utils::killVMs(); # restore from checkpoint
  Utils::reloadNS3(); # we will back to the original status
  Utils::restoreVMs(-1); # restore from checkpoint
  Utils::resumeNS3();
  Utils::logTime("start do gatling");
	print "Message Type: $curMsgType $MsgParse::msgName[$curMsgType] $learned[$curMsgType]\n";
  $passone = 1;
	return;
}

my $socket;


sub start_Listener {
## LISTENER - malproxy will talk to this module
  $Attack::socket = new IO::Socket::INET (
      LocalHost => '127.0.0.1',
      #LocalHost => '128.10.130.184',
      LocalPort => '8888',
      Proto => 'udp',
      ) or die "Error in Listener Socket Creation : $!\n";

  # tell learned first
  for (my $i = 0; $i <= $messageCount; $i++) {
		if ($learned[$i] >= 0) {
      Utils::directTopology("C Learned $strategyList{$i}[$learned[$i]]");
      print "Commanding C Learned $strategyList{$i}[$learned[$i]]\n";
    }
  }
  print "Controller listens on port 8888\n";


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
  
  my $once = 0;
  while ($GatlingConfig::watch_ns3) {
    my $line;
    print "Waiting to listen\n";
    $Attack::socket->recv($line, 1024);
    Utils::logTime("start do gatling");
    print "Directing topology\n";
    if ($once == 1) {
      $once = 0;
      next;
    }
    Utils::directTopology("C Gatling Pause"); 
    my $com = $line;
    print "RCVD: $com\n";
    Utils::logTime("message asked");
    Attack::decisionMaker($com);
    $once = 1;
    #Utils::resumeNS3(); 
    Utils::directTopology("C Gatling Resume"); 
    Utils::logTime("end do gatling");
  }
  close $Attack::socket;
  print "Listner stopped\n";
}

1;
