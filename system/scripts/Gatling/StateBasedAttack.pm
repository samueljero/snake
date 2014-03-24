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
my @FullStrategyList;
my @FullPerfScore;
my @FullResourceUsage;

$SIG{TERM} = 'term_handler';
$SIG{KILL} = 'term_handler';

sub term_handler {
	print "term_handler\n";
	$GatlingConfig::watch_ns3 = 2;
	exit;
}

sub CreateStrategyList{
	prepareMessages();
	for(my $i=0; $i < $messageCount; $i++){
		for(my $j=0; $j < $strategyCount{$i};$j++){
			push(@FullStrategyList,$strategyList{$i}[$j]);
			push(@FullperfScore, $perfScore{$i}[$j]);
			push(@FullResourceUsage, -1);
		}
	}
}

sub prepareMessages {

	#Prepare perfLog
	open PERF_LOG, "+>", $GatlingConfig::perfMeasured
	  or die "Can't create $GatlingConfig::perfMeasured $!";
	print PERF_LOG "# MsgName MsgType action Index perfValue (Chosen)\n";

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
		$strategyListForMessage[0] = "$MsgParse::msgName[$i] NONE 0";
		$strategyListForMessage[1] = "$MsgParse::msgName[$i] DROP 100";
		$strategyListForMessage[1] = "$MsgParse::msgName[$i] DROP 50";
		$strategyListForMessage[2] = "$MsgParse::msgName[$i] DUP 10";
		$strategyListForMessage[3] = "$MsgParse::msgName[$i] DELAY 1.0";
		$strategyListForMessage[4] = "$MsgParse::msgName[$i] DIVERT 1.0";
		$strategyListForMessage[5] = "$MsgParse::msgName[$i] REPLAY 1";
		$strategyListForMessage[6] = "$MsgParse::msgName[$i] BURST 1.0";
		$strategyListForMessage[7] = "$MsgParse::msgName[$i] BURST 2.0";
		$strategyListForMessage[8] = "$MsgParse::msgName[$i] INJECT t=5 0 $clientip $serverip 0=$clientport 1=$serverport 2=111 5=5";
		$strategyListForMessage[9] = "$MsgParse::msgName[$i] WINDOW w=$defaultwindow t=5 $clientip $serverip $clientport $serverport 5";
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
					push( @strategyListForMessage,"$MsgParse::msgName[$i] LIE $FlenList->{$msgFlenList->{$i}[$j]}[$k] $j");
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
					push( @strategyListForMessage,"$MsgParse::msgName[$i] LIE $typeStrategyListRef->{$fieldsPerMsgRef->{$i}[$j]}[$k] $j");
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

##LOAD PREFORMANCE MEASURED IN PREV RUN (IF ANY)
#	$res = open PRE_PERF, "<", $GatlingConfig::prePerf;
#	if ($res) {
#		while ( my $line = <PRE_PERF> ) {
#			chomp($line);
#			my @token = split( / /, $line );
#			if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
#				next;
#			}
#			my $msgName     = $token[0];
#			my $msgType     = $token[1];
#			my $actionIndex = $token[2];
#			my $perfValue   = $token[3];
#			if ( $learned[$msgType] == -1 ) {
#				if ( $#token > 3 and $token[4] eq "Chosen" ) {
#					chooseAction( $msgType, $actionIndex, 0, $msgName );
#				}
#				elsif ( $#token > 3
#					and ( $token[4] eq "Exclude" || $token[5] eq "Exclude" ) )
#				{
#					excludeAction( $msgType, $actionIndex, 0, $msgName );
#				}
#				else {
#				$perfScore{$msgType}[$actionIndex] = $perfValue;
#					$curActionIndex[$msgType] = $actionIndex + 1;
#				}
#			}
#		}
#		close PRE_PERF;
#	}
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
	CreateStrategyList();
	Utils::updateSnapshot(-1);
	Utils::pauseVMs();
	Utils::snapshotVMs();
	
	for(my $i=0; $i < $#FullStrategyList; $i++){
		
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
		print "Trying strategy $FullStrategyList[$i]...\n";
		my $fullpath = abs_path($statediagramFile);
		my $command = "C GatlingLoadStateDiagram $fullpath";
		Utils::logTime("command $command");
		Utils::directTopology($command);
		$command = "C $FullStrategyList[$i]";
		Utils::logTime("command $command");
		Utils::directTopology($command);
		Utils::directTopology("C Gatling Resume");
		
		#Start clients
		print "Starting System\n";
		GatlingConfig::runSystem();
		GatlingConfig::runClient();
		Utils::logTime("system start");
		
		
		#Wait for NS-3
		sleep($GatlingConfig::runTime);
		while($GatlingConfig::watch_ns3 !=0){
			sleep(1);
		}

		#Measure perf
		$FullperfScore[$i] = Utils::getPerfScore();
		Utils::resetPerfScore();
		GatlingConfig::prepare();

		#Measure Resource Utilization
		$FullResourceUsage[$i] = Utils::getNumConnections("root\@$serverip");

		#Print results
		print "===perfScore $i: $FullperfScore[$i] for strategy $FullStrategyList[$i] used $FullResourceUsage[$i]\n";
		print PERF_LOG "$i $FullperfScore[$i] $FullResourceUsage[$i] $FullStrategyList[$i]\n";
	}
}

1;
