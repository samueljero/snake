#!/usr/bin/env perl

use strict;
use warnings;
use IO::Socket::INET;
use threads;
use Scalar::Util qw(looks_like_number);
use lib ("./Gatling/");
require GatlingConfig;

$GatlingConfig::systemname = "TCP";

for (my $i = 0; $i < $#ARGV; $i++) {
    if ($ARGV[$i] =~ "-offset") {
        $GatlingConfig::offset = $ARGV[$i+1];
	GatlingConfig::setSystem();
    }
}

GatlingConfig::offsetScoreFile();
my $socknumber = 7779 + $GatlingConfig::offset;


my $socket = new IO::Socket::INET (
		LocalHost => '10.0.0.1',
		LocalPort => $socknumber,
		Proto => 'udp',
		#Listen => 50,
		#Reuse => 1
		) or die "Error in Socket Creation for perfMonitor : $!\n";

open (PERFFILE, ">$GatlingConfig::scoreFile");
print PERFFILE "0\n";
close (PERFFILE);
print "perfMonitor listens on port $socknumber\n";

my $line;
my $next = 0;
my $perf = 0;
my $tmp = 0;

sub BFTPerf {
  my $eachline = shift;
  my @log = split /\s+/, $eachline;
  if ($#log >= 5) {
    if ($log[0] eq "Elapsed") {
      my $perf = $log[4]; #$log[2]/$log[4];
      open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
      print SCORE "$perf\n";
      print "PERF: $perf\n";
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
      print "PERF: $perf\n";
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
    print "PERF: $perf\n";
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
    print "PERF: $perf\n";
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
    STDOUT->printflush("PERF: $cnt\n");
	close SCORE;
}

sub DCCP_Perf{
	my $eachline = shift;
	#print "$eachline\n";
	open SCORE, "+>>$GatlingConfig::scoreFile" or die $!;
	my @cnt= $eachline =~ qr/\[ +([0-9]+)\] +([0-9\.]+- *[0-9\.]+) sec +([0-9\.]+) KBytes + ([0-9\.]+) Kbits\/sec/;
	if (@cnt) {
		print SCORE "$cnt[2]\n";
		print "PERF: $cnt[2]\n";
	}
	close SCORE;
}

while (1) 
{
    #my $sock = $socket->accept();
    $socket->recv($line, 1024);
    #chop($line);
    $tmp = $tmp + 1;
    if ($tmp % 10 == 1) {
        system("date");
    }
    my @lines = split /\n/, $line;
    foreach my $eachline (@lines) {
	if($GatlingConfig::systemname eq "Prime"){
       		Prime_bugPerf($eachline);
		#PrimePerf($eachline);
	}elsif($GatlingConfig::systemname eq "BFT"){
		BFTPerf($eachline);
	}elsif($GatlingConfig::systemname eq "Steward"){
		StewardPerf($eachline);
	}elsif($GatlingConfig::systemname eq "TCP"){
		TCP_Perf($eachline);
	}elsif($GatlingConfig::systemname eq "DCCP"){
		DCCP_Perf($eachline);
 	}else{
		print "ERROR: Unknown system!\n";
	}
    }
}
