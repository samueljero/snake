#!/usr/bin/env perl

package Perf;
use strict;
use warnings;
use IO::Socket::INET;
use Scalar::Util qw(looks_like_number);
require GatlingConfig;

sub BFTPerf {
  my $eachline = shift;
  my @log = split /\s+/, $eachline;
  if ($#log >= 5) {
    if ($log[0] eq "Elapsed") {
      my $perf = $log[4]; #$log[2]/$log[4];
      open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
      print SCORE "$perf\n";
      #print "PERF: $perf\n";
      close SCORE;
    } else {
      print "==> $eachline\n";
    }
  } else {
    print "==> $eachline\n";
  }
}

sub StewardPerf {
  $_ = shift;
  if (/Added latency:/) {
    my @log = split();
    if ($#log >= 6) {
      my $perf = $log[6]; #$log[2]/$log[4];
      open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
      print SCORE "$perf\n";
      #print "PERF: $perf\n";
      close SCORE;
    }
  }
}

sub PrimePerf {
  my $eachline = shift;
  my @log = split /\s+/, $eachline;
  if ($#log == 1 && looks_like_number($log[0]) && looks_like_number($log[1])){
    my $perf = $log[1];
    open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
    print SCORE "$perf\n";
    #print "PERF: $perf\n";
    close SCORE;
  } else {
      #print "$eachline - $#log $log[0] $log[1]\n";
  }
}

# Endadul
sub Prime_bugPerf {
  my $eachline = shift;
  my @log = split /\s+/, $eachline;
  if ($#log == 1 && looks_like_number($log[0]) && looks_like_number($log[1])){
    my $perf = $log[1];
    open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
    print SCORE "$perf\n";
    #print "PERF: $perf\n";
    close SCORE;
  } else {
      #print "$eachline - $#log $log[0] $log[1]\n";
  }
}

sub TCP_Perf{
	my $eachline = shift;
    print STDERR "opening: $GatlingConfig::scoreFile\n";
	open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
	my $cnt= $eachline =~ tr/.//;
    SCORE->printflush("$cnt\n");
    #STDOUT->printflush("PERF: $cnt\n");
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

