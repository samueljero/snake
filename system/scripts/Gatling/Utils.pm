#!/usr/bin/env perl

package Utils;

use Time::HiRes;
use GatlingConfig;
use Net::Ping;

## variables
$tm = (time)[0];
$tmstr = `date --rfc-3339=ns`;
$lostTime = 0;
BEGIN {
  open TIMING, "+>$GatlingConfig::timeLog" or die $!;
  if ($GatlingConfig::VM eq 'KVM') {
    require KVMUtils;
    import KVMUtils;
  } else {
    require XenUtils;
    import XenUtils;
  }
}

sub reportGC
{
    my $msg = shift;
    my $toread = shift;
    my $res = "";

    my $sock = new IO::Socket::INET->new( PeerAddr => $GatlingConfig::GlobalCollectorAddr,
            PeerPort => $GatlingConfig::GlobalCollectorPort,
            Proto => 'tcp');
    die "Could not create socket to ctrl connect NS3: $GatlingConfig::GlobalCollectorPort $!\n" unless $sock;
    print $sock "$GatlingConfig::ListenAddr:$GatlingConfig::ListenPort:$msg";
    if ($toread == 1) {
        while(my $tmp=<$sock>){
            $res=$res.$tmp;
        }
    }
    $sock->close();
    return $res;
}

sub useXEN()
{
  require XenUtils;
  import XenUtils;
}

sub useKVM()
{
  require KVMUtils;
  import KVMUtils;
}

sub logTime
{
  my $msg = shift;
  my $curTime = Time::HiRes::time + $lostTime;
  print TIMING "$curTime $msg\n";
}
## NS 3 related commands 

sub directTopology($)
{
  my ($comm) = @_;
  my $topoPort = 8000 + $GatlingConfig::offset;
  my $sock = new IO::Socket::INET->new( PeerAddr => '127.0.0.1',
      PeerPort => $topoPort,
      Proto => 'tcp');
  die "Executor: Could not create socket to talk to NS3 ($topoPort): $!\n" unless $sock;
  print $sock "$comm\n";
  my $tmp;
  $res="";
  while($tmp=<$sock>){
	  $res=$res.$tmp;
  }
  if ($GatlingConfig::debug > 1) {
  	print "$comm : $res\n";
  }
  $sock->close();
  return $res;
}

sub freezeNS3() {
  logTime("start freezeNS3");
  directTopology("F");
  sleep(0.05);
  logTime("end freezeNS3");
}

sub saveNS3() {
  logTime("start saveNS3");
  directTopology("S");
  logTime("end saveNS3");
}

sub reloadNS3() {
  logTime("start reloadNS3");
  directTopology("L");
  logTime("end reloadNS3");
}

sub resumeNS3() {
  logTime("start resumeNS3");
  timeRollback();
  directTopology("R");
  logTime("end resumeNS3");
}

## HOST
sub takeTime() {
  $time_taken = Time::HiRes::time;
  $tmstr = `date --rfc-3339=ns`;
  chop($tmstr);
  print "Set Time to: $tmstr\n";
}

sub timeRollback() {
  $delta = Time::HiRes::time - $time_taken;
  $tmstr_new = `date --rfc-3339=ns`;
  `date --set="$tmstr" && date --rfc-3339=ns`;
  $lostTime = $lostTime + $delta;
}
sub resetPerfScore()
{
  system("rm $GatlingConfig::scoreFile");
}

sub getPerfScore()
{
  open SCORE, "$GatlingConfig::scoreFile" or return $GatlingConfig::brokenPerf;
  my $score = `cat $GatlingConfig::scoreFile | wc -l`;
  if ($GatlingConfig::systemname eq "BFT" || $GatlingConfig::systemname eq "Prime_bug") {
    my $lines = `cat $GatlingConfig::scoreFile | wc -l`;
    $score = $GatlingConfig::brokenPerf - $lines;
  } elsif ($GatlingConfig::systemname eq "TCP" or $GatlingConfig::systemname eq "DCCP") {
	  my $sum = 0;
	  while (my $line = <SCORE>) {
		  $sum = $sum+$line;
	  }
	  $score=$sum;
  } else {
    my $count = 0;
    my $sum = 0;
    while (my $line = <SCORE>) {
      $count = $count + 1;
      $sum = $sum+$line;
    }
    if($count==0){
		$score=0;
    }else{
		$score = $sum/$count;
    }
  }
  close SCORE;
  return $score;
}

sub getNumConnections
{
	my $host=shift;
	system("ssh $host \"$GatlingConfig::NumConnsCmd\" > $GatlingConfig::tmpFile");
	if($GatlingConfig::debug > 0){
		system("cat $GatlingConfig::tmpFile");
	}
	my $lines = `cat $GatlingConfig::tmpFile | wc -l`;
	system("rm -f $GatlingConfig::tmpFile");
	if($lines==0){
		#Error, couldn't connect!
		return -1;
	}else{
		return $lines - 3;
	}
}

sub PingHost
{
	my $host=shift;
	my $p = Net::Ping->new("tcp");
	$p->port_number(80);
	if($p->ping($host,2)){
		return 0;
	}
	return 10;
}

sub computeAttackScore
{
	my $perf=shift;
	my $resrc=shift;
	#return $GatlingConfig::brokenPerf - $perf*($resrc+1);
	return $perf*($resrc+1);
}

sub makeMetricDB
{
	my $str=shift;
	my @lines = split('\n',$str);
	my $host="server";
	my %db;

	foreach my $val (@lines){
		if($val =~/^#+/){
			next;
		}
		if($val =~/=======/){
			$host="client";
			next;
		}
		my @lna=split(',',$val); #metric,state,value
		$db{$host}{$lna[0]}{$lna[1]}=$lna[2];
	}
	return %db;
}
1;
