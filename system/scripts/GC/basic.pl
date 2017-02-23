#!/usr/bin/env perl
########################
# Turret Attack Script
# Samuel Jero <sjero@purdue.edu>
# Hyo Lee <lee747@purdue.edu>
########################
use strict;
use POSIX;
use Switch;

my $DEBUG = 0;
my $system = "TCP";


# System Initialization
use lib ("../Gatling/");
require Strategy;
require GatlingConfig;
require Utils;
$GatlingConfig::systemname = $system;
GatlingConfig::setSystem();

# Load Message Info
my $fieldsPerMsgRef   = Strategy::parseMessage();
my $msgTypeRef        = Strategy::getMsgTypeRef();
my $msgNameRef        = Strategy::getMsgNameRes();
my $msgBitfieldRef    = Strategy::getMsgFlenList();
my $bitfieldStrategyRef = Strategy::getFlenList();
my $coarseStrategyRef = Strategy::getCoarseStrategyList();
my $fineStrategyRef   = Strategy::getFineStrategyList();
my @strategyList;
my @FeedbackList1;
my @FeedbackList2;
my @RetryList;
my $benign_calc_total;
my $benign_calc_num;
my $benign_val;


sub DEBUGPRINT {
	my $line = shift;
	if ($DEBUG) {
		print STDERR "$line\n";
	}
}


#Output new strategies to stdout
sub outputStrat {
	my @arr = @_;
	foreach my $line (@arr){
		if($line ~~ @strategyList){
			#ignore---old strategy
		} else{
			push(@strategyList, $line);
			print TESTING_LOG "TRY,$line\n";
			print "$line\n";
			$| = 1;
		}
	}

	LEARNED_LOG->autoflush(1);
	PERF_LOG->autoflush(1);
	FEEDBACK_LOG->autoflush(1);
	TESTING_LOG->autoflush(1);
	RETRY_LOG->autoflush(1);
}

sub outputStrat_nocheck {
        my @arr = @_;
        foreach my $line (@arr){
		push(@strategyList, $line);
		print TESTING_LOG "TRY,$line\n";
		print "$line\n";
		$| = 1;
        }

        LEARNED_LOG->autoflush(1);
        PERF_LOG->autoflush(1);
        FEEDBACK_LOG->autoflush(1);
        TESTING_LOG->autoflush(1);
        RETRY_LOG->autoflush(1);
}


#Decide if some perf score represents an attack
sub isAttack {
	my $val = shift;
	my $ref = shift;

	if ( $val > 1.5 * $ref ) {    #greater than 1.5 times reference
		return 1;
	}
	if ( $val < 0.5 * $ref ) {    #less than 0.5 times reference
		return 1;
	}
	return 0;
}

sub initSystem {
	$benign_calc_total = 0;
	$benign_calc_num = 0;
	$benign_val = 0;
	my @curr;
	my $res;

	#Load Saved Performance Info
	$res = open PRE_PERF, "<", $GatlingConfig::perfMeasured;
	if ($res) {
		while ( my $line = <PRE_PERF> ) {
			chomp($line);
			my @token = split( ',', $line );
			if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
				next;
			}
			my $perf     = $token[1];
			my $strategy = $token[0];
			my $res      = $token[2];
			push(@strategyList, $strategy);

			if ($strategy =~ / NONE /) {
				$benign_calc_total += Utils::computeAttackScore( $perf, $res );
				$benign_calc_num += 1;
			}
		}
		close PRE_PERF;
	}
	if($benign_calc_num > 0){
		$benign_val = $benign_calc_total / $benign_calc_num;
	}


	#Load Saved feedback info
	$res = open PRE_FEEDBACK, "<", "$GatlingConfig::state_dir/feedback.txt";
	if ($res) {
		while ( my $line = <PRE_FEEDBACK> ) {
			chomp($line);
			my @token = split(',', $line );
			if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
				next;
			}
			my $type    = $token[0];
			my $val	    = $token[1];
			if($type eq "1"){
				push(@FeedbackList1, $val);
			}elsif($type eq "2"){
				push(@FeedbackList2, $val);
			}
		}
		close PRE_FEEDBACK;
	}

        #Load Saved retry info
        $res = open PRE_RETRY, "<", "$GatlingConfig::state_dir/retry.txt";
        if ($res) {
                while ( my $line = <PRE_RETRY> ) {
                        chomp($line);
                        my @token = split(',', $line );
                        if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
                                next;
                        }
                        my $strat    = $token[0];
                        my $val     = $token[1];
			push(@RetryList, $strat);
                }
                close PRE_RETRY;
        }

	#Load in progess attacks
	$res = open PRE_LEARNED, "<", "$GatlingConfig::state_dir/testing.txt";
	if ($res) {
		while ( my $line = <PRE_LEARNED> ) {
			chomp($line);
			if($line =~ /TRY/){
				my @token = split(',', $line );
				my $type    = $token[0];
				my $val	    = $token[1];
				if($val ~~ @strategyList){
					#ignore
				}else{
					push(@curr, $val);
				}
			}
		}
		close PRE_LEARNED;
	}

	#Open Perf Log
	open PERF_LOG, "+>>", $GatlingConfig::perfMeasured
	  or die "Can't create $GatlingConfig::perfMeasured $!";
	print PERF_LOG "#Strategy,Performance,Resources,Threshold\n";

	#Open decision log
	open LEARNED_LOG, "+>>", $GatlingConfig::newlyLearned
	  or die "Can't create $GatlingConfig::newlyLearned $!";
	
	#Open testing log
	open TESTING_LOG, "+>>", "$GatlingConfig::state_dir/testing.txt"
	  or die "Can't create $GatlingConfig::state_dir/testing.txt $!";

	#Open feedback log
	open FEEDBACK_LOG, "+>>", "$GatlingConfig::state_dir/feedback.txt"
	  or die "Can't create feedback.txt $!";

        #Open retry log
        open RETRY_LOG, "+>>", "$GatlingConfig::state_dir/retry.txt"
          or die "Can't create retry.txt $!";

	return @curr;	
}

#Initial Strategies to start with
sub primeSystem {
	my @strat;

	#Prime with Benign strategies
	foreach my $msgType (@$msgNameRef) {
		my $prefix = "*?*?$msgType";
		if ( $msgType eq "BaseMessage" ) {
			push( @strat, "$prefix NONE 0" );
			push( @strat, "$prefix NONE 1" );
			push( @strat, "$prefix NONE 2" );
		}
	}

	#Prime with other strategies
	foreach my $msgType (@$msgNameRef) {
		my $prefix = "*?*?$msgType";
		if ( $msgType ne "BaseMessage" ) {
			if ( $GatlingConfig::systemname eq "DCCP" ) {
				my $size = 0;
				if ( $msgType eq "BaseMessage" ) {
					$size = 3;
				}
				elsif ( $msgType eq "Data" ) {
					$size = 4;
				}
				elsif ( $msgType eq "Request" ) {
					$size = 5;
				}
				elsif ( $msgType eq "Reset" ) {
					$size = 7;
				}
				elsif ( $msgType eq "Response" ) {
					$size = 7;
				} 
				else {
					$size = 6;
				}
				push( @strat,
"$prefix INJECT t=10 0 $GatlingConfig::clientip $GatlingConfig::serverip 0=$GatlingConfig::clientport 1=$GatlingConfig::serverport 2=$size 6=1 11=111"
				);
				push( @strat,
"$prefix INJECT t=10 0 $GatlingConfig::serverip  $GatlingConfig::clientip 0=$GatlingConfig::serverport 1=$GatlingConfig::clientport 2=$size 6=1 11=111"
				);
				push( @strat,
"$prefix WINDOW w=$GatlingConfig::defaultwindow t=10 $GatlingConfig::clientip $GatlingConfig::serverip 0=$GatlingConfig::clientport 1=$GatlingConfig::serverport 2=$size 6=1"
				);
				push( @strat,
"$prefix WINDOW w=$GatlingConfig::defaultwindow t=10 $GatlingConfig::serverip $GatlingConfig::clientip 0=$GatlingConfig::serverport 1=$GatlingConfig::clientport 2=$size 6=1"
				);
			}
			elsif ( $GatlingConfig::systemname eq "TCP" ) {
				push( @strat,
"$prefix INJECT t=10 0 $GatlingConfig::clientip $GatlingConfig::serverip 0=$GatlingConfig::clientport 1=$GatlingConfig::serverport 2=111 5=5 10=$GatlingConfig::defaultwindow"
				);
				push( @strat,
"$prefix INJECT t=10 0 $GatlingConfig::serverip  $GatlingConfig::clientip 0=$GatlingConfig::serverport 1=$GatlingConfig::clientport 2=111 5=5 10=$GatlingConfig::defaultwindow"
				);
				push( @strat,
"$prefix WINDOW w=$GatlingConfig::defaultwindow t=10 $GatlingConfig::clientip $GatlingConfig::serverip 0=$GatlingConfig::clientport 1=$GatlingConfig::serverport 5=5"
				);
				push( @strat,
"$prefix WINDOW w=$GatlingConfig::defaultwindow t=10 $GatlingConfig::serverip $GatlingConfig::clientip 0=$GatlingConfig::serverport 1=$GatlingConfig::clientport 5=5"
				);
			}
		}

	}
	#if ( $GatlingConfig::systemname eq "TCP" ) {
	#	push( @strat,
	#	"LISTEN?*?Syn REPLAY 1,LISTEN?*?Syn LIE =$GatlingConfig::serverport 0,LISTEN?*?Syn LIE =$GatlingConfig::clientport 1"
	#	);
	#}
	return @strat;
}

sub analyze {
	my @feedback = @_;
	my $perfStr = shift(@feedback);
	my @strat;

	# Analyze our perf score, if we got one
	if ( length($perfStr) > 0 ) {
		chomp($perfStr);
		my @token = split(':', $perfStr);
		if ($#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
			#skip
		} else {
			my $perf     = $token[1];
			my $strategy = $token[0];
			my $res      = $token[2];
			my $score    = Utils::computeAttackScore( $perf, $res );

			#Save Perf Score
			print PERF_LOG "$strategy,$perf,$res,$benign_val\n";

			if ($perf < 15) {
				push(@RetryList, $strategy);
				print RETRY_LOG "$strategy,$perf,$res\n";
				outputStrat_nocheck($strategy);
				return @strat;
			}

			#Check for Attack
			if ($benign_val > 0 and isAttack($score, $benign_val)) {
				if ($strategy ~~ @RetryList) {
					print LEARNED_LOG "FOUND,$strategy,$perf,$res,$benign_val\n";
				} else {
					push(@RetryList, $strategy);
					print RETRY_LOG "$strategy, $perf, $res\n";
					outputStrat_nocheck($strategy);
				}
			}

			if ($strategy =~ / NONE /) {
				$benign_calc_total += Utils::computeAttackScore( $perf, $res );
				$benign_calc_num += 1;
				$benign_val = $benign_calc_total / $benign_calc_num;
			}
		}
	}

	#Examine feedback and suggest strategies
	foreach my $line (@feedback) {
		# pkt_cnt_Ack,client,CLOSED,3768,1
		my @tokens = split( /,/, $line );
		my $metric = $tokens[0];
		my $side   = $tokens[1];
		my $key    = $tokens[2];
		my $value  = $tokens[3];
	        
		if ($side eq "server") {
		    $key .= "?0";
		} elsif ($side eq "client") {
		    $key .= "?1";
		} else {
		    $key .= "?*";
		}

		if ($metric =~ "pkt_cnt_") {
			my $msg = substr $metric, 8;
			my @tmp = strategyCompose($msg, $key);
			push(@strat, @tmp);
		}
	}
	return @strat;
}

sub strategyCompose {
	my $msg   = shift;
	my $state = shift;
	my $level = 0;
	my @strat;

	if( "$state?$msg" ~~ @FeedbackList2){
		$level = 2;
	}elsif( "$state?$msg" ~~ @FeedbackList1){
		push(@FeedbackList2, "$state?$msg");
		print FEEDBACK_LOG "2,$state?$msg\n";
		$level = 1;
	}else{
		push(@FeedbackList1, "$state?$msg");
		print FEEDBACK_LOG "1,$state?$msg\n";
		$level = 0;
	}

	switch ($level) {
		case (0) {
			DEBUGPRINT("coarse strategy for $msg,$state");
			my $prefix = "$state?$msg";

			my $msgType  = $msgTypeRef->{$msg};
			my $fields   = $fieldsPerMsgRef->{$msgType};
			my $fieldIdx = 0;

			# delivery actions
			push( @strat, "$prefix DROP 50" );
			push( @strat, "$prefix DROP 100" );
			push( @strat, "$prefix DELAY 1.0" );
			push( @strat, "$prefix DUP 10" );

			# coase lying actions
			foreach my $field ( @{$fields} ) {
				if($msgBitfieldRef->{$msgType}[$fieldIdx] > 0){
					my $coarseStr = $bitfieldStrategyRef->{$msgBitfieldRef->{$msgType}[$fieldIdx]};
					foreach my $str ( @{$coarseStr} ){
						push( @strat, "$prefix LIE $str $fieldIdx" );
					}
				}else{
					my $coarseStr = $coarseStrategyRef->{$field};
					foreach my $str ( @{$coarseStr} ) {
						push( @strat, "$prefix LIE $str $fieldIdx" );
					}
				}
				$fieldIdx++;
			}
		}
		case (1) {
			DEBUGPRINT("fine strategy for $msg,$state");
			my $prefix = "$state?$msg";
			push( @strat, "$prefix BURST 1.0" );

			my $msgType  = $msgTypeRef->{$msg};
			my $fields   = $fieldsPerMsgRef->{$msgType};
			my $fieldIdx = 0;

			# coase lying actions
			foreach my $field ( @{$fields} ) {
				if($msgBitfieldRef->{$msgType}[$fieldIdx] == 0){
					my $fineStr = $fineStrategyRef->{$field};
					foreach my $str ( @{$fineStr} ) {
						push( @strat, "$prefix LIE $str $fieldIdx" );
					}
				}
				$fieldIdx++;
			}
		}

		# default
		{
			DEBUGPRINT("already exercised $msg,$state");
		}
	}
	return @strat;
}

sub main{
	my @strat;
	my @feedback;
	my $perfInput;

	#Prime System
	@strat = initSystem();
	if(scalar @strat == 0){
		@strat = primeSystem();
	}
	outputStrat(@strat);
	undef(@strat);
			


	#Main loop
	$perfInput ="";
	while (<STDIN>) {
		my $line = $_;
		chomp($line);
		DEBUGPRINT("got from stdin: $line");
		if ( $line eq "info:end" ) {
			@strat = analyze($perfInput, @feedback);
			outputStrat(@strat);
			undef(@strat);
			undef(@feedback);
			$perfInput = "";
		}
		if ( $line =~ /perf:/ ){
			$line =~ s/perf://g;
			$perfInput = $line;
		}
		if( $line =~ /info:/ ){
			$line =~ s/info://g;
			push(@feedback, $line);
		}
	}

	close PERF_LOG;
	close LEARNED_LOG;
	close FEEDBACK_LOG;
	close TESTING_LOG;
}

main();
