#!/usr/bin/env perl

require MsgParse;
require Utils;
require GatlingConfig;
use Cwd;
use List::Util qw(max min);
use Sys::Hostname;

my $host = Sys::Hostname::hostname(); #Get hostname

#Attack topology/connection details
my $serverip = "10.1.2.3";
my $hostserverip=$serverip;
if ( $host =~ /^sound/ or $host =~ /^ocean1/ ){
	$hostserverip = "10.0.1.3";
}elsif ($host =~ /^cloud15/ ){
	$hostserverip = $serverip;
}
my $clientip = "10.1.2.2";
my $malip = "10.1.2.1";
my $clientport= 5555;
my $serverport= 80;
my $malport = 5556;
my $defaultwindow=20000;


my $fieldsPerMsgRef 	= MsgParse::parseMessage();
my $msgNameRef      	= $MsgParse::msgNameRef;
my $messageCount        = MsgParse::getMsgNameClount();
my $msgTypeRef          = $MsgParse::msgType;
my $typeStrategyRef     = MsgParse::getStrategyCount();
my $typeStrategyListRef = MsgParse::getTypeStrategyList();
my $msgFlenList         = MsgParse::getMsgFlenList();
my $FlenList            = MsgParse::getFlenList();
my $FlenNumList         = MsgParse::getFlenNumList();

print "Total $messageCount message types\n";

my %strategyMatrixCounts; # message by number of malicious actions
my %strategyMatrix;	# message by list of malicious actions

my %basicStrategyMatrix;
my %basicStrategyMatrixCounts;


my @excludeStrategy;    # Strategies to exclude
my @WaitingStrategyList; # Queue of strategies waiting to execute
my @FinishedStrategyList; # List of previously executed strategies
my @FinishedPerfScore;	# Performance scores from previously executed strategies
my @FinishedResourceUsage; # Resource usage info from previously executed strategies
my %class2states; # State classes to a list of states
my %class2packets; # State classes to a list of packet types
my $benignAttackScore; #"Attack Score" for benign run
my $numBenignRuns=3; #Number of benign runs to average together for performance threshold
my $numtrials=1; #Number of times to execute an apparent attack

$SIG{TERM} = 'term_handler';
$SIG{KILL} = 'term_handler';

sub term_handler {
	print "term_handler\n";
	$GatlingConfig::watch_ns3 = 2;
	exit;
}

sub CreateStrategyList{
	my $strategy=shift;
	my $perf=shift;
	my $res=shift;
	my %db=@_;

	#Manual class - states mapping
	$class2states{"DATATRANSFER"}= [ ("ESTAB") ];
	$class2states{"CONNECTING"}= [ ("LISTEN","SYN_RCVD","SYN_SENT")];
	$class2states{"PASIVE_CLOSING"}= [("CLOSE_WAIT", "LAST_ACK", "CLOSING","CLOSED")];
	$class2states{"ACTIVE_CLOSING"}= [("FIN_WAIT_1", "FIN_WAIT_2","TIME_WAIT", "CLOSING")];

	#Class - Packet Type mapping
	foreach $cl (keys %class2states){
		foreach $st (@{$class2states{$cl}}){
			for(my $i=0; $i < $messageCount; $i++){
				if($db{"server"}{"pkt_cnt_$MsgParse::msgName[$i]"}{"$st"}){
					my $found=$false;
					foreach $val (@{$class2packets{$cl}}){
						if ($val eq $MsgParse::msgName[$i]){
							$found=$true;
							last;
						}
					}
					if(!$found){
						push(@{$class2packets{$cl}},$MsgParse::msgName[$i]);
					}
				}
				if($db{"client"}{"pkt_cnt_$MsgParse::msgName[$i]"}{"$st"}){
					my $found=$false;
					foreach $val (@{$class2packets{$cl}}){
						if($val eq $MsgParse::msgName[$i]){
							$found=$true;
							last;
						}
					}
					if(!$found){
						push(@{$class2packets{$cl}},$MsgParse::msgName[$i]);
					}
				}
			}
		}
	}
	
	# Build  Strategy List
	foreach $cl (keys %class2packets){
		for(my $i=0; $i < $messageCount; $i++){
			if(grep /^$MsgParse::msgName[$i]$/, @{$class2packets{$cl}}){
				for(my $j=1; $j < $strategyMatrixCounts{$i}; $j++){
					if($j > 11 and  $j < 20){
						next;
					}
					foreach $st (@{$class2states{$cl}}){
						push(@WaitingStrategyList, "$st?0?$strategyMatrix{$i}[$j]");
						push(@WaitingStrategyList, "$st?1?$strategyMatrix{$i}[$j]");
					}
				}
			}
			foreach $st (@{$class2states{$cl}}){
				push(@WaitingStrategyList, "$st?*?$strategyMatrix{$i}[12]");
				push(@WaitingStrategyList, "$st?*?$strategyMatrix{$i}[13]");
				push(@WaitingStrategyList, "$st?*?$strategyMatrix{$i}[14]");
				push(@WaitingStrategyList, "$st?*?$strategyMatrix{$i}[15]");
			}
			push(@WaitingStrategyList, "*?*?$strategyMatrix{$i}[16]");
			push(@WaitingStrategyList, "*?*?$strategyMatrix{$i}[17]");
			push(@WaitingStrategyList, "*?*?$strategyMatrix{$i}[18]");
			push(@WaitingStrategyList, "*?*?$strategyMatrix{$i}[19]");
		}
	}

	#Add (other) known attacks
	my $strat="LISTEN?*?Syn REPLAY 1,LISTEN?*?Syn LIE =$serverport 0,LISTEN?*?Syn LIE =$clientport 1"; #Simlutainious Open
	push(@WaitingStrategyList, $strat);
	$strat="ESTAB?*?Ack LIE +10000 3"; #Ack Storm (approximation)
	push(@WaitingStrategyList, $strat);
	
	#Save attacks
	foreach $a (@WaitingStrategyList){
		print NEW_LEARNED "TRY $a\n";
	}
}

sub prepareMessages {

	#Prepare perfLog
	open PERF_LOG, "+>", $GatlingConfig::perfMeasured
	  or die "Can't create $GatlingConfig::perfMeasured $!";
	print PERF_LOG "#Sequence,Performance,Strategy,Resources\n";

	#Prepare decision log
	open NEW_LEARNED, "+>", $GatlingConfig::newlyLearned
	  or die "Can't create $GatlingConfig::newlyLearned $!";

	#Create data structure of all possible attacks
	for ( my $i = 0 ; $i <= $messageCount ; $i++ ) {

		#for each message
		if ( $GatlingConfig::debug > 1 ) {
			my $nameStr = $MsgParse::msgName[$i];
			print "$i] - $nameStr - $MsgParse::msgType[$i] $MsgParse::msgTypeList{$nameStr}\n";
		}
		my @messageStrategyList;
		my $numMessageStrategies = 0;

		#Add NONE, Drop, Dup, Delay, Divert commands for this message
		$messageStrategyList[0] = "$MsgParse::msgName[$i] NONE 0";
		$messageStrategyList[1] = "$MsgParse::msgName[$i] DROP 100";
		$messageStrategyList[2] = "$MsgParse::msgName[$i] DROP 50";
		$messageStrategyList[3] = "$MsgParse::msgName[$i] DUP 10";
		$messageStrategyList[4] = "$MsgParse::msgName[$i] DUP 100";
		$messageStrategyList[5] = "$MsgParse::msgName[$i] DELAY 1.0";
		$messageStrategyList[6] = "$MsgParse::msgName[$i] DELAY 5.0";
		$messageStrategyList[7] = "$MsgParse::msgName[$i] DIVERT 1.0";
		$messageStrategyList[8] = "$MsgParse::msgName[$i] REPLAY 1";
		$messageStrategyList[9] = "$MsgParse::msgName[$i] BURST 1.0";
		$messageStrategyList[10] = "$MsgParse::msgName[$i] BURST 2.0";
		$messageStrategyList[11] = "$MsgParse::msgName[$i] BURST 0.5";
		$messageStrategyList[12] = "$MsgParse::msgName[$i] INJECT t=0.01 0 $clientip $serverip 0=$clientport 1=$serverport 2=111 5=5";
		$messageStrategyList[13] = "$MsgParse::msgName[$i] INJECT t=0.01 0 $serverip  $clientip 0=$serverport 1=$clientport 2=111 5=5";
		$messageStrategyList[14] = "$MsgParse::msgName[$i] WINDOW w=$defaultwindow t=0.01 $clientip $serverip $clientport $serverport 5";
		$messageStrategyList[15] = "$MsgParse::msgName[$i] WINDOW w=$defaultwindow t=0.01 $serverip $clientip $serverport $clientport 5";
		$messageStrategyList[16] = "$MsgParse::msgName[$i] INJECT t=10 0 $clientip $serverip 0=$clientport 1=$serverport 2=111 5=5";
		$messageStrategyList[17] = "$MsgParse::msgName[$i] INJECT t=10 0 $serverip  $clientip 0=$serverport 1=$clientport 2=111 5=5";
		$messageStrategyList[18] = "$MsgParse::msgName[$i] WINDOW w=$defaultwindow t=10 $clientip $serverip $clientport $serverport 5";
		$messageStrategyList[19] = "$MsgParse::msgName[$i] WINDOW w=$defaultwindow t=10 $serverip $clientip $serverport $clientport 5";
		$numMessageStrategies = 20;

		#For each field in this message
		for ( my $j = 0 ; $j <= $#{ $fieldsPerMsgRef->{$i} } ; $j++ ) {
			if ( $msgFlenList->{$i}[$j] > 0 ) {
				#Bit fields
				#For each value it makes sense to lie on based on field type
				for (
					my $k = 0 ;
					$k <= $FlenNumList->{ $msgFlenList->{$i}[$j] } ;
					$k++
				  )
				{
					#Add this lie command for message
					#print "$MsgParse::msgName[$i] LIE $FlenList->{$msgFlenList->{$i}[$j]}[$k] $j\n";
					push( @messageStrategyList,"$MsgParse::msgName[$i] LIE $FlenList->{$msgFlenList->{$i}[$j]}[$k] $j");
					$numMessageStrategies = $numMessageStrategies + 1;
				}
			}
			elsif ( $typeStrategyRef->{ $fieldsPerMsgRef->{$i}[$j] } > 0 ) {
				#For each value it makes sense to lie on based on field type
				for (
					my $k = 0 ;
					$k <= $typeStrategyRef->{ $fieldsPerMsgRef->{$i}[$j] } ;
					$k++
				  )
				{
					#Add this lie command for message
					push( @messageStrategyList,"$MsgParse::msgName[$i] LIE $typeStrategyListRef->{$fieldsPerMsgRef->{$i}[$j]}[$k] $j");
					$numMessageStrategies = $numMessageStrategies + 1;
				}
			}
		}

		#Final totals
		$strategyMatrixCounts{$i}   = $numMessageStrategies;
		$strategyMatrix{$i}    = \@messageStrategyList;
	}

	#LOAD PREFORMANCE MEASURED IN PREV RUN (IF ANY)
	$res = open PRE_PERF, "<", $GatlingConfig::prePerf;
	if ($res) {
		while ( my $line = <PRE_PERF> ) {
			chomp($line);
			my @token = split(',', $line );
			if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
				next;
			}
			my $i    = $token[0];
			my $perf = $token[1];
			my $strategy = $token[2];
			my $res   = $token[3];
			push(@FinishedStrategyList, $strategy);
			push(@FinishedPerfScore, $perf);
			push(@FinishedResourceUsage, $res);
		}
		close PRE_PERF;
	}
	
	#LOAD STRATEGIES FROM PREV RUN (IF ANY)
	$res = open PRE_LEARNED, "<", $GatlingConfig::alreadyLearned;
	if ($res) {
		while ( my $line = <PRE_LEARNED> ) {
			chomp($line);
			my @token = split(/ /, $line );
			if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
				next;
			}
			my $type    = $token[0];
			shift @token;
			my $strategy = join(" ", @token);
			if($type eq "TRY"){
				if(grep {$_ eq $strategy} @FinishedStrategyList){
					#Already tried
				}else{
					push(@WaitingStrategyList, $strategy);
				}
			}
		}
		close PRE_PERF;
	}
}

# should think where this should be
sub isAttack
{
	my $val=shift;
	my $ref=shift;

	if($val > 1.5*$ref){ #greater than 1.5 times reference
		return 1;
	}
	if($val < 0.5*$ref){ #less than 0.5 times reference
		return 1;
	}

	return 0;
}

# Main loop
# pick strategies from global strategy map and distribute startegies to executors

sub distributor {
    # while global strategy list is not empty
    
}

# prepare message
# load from global log 

while (1) {
    
}
