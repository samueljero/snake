#!/usr/bin/env perl

use strict;
use warnings;
use Cwd 'abs_path';
use File::Basename;
use IO::Socket;
use Net::Ping;
use Sys::Hostname;
use threads ('yield',
     'stack_size' => 64*4096,
     'exit' => 'threads_only',
     'stringify');

my $debug = 0;
my $basedir = abs_path(dirname(__FILE__));
my $masterdir = $basedir."/master";
my $clonedir = $basedir."/images";
my $ipbase = "10.1.1";
my $sn = 1;
my $seq = 1;
my $savevmtime=0;
my $host = hostname;
if ( $host =~ /^sound/ or $host =~ /^ocean1/ ) {
    $ipbase = "10.0.1";
}

my $specialVM = -1;
# HYO - For special VM scripting
for (my $i = 0; $i < $#ARGV; $i++) {
    if ($ARGV[$i] =~ "-special") {
        $specialVM = $ARGV[$i+1];
    }
}

my $offset = 0;
for (my $i = 0; $i < $#ARGV; $i++) {
    if ($ARGV[$i] =~ "offset") {
        $offset = $ARGV[$i+1];
    }
}

if ($#ARGV < 0) {
	print "usage 1: ./SnapshotManager.pl pause|save|load|resume|kill (start) end\n";
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
  $line = "asdf";
  while ($line !~ /\(/ && length($line) >  0) {
	$socket->recv($line, 1024,0);
  }
  print $socket "$command\n";
  my $ret=1;
  $line = "asdf";
  while ($line !~ /\(/ && length($line) > 0) {
	$socket->recv($line, 1024,0);
  }
  #if ($quit==1) {
  #	sleep 10;
  #}
  close $socket;
}


sub load_and_start {
	my $i=shift;
	my $telnetport = 10100 + $i;
	my $vncport = (11000 - 5900) + $i;
	my $mac1 = "00:00:00:01:";
	my $mac2 = "00:00:00:02:";
	my $str = sprintf("%02d:%02d", $i / 100, $i % 100);
	$mac1 .= $str;
	$mac2 .= $str;
	my $snapshot = "$basedir/sn.$sn.$i";
	my $t;
	my $p;
	my $ip;
	my $vm_num = (($i-1)%4) + 1;
	do {
		if ($vm_num == $specialVM) {
            # TODO - vm images might be better configured manually. e.g. read from another config file
            # especially if we're going to use various VM images

			#system("qemu-system-x86_64 -hda $clonedir/debian3-clone.qcow2 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=pcnet,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=$mac2,vlan=1 -vnc :$vncport -M pc-0.12 -monitor telnet:127.0.0.1:$telnetport,server,nowait -daemonize -incoming \"exec: cat $snapshot\"");
			#system("qemu-system-x86_64 -hda $clonedir/fedora-clone.qcow2 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=pcnet,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=$mac2,vlan=1 -vnc :$vncport -M pc-0.12 -monitor telnet:127.0.0.1:$telnetport,server,nowait -daemonize -incoming \"exec: cat $snapshot\"");
			#system("qemu-system-x86_64 -hda $clonedir/windows-8.1-clone$i.qcow2 -m 2048 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=e1000,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=e1000,macaddr=$mac2,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait -daemonize -incoming \"exec: cat $snapshot\"");
			#system("cp $clonedir/windows95-clone$i.qcow2.orig $clonedir/windows95-clone$i.qcow2");
			#system("qemu-system-x86_64 -hda $clonedir/windows95-clone$i.qcow2 -m 128 -M pc -vga std -no-kvm -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=ne2k_pci,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=ne2k_pci,macaddr=$mac2,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait -daemonize");
			sleep(90);
			$ip = sprintf("$ipbase.%d", $i);
                        $p = Net::Ping->new("tcp");
			$p->port_number(80);
		}else{
			system("qemu-system-x86_64 -hda $clonedir/ubuntu-clone$i.qcow2 -m 128 -enable-kvm -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait -daemonize -incoming \"exec: cat $snapshot\"");
			$ip = sprintf("$ipbase.%d", $i);
			$p = Net::Ping->new("tcp");
			$p->port_number(22);
		}
        sleep 1;
	}until($p->ping($ip,2));
	$p->close();
}


my $command = $ARGV[0];
my $end = $ARGV[1];
my $start = 1;
my $exec_command = $ARGV[2];
if ($#ARGV >= 2) {
  $start = $ARGV[1];
  $end = $ARGV[2];
}

if ($#ARGV >= 3) {
  $exec_command = $ARGV[3];
	$sn = $ARGV[3];
}

if($#ARGV >= 4){
	$seq = $ARGV[4];
}

$start = $start + $offset;
$end = $end + $offset;

print "SNAP: $start --> $end -- $offset\n";
my $exec = "";
my $batch_telnet = 1;
if ($command eq "pause") {
	if($savevmtime==1){
	  system("ssh $ipbase.1 date > date_paused");
  	}
	for (my $i = $start; $i <= $end; $i++) {
		my $telnetport = 10100 + $i;
    tellKVM($telnetport, "stop", 1);
	}
} elsif ($command eq "resume") {
	for (my $i = $start; $i <= $end; $i++) {
		my $telnetport = 10100 + $i;
    tellKVM($telnetport, "c", 1);
	}
} elsif ($command eq "save") { # kill afer saving
	print("Saving Snapshot: sn.$sn.#\n");
	for (my $i = $start; $i <= $end; $i++) {
		my $telnetport = 10100 + $i;
		my $snapshot = "$basedir/sn.$sn.$i";
		$exec = "migrate \"exec:cat > $snapshot\"";
    tellKVM($telnetport, $exec, 1);
	}
    sleep 5;
	for (my $i = $start; $i <= $end; $i++) {
		my $telnetport = 10100 + $i;
    tellKVM($telnetport, "q", 0);
	}
} elsif ($command eq "load") {
	print("Loading Snapshot: sn.$sn.#\n");
	my @thr;
	for (my $i =$start; $i <= $end; $i++) {
		push(@thr,threads->create('load_and_start', $i));
	}
	for(my $i = 0; $i <= $end - $start; $i++){
		$thr[$i]->join();
	}
} elsif ($command eq "kill") {
	for (my $i = $start; $i <= $end; $i++) {
		my $telnetport = 10100+$i;
    tellKVM($telnetport, "quit", 0);
	}
}

