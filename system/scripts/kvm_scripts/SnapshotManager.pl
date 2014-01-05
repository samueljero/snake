#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use IO::Socket; 

my $debug = 0;

if ($#ARGV < 0) {
	print "usage 1: ./SnapshotManager.pl pause|save|load|resume|kill (start) num\n";
	exit;
}

if ($#ARGV < 1) {
	print "Need number of clones\n";
	exit;
}

sub tellKVM {
  my $telnetport = shift;
  my $command = shift;
  my $quit = shift;
  my $socket;
  my $line;
  $socket = new IO::Socket::INET (
      PeerAddr => '127.0.0.1',
      PeerPort => $telnetport,
      Proto => 'tcp',
      ) or return;
  print $socket "$command\n";
  my $ret=1;
  while ($quit==0 && $ret > 0) {
    $socket->recv($line, 1024, 0);
    $ret = length($line);
  }
  close $socket;
}

my $basedir = abs_path(dirname(__FILE__));
my $masterdir = $basedir."/master";
my $clonedir = $basedir."/images";

my $command = $ARGV[0];
my $num = $ARGV[1];
my $start = 1;
my $exec_command = $ARGV[2];
my $sn = 1;
my $seq = 1;
if ($#ARGV == 2) {
  $start = $ARGV[1];
  $num = $ARGV[2] + $start - 1;
}

if ($#ARGV == 3) {
  $start = $ARGV[1];
  $num = $ARGV[2] + $start - 1;
  $exec_command = $ARGV[3];
	$sn = $ARGV[3];
}

if($#ARGV == 4){
	$start = $ARGV[1];
	$num = $ARGV[2] + $start - 1;
	$exec_command = $ARGV[3];
	$sn = $ARGV[3];
	$seq = $ARGV[4];
}


my $exec = "";
my $batch_telnet = 1;
if ($command eq "pause") {
  system("ssh 10.1.1.1 date > date_paused");
	for (my $i = $start; $i <= $num; $i++) {
		my $telnetport = 10100 + $i;
    tellKVM($telnetport, "stop", 1);
	}
} elsif ($command eq "resume") {
	for (my $i = $start; $i <= $num; $i++) {
		my $telnetport = 10100 + $i;
    tellKVM($telnetport, "c", 1);
	}
} elsif ($command eq "save") { # kill afer saving
	for (my $i = $start; $i <= $num; $i++) {
		my $telnetport = 10100 + $i;
		my $snapshot = "$basedir/sn.$sn.$i";
		$exec = "migrate \"exec:cat > $snapshot\"";
    print("echo $exec | telnet localhost $telnetport \n");
    tellKVM($telnetport, $exec, 1);
    tellKVM($telnetport, "q", 0);
	}
} elsif ($command eq "load") {
  print "Loading sn.$sn.#";
	for (my $i = $start; $i <= $num; $i++) {
		my $telnetport = 10100 + $i;
		my $vncport = (11000 - 5900) + $i;
		my $mac1 = "00:00:00:01:";
		my $mac2 = "00:00:00:02:";
		my $str = sprintf("%02d:%02d", $i / 100, $i % 100);
		#my $line;
		#my $socket;
		$mac1 .= $str;
		$mac2 .= $str;
		my $snapshot = "$basedir/sn.$sn.$i";
		system("qemu-system-x86_64 -hda $clonedir/ubuntu-clone$i.qcow2 -m 128 -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait -incoming \"exec: cat $snapshot\" &");
		#system("qemu-system-x86_64 -hda $clonedir/ubuntu-clone$i.qcow2 -m 128 -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait -shared-mode F -shared-sequence $seq -incoming \"exec: cat $snapshot\" &");
		my $t=0;
		#while($t==0){
		#$t=1;
		#$socket = new IO::Socket::INET (
	    #  PeerAddr => '127.0.0.1',
	    #  PeerPort => $telnetport,
	    #  Proto => 'tcp',
	    # ) or $t=0;
		#}
  		#$socket->recv($line, 1024, 0);
		#close $socket;
	}
} elsif ($command eq "kill") {
	for (my $i = $start; $i <= $num; $i++) {
		my $telnetport = 10100+$i;
    tellKVM($telnetport, "quit", 0);
	}
}

