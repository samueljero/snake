#!/usr/bin/env perl

package Perf;
use strict;
use warnings;
use IO::Socket::INET;
use Scalar::Util qw(looks_like_number);
require GatlingConfig;

sub TCP_Perf{
	my $eachline = shift;
	open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
	my $cnt= $eachline =~ tr/.//;
        SCORE->printflush("$cnt\n");
	close SCORE;
}

sub DCCP_Perf{
	my $eachline = shift;
	#print "$eachline\n";
	open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
	my @cnt= $eachline =~ qr/\[ +([0-9]+)\] +([0-9\.]+- *[0-9\.]+) sec +([0-9\.]+) KBytes + ([0-9\.]+) Kbits\/sec/;
	if (@cnt) {
		print SCORE "$cnt[2]\n";
		#print "PERF: $cnt[2]\n";
	}
	close SCORE;
}

