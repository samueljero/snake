#!/usr/bin/env perl

########################
# STATELESS ATTACKS
########################

use strict;
use POSIX;
use Switch;

my $system = "TCP";

######################
# LOAD MSG TYPES
######################

use lib ("../Gatling/");
require Strategy;
require GatlingConfig;
require Utils;

$GatlingConfig::systemname = $system;
GatlingConfig::setSystem();
my $fieldsPerMsgRef 	= Strategy::parseMessage();
my $msgTypeRef      	= Strategy::getMsgTypeRef();
my $msgNameRef = Strategy::getMsgNameRes();
my $fieldRef = Strategy::getFlenList(); 

# for each types (e.g. int8_t, bool, etc.)
my $coarseStrategyRef = Strategy::getCoarseStrategyList();
my $fineStrategyRef = Strategy::getFineStrategyList();

#Get perf from cmd line
my $perfInput="";
foreach my $arg (@ARGV) {
	$perfInput .= "$arg";
}

#Get other feedback from stdin
my @feedback;
my $gotany = 0;
while (<STDIN>) {
    my $line = $_;
    chomp($line);
    if ($line eq "END") {
        last;
    }
    push (@feedback, $line);
    $gotany = 1;
}



# Analyize our perf score, if we got one
if(length($perfInput)> 0){
	my $benign = getBenignPerf();
	chomp($perfInput);
	my @token = split(':', $perfInput);
	if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
		#skip
	}else{
		my $perf = $token[2];
		my $strategy = $token[1];
		my $res   = $token[3];
		my $score = Utils::computeAttackScore($perf,$res);

		#Save Perf Score
		my $f = open PERF, "+>>", $GatlingConfig::perfMeasured;
		if($f){
			print PERF "$strategy,$perf,$res\n";
			close(PERF);
		}

		#Check for Attack
		if($benign > 0 and isAttack($score, $benign)){
			$f = open LEARNED, "+>>", $GatlingConfig::newlyLearned;
			if($f){
				print LEARNED "FOUND $strategy,$perf,$res,$benign\n";
				close(LEARNED);
			}
		}
	}
}

#Figure out more/new strategies
my $weight=0;
my @strArray;
my $short = 0;
if ($gotany == 0) {
    #No feedback---system start
    my $i = 0;
    foreach my $msgType (@$msgNameRef) {
        my $prefix = "$weight:*?*?$msgType";
        if ($msgType eq "BaseMessage") {
            push (@strArray, "$prefix NONE 0");
            push (@strArray, "$prefix NONE 0");
            push (@strArray, "$prefix NONE 0");
            $weight++;
        } else {
            push(@strArray, "$prefix INJECT t=10 0 $GatlingConfig::clientip $GatlingConfig::serverip 0=$GatlingConfig::clientport 1=$GatlingConfig::serverport 2=111 5=5 10=$GatlingConfig::defaultwindow");
            push(@strArray, "$prefix INJECT t=10 0 $GatlingConfig::serverip  $GatlingConfig::clientip 0=$GatlingConfig::serverport 1=$GatlingConfig::clientport 2=111 5=5 10=$GatlingConfig::defaultwindow");
            push(@strArray, "$prefix WINDOW w=$GatlingConfig::defaultwindow t=10 $GatlingConfig::clientip $GatlingConfig::serverip $GatlingConfig::clientport $GatlingConfig::serverport 5");
            push(@strArray, "$prefix WINDOW w=$GatlingConfig::defaultwindow t=10 $GatlingConfig::serverip $GatlingConfig::clientip $GatlingConfig::serverport $GatlingConfig::clientport 5");
        }
       if ($short == 1 && $i == 1) { # debugging option - to keep it short
           last;
       }
        $i++;
    }

} else {
    #Examine feedback and suggest strategies
    foreach my $line (@feedback) {
        if ($short == 1) {
            last;
        }
        # pkt_cnt_Ack,client,CLOSED,3768,1
        my @tokens = split(/,/, $line);
        my $metric = $tokens[0];
        my $side = $tokens[1];
        my $key = $tokens[2];
        my $value = $tokens[3];
        my $level = $tokens[4];

        if ($side eq "server") {
            $key .= "?0";
        } elsif ($side eq "client") {
            $key .= "?1";
        } else {
            $key .= "?*";
        }
        if ($metric =~ "pkt_cnt_") {
            my $msg = substr $metric, 8;
            strategyCompose($msg, $key, $level);
        }
    }
}

sub strategyCompose {
    my $msg = shift;
    my $state = shift;
    my $level = shift;

    switch ($level) {
        case (0)
        {
            print STDERR "Selecting Coarse Strategy for $msg,$state\n";
            my $weight = 1;
            my $prefix = "$weight:$state?$msg";

            my $msgType = $msgTypeRef->{$msg};
            my $fields = $fieldsPerMsgRef->{$msgType};
            my $fieldIdx = 0;

            # delivery actions
            push (@strArray, "$prefix DROP 50");
            push (@strArray, "$prefix DROP 100");
            push (@strArray, "$prefix DELAY 1.0");
            push (@strArray, "$prefix DUP 10");

            # coase lying actions
            foreach my $field (@{$fields}) { 
                my $coarseStr = $coarseStrategyRef->{$field};
                foreach my $str (@{$coarseStr}) { 
                    push (@strArray, "$prefix LIE $str $fieldIdx");
                }
                $fieldIdx++;
            }
        }
        case (1) 
        {
            print STDERR "Selecting Fine Strategy for $msg,$state\n";
            my $weight = 5;
            my $prefix = "$weight:$state?$msg";

            my $msgType = $msgTypeRef->{$msg};
            my $fields = $fieldsPerMsgRef->{$msgType};
            my $fieldIdx = 0;
            # coase lying actions
            foreach my $field (@{$fields}) { 
                my $fineStr = $fineStrategyRef->{$field};
                foreach my $str (@{$fineStr}) { 
                    push (@strArray, "$prefix LIE $str $fieldIdx");
                }
                $fieldIdx++;
            }
        }
        # default
        {
            print STDERR "Already Exercised $msg,$state\n";
        }
    }
}

sub getBenignPerf{
	my $total = 0;
	my $num = 0;
	my $res = open PERF, "<", $GatlingConfig::perfMeasured;
        if ($res) {
                while ( my $line = <PERF> ) {
                        chomp($line);
                        my @token = split(',', $line );
                        if ( $#token < 1 or $token[0] =~ /^\/\// or $token[0] =~ /^#/ ) {
                                next;
                        }
                        my $perf = $token[1];
                        my $strategy = $token[0];
                        my $res   = $token[2];

			if($strategy =~/NONE 0/){
				$total += Utils::computeAttackScore($perf,$res);
				$num +=1;
			}
                }
                close PERF;
        }

	if($num==0){
		return 0;
	}
	return $total/$num;
}

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

# Dump new strategies to stdout
foreach my $strategy (@strArray) {
    print "$strategy\n";
}
