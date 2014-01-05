#!/usr/bin/perl -w

use strict;
use IO::Socket::INET;
#use Time::Local;
use threads;
use Scalar::Util qw(looks_like_number);
use lib ("./Gatling/");
require GatlingConfig;

my $socket = new IO::Socket::INET (
		LocalHost => '10.0.0.1',
		LocalPort => '7779',
		Proto => 'udp',
		#Listen => 50,
		#Reuse => 1
		) or die "Error in Socket Creation for perfMonitor : $!\n";


open (PERFFILE, ">$GatlingConfig::scoreFile");
print PERFFILE "0\n";
close (PERFFILE);
print "perfMonitor listens on port 7779\n";

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
    #Prime_bugPerf($eachline);
    # PrimePerf($eachline);
    BFTPerf($eachline);
    #StewardPerf($eachline);
  }
}
