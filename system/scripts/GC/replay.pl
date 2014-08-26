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
my $fieldRef          = Strategy::getFlenList();
my $coarseStrategyRef = Strategy::getCoarseStrategyList();
my $fineStrategyRef   = Strategy::getFineStrategyList();
my @strategyList;
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
		print "$line\n";
		$| = 1;
	}
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
	my $res;

	#Load Saved Performance Info
	$res = open PRE_PERF, "<", $GatlingConfig::prePerf;
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
	}else{
		die "No pre perf file!";
	}

	if($benign_calc_num > 0){
		$benign_val = $benign_calc_total / $benign_calc_num;
	}


	#Open Perf Log
	open PERF_LOG, "+>>", $GatlingConfig::perfMeasured
	  or die "Can't create $GatlingConfig::perfMeasured $!";
	print PERF_LOG "#Sequence,Performance,Strategy,Resources\n";

	#Open decision log
	open LEARNED_LOG, "+>>", $GatlingConfig::newlyLearned
	  or die "Can't create $GatlingConfig::newlyLearned $!";
}

sub analyze {
	my $perfStr = shift;
	my $feedback = @_;
	my $strat;

	# Analyze our perf score, if we got one
	if ( length($perfStr) > 0 ) {
		chomp($perfStr);
		my @token = split(':', $perfStr);
		if ($#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
			#skip
		} else {
			my $perf     = $token[2];
			my $strategy = $token[1];
			my $res      = $token[3];
			my $score    = Utils::computeAttackScore( $perf, $res );

			#Save Perf Score
			print PERF_LOG "$strategy,$perf,$res,$benign_val\n";

			#Check for Attack
			if ($benign_val > 0 and isAttack($score, $benign_val)) {
				print LEARNED_LOG "FOUND $strategy,$perf,$res,$benign_val\n";
			}

			if ($strategy =~ / NONE /) {
				$benign_calc_total += Utils::computeAttackScore( $perf, $res );
				$benign_calc_num += 1;
				$benign_val = $benign_calc_total / $benign_calc_num;
			}
		}
	}
}

sub main{
	my @strat;
	my @feedback;
	my $perfInput;

	#Prime System
	initSystem();
	outputStrat(@strategyList);

	#Main loop
	$perfInput ="";
	while (<STDIN>) {
		my $line = $_;
		chomp($line);
		DEBUGPRINT("got from stdin: $line");
		if ( $line eq "info:end" ) {
			@strat = analyze($perfInput, @feedback);
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
}

main();
