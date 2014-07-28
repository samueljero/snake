#!/usr/bin/perl

########################
# STATELESS ATTACKS
########################

use strict;
use POSIX;
use Switch;
my $str = "";
my $DEBUG = 0;

######################
# Some static setting - make it configurable later
######################
my $clientip = "10.1.2.2";
my $serverip = "10.1.2.3";
my $malip = "10.1.2.1";
my $clientport= 5555;
my $serverport= 80;
my $malport = 5556;
my $defaultwindow=20000;

######################
# LOAD MSG TYPES
######################

use lib ("../Gatling/");
#require MsgParse;
require Strategy;
require GatlingConfig;

GatlingConfig::systemTCP();
my $fieldsPerMsgRef 	= Strategy::parseMessage();
my $msgTypeRef      	= Strategy::getMsgTypeRef();
my $msgNameRef = Strategy::getMsgNameRes();

# %msgFlenList - 
my $fieldRef = Strategy::getFlenList(); 
# my $fieldRef = Strategy::getMsgFlenList(); 

# for each types (e.g. int8_t, bool, etc.)
my $coarseStrategyRef = Strategy::getCoarseStrategyList();
my $fineStrategyRef = Strategy::getFineStrategyList();

foreach my $arg (@ARGV) {
    $str .= "$arg";
}

my @feedback;
my $gotany = 0;

sub DEBUGPRINT{
    my $line = shift;
    if ($DEBUG) {
        print STDERR "$line\n";
    }
}

while (<STDIN>) {
    my $line = $_;
    chomp($line);
    if ($line eq "END") {
        last;
    }
    push (@feedback, $line);
    $gotany = 1;
    DEBUGPRINT("got from stdin: $line-----------");
}

my $weight=0;
my @strArray;

my $short = 0;
if ($gotany == 0) {
### WHEN NO FEEDBACK IS GIVEN
    my $i = 0;
    foreach my $msgType (@$msgNameRef) {
        my $prefix = "$weight:*?*?$msgType";
        if ($msgType eq "BaseMessage") {
            push (@strArray, "$prefix NONE 0");
            $weight++;
        } else {
            push(@strArray, "$prefix INJECT t=10 0 $clientip $serverip 0=$clientport 1=$serverport 2=111 5=5 10=$defaultwindow");
            push(@strArray, "$prefix INJECT t=10 0 $serverip  $clientip 0=$serverport 1=$clientport 2=111 5=5 10=$defaultwindow");
            push(@strArray, "$prefix WINDOW w=$defaultwindow t=10 $clientip $serverip $clientport $serverport 5");
            push(@strArray, "$prefix WINDOW w=$defaultwindow t=10 $serverip $clientip $serverport $clientport 5");
        }
       if ($short == 1 && $i == 1) { # XXX - to keep it short
           last;
       }
        $i++;
    }

} else {
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
            DEBUGPRINT("coarse strategy for $msg,$state");
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
            DEBUGPRINT("fine strategy for $msg,$state");
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
            DEBUGPRINT("already exercised $msg,$state");
        }
    }
}

foreach my $strategy (@strArray) {
    print "$strategy\n";
}

#foreach my $typename (keys %{$msgTypeRef}) {
#    print "$typename\n";
#}
#
#
#foreach my $typename (keys %{$coarseStrategyRef}) {
#    print "$typename\n";
#}

#foreach my $key (keys %{$fieldsPerMsgRef}) {
#    my $arrref = $fieldsPerMsgRef->{$key};
#    foreach my $arritem (@{$arrref}) {
#    print "$key $arritem\n";
#    }
#}
