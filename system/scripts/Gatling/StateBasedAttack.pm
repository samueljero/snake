#!/usr/bin/perl

package StateBasedAttack;
require MsgParse;
require Utils;
require GatlingConfig;
use Cwd;


#Attack topology/connection details
my $serverip = "10.1.2.3";
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

my %strategyCount;
my %strategyList;
my %excludeStrategy;    # actions to exclude
my %perfScore;          # latest performance recorded
my @WaitingStrategyList;
my @FinishedStrategyList;
my @FinishedPerfScore;
my @FinishedResourceUsage;

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
	 
	for(my $i=0; $i < $messageCount; $i++){
		if(scalar keys (%{$db{"client"}{"pkt_cnt_$MsgParse::msgName[$i]"}})>0){
			for(my $j=1; $j < $strategyCount{$i};$j++){
				push(@WaitingStrategyList,$strategyList{$i}[$j]);
				print NEW_LEARNED "TRY $strategyList{$i}[$j]\n";
			}
		}elsif(scalar keys (%{$db{"server"}{"pkt_cnt_$MsgParse::msgName[$i]"}})>0){
			for(my $j=1; $j < $strategyCount{$i};$j++){
				push(@WaitingStrategyList,$strategyList{$i}[$j]);
				print NEW_LEARNED "TRY $strategyList{$i}[$j]\n";
			}
		}else{
			push(@WaitingStrategyList,$strategyList{$i}[8]); #INJECT
			push(@WaitingStrategyList,$strategyList{$i}[9]); #WINDOW
			print NEW_LEARNED "TRY $strategyList{$i}[8]\n";
			print NEW_LEARNED "TRY $strategyList{$i}[9]\n";
		}
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
		my @strategyListForMessage;
		my @score;
		my @selected;
		my @excluded;
		my $strategyForMessage = 0;

		#Add NONE, Drop, Dup, Delay, Divert commands for this message
		$strategyListForMessage[0] = "*?*?$MsgParse::msgName[$i] NONE 0";
		$strategyListForMessage[1] = "*?*?$MsgParse::msgName[$i] DROP 100";
		$strategyListForMessage[1] = "*?*?$MsgParse::msgName[$i] DROP 50";
		$strategyListForMessage[2] = "*?*?$MsgParse::msgName[$i] DUP 10";
		$strategyListForMessage[3] = "*?*?$MsgParse::msgName[$i] DELAY 1.0";
		$strategyListForMessage[4] = "*?*?$MsgParse::msgName[$i] DIVERT 1.0";
		$strategyListForMessage[5] = "*?*?$MsgParse::msgName[$i] REPLAY 1";
		$strategyListForMessage[6] = "*?*?$MsgParse::msgName[$i] BURST 1.0";
		$strategyListForMessage[7] = "*?*?$MsgParse::msgName[$i] BURST 2.0";
		$strategyListForMessage[8] = "*?*?$MsgParse::msgName[$i] INJECT t=5 0 $clientip $serverip 0=$clientport 1=$serverport 2=111 5=5";
		$strategyListForMessage[9] = "*?*?$MsgParse::msgName[$i] WINDOW w=$defaultwindow t=5 $clientip $serverip $clientport $serverport 5";
		for ( my $j = 0 ; $j < 10 ; $j++ ) {   # NONE, DROP 100, DROP 40, DUP 10, etc
			push( @score,    9999 );
			push( @selected, 0 );
			push( @excluded, 0 );
			$strategyForMessage = $strategyForMessage + 1;
		}

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
					push( @strategyListForMessage,"*?*?$MsgParse::msgName[$i] LIE $FlenList->{$msgFlenList->{$i}[$j]}[$k] $j");
					push( @score,    9999 );
					push( @selected, 0 );
					push( @excluded, 0 );
					$strategyForMessage = $strategyForMessage + 1;
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
					push( @strategyListForMessage,"*?*?$MsgParse::msgName[$i] LIE $typeStrategyListRef->{$fieldsPerMsgRef->{$i}[$j]}[$k] $j");
					push( @score,    9999 );
					push( @selected, 0 );
					push( @excluded, 0 );
					$strategyForMessage = $strategyForMessage + 1;
				}
			}
		}

		#Final totals
		$strategyCount{$i}   = $strategyForMessage;
		$strategyList{$i}    = \@strategyListForMessage;
		$excludeStrategy{$i} = \@excluded;
		$perfScore{$i}       = \@score;
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
			push(@FinishedStrategyList, $res);
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
	Utils::updateSnapshot(-1);
	Utils::pauseVMs();
	Utils::snapshotVMs();
	my $i;
	
	if(not @WaitingStrategyList){
		#Start with a benign execution
		push(@WaitingStrategyList,"*?*?BaseMessage NONE 0");
		$i=0;
	}else{
		$i=scalar @WaitingStrategyList;
	}
	
	while(@WaitingStrategyList){
		my $strategy=shift(@WaitingStrategyList);
		
		#Setup VMs/NS-3	
		Utils::killVMs();
		sleep(1);
		Utils::restoreVMs(-1);
		$GatlingConfig::watch_ns3=1;
		$ns3_thread = threads->create( 'ns3_thread',
	"./run_command.sh \"malproxy_simple $GatlingConfig::mal -num_vms $GatlingConfig::num_vms -ip_base 10.1.2 -tap_base tap-ns $GatlingConfig::watchPort -runtime $GatlingConfig::runTime\""
		);
		sleep ($GatlingConfig::start_attack);
		if($GatlingConfig::watch_ns3==0){
			print "PANIC: NS-3 didn't start!!\n";
			exit;
		}
		
		#Load Strategy
		print "Trying strategy $strategy...\n";
		my $fullpath = Cwd::abs_path($GatlingConfig::statediagramFile);
		my $command = "C GatlingLoadStateDiagram $fullpath";
		Utils::logTime("command $command");
		Utils::directTopology($command);
		$command = "C $strategy";
		Utils::logTime("command $command");
		Utils::directTopology($command);
		Utils::directTopology("C Gatling Resume");
		
		#Start clients
		print "Starting System\n";
		GatlingConfig::runSystem();
		GatlingConfig::runClient();
		Utils::logTime("system start");
		
		
		#Wait for NS-3
		sleep($GatlingConfig::runTime*(2/3.0));
		
		#Measure State Information
		$command = "C GatlingSendStateStats";
		Utils::logTime("command $command");
		my $statstr=Utils::directTopology($command);
		my %db=Utils::makeMetricDB($statstr);
		
		sleep($GatlingConfig::runTime*(1/3.0));
		while($GatlingConfig::watch_ns3 !=0){
			sleep(1);
		}

		#Measure perf
		
		my $perfscore = Utils::getPerfScore();
		Utils::resetPerfScore();
		GatlingConfig::prepare();

		#Measure Resource Utilization
		my $resourceusage = Utils::getNumConnections("root\@$serverip");
		
		#Save results
		push(@FinishedStrategyList, $strategy);
		push(@FinishedPerfScore, $perfscore);
		push(@FinishedResourceUsage, $resourceusage);

		#Print results
		print "===perfScore $i: $perfscore for strategy $strategy used $resourceusage\n";
		print PERF_LOG "$i,$perfscore,$strategy,$resourceusage\n";

		#Join NS-3 Thread (so it goes away)
		$ns3_thread->join();

		#Debug State Results
		foreach $host (keys %db){
			foreach $metric (keys %{$db{$host}}){
				foreach $state (keys %{$db{$host}{$metric}}){
					print "$host,$metric,$state,$db{$host}{$metric}{$state}\n";
				}
			}
		}
		
		#Update Strategies
		if($i==0){
			#First execution (benign)
			CreateStrategyList($strategy,$perfscore,$resourceusage,%db);
		}else{
			#Any other execution
		}
		
		$i++;
	}
}

1;
