#!/usr/bin/env perl

use strict;
use Cwd 'abs_path';
use File::Basename;
use Sys::Hostname;

my $basedir = abs_path(dirname(__FILE__));
my $classroot = $basedir; 
my $masterdir = $classroot."/master";
my $clonedir = $basedir."/images";

my $ssh_delay = 10; # seconds
my $ssh_attempts = 10;
my $ipbase = "10.1.1";

my $usage = "\nusage 1: $0 netstart|netstop|create|fresh|prep|start|kill|status start_vm end_vm [-special specialVM]\n";

my $host = hostname;
if (@ARGV < 3) {
  print "$usage\n";
  exit;
}

my $specialVM = -1;
# HYO - For special VM scripting
for (my $i = 0; $i < $#ARGV; $i++) {
    if ($ARGV[$i] =~ "-special") {
        $specialVM = $ARGV[$i+1];
    }
}

# need root for some things
my $login = (getpwuid $>);
# if it's not cloud, we can't run as root
my $root = 1;

my $command = $ARGV[0];
my $start = $ARGV[1];
my $num = $ARGV[2];
my $exec_command = "/usr/local/bin/CustomizeVM.pl";

if (not $command eq "prep") {
    if ( $host =~ /^cloud/ ) {
        die "must run as root" if $login ne 'root';
    } else {
        $root = 0;
    }
}

if ( $host =~ /^sound/ or $host =~ /^ocean1/ ) {
    $ipbase = "10.0.1";
}
if ($num < 1 || $start < 1 || $num < $start)
{ 
  print "Incorrect VM no: start_vm = $start and end_vm = $num\n";
  print "$usage\n";
  exit;
}

my $offset = 0;
for (my $i = 0; $i < $#ARGV; $i++) {
    if ($ARGV[$i] =~ "offset") {
        $offset = $ARGV[$i+1];
    }
}

$start = $start + $offset;
$num = $num + $offset;

print "CLONE: $start --> $num -- $offset\n";
print "Command: $command - from VM $start to VM $num\n";

if ($command eq "netstart")
{
  system("$basedir/NetworkManager.pl addall $start $num");
}
elsif ($command eq "netstop")
{
  system("$basedir/NetworkManager.pl delall $start $num");
} 

elsif ($command eq "create") 
{
  system("mkdir -p $clonedir");
  for (my $i = $start ; $i <= $num; $i++) 
  {
    print "\nCreating clone image: $clonedir/ubuntu-clone$i.qcow2\n";
    system("qemu-img create -b $masterdir/ubuntu-master.qcow2 -f qcow2 $clonedir/ubuntu-clone$i.qcow2");
  }
  print "\nCreated ".($num-$start+1)." VMs for you!\n";
} 

if ($command eq "master")
{
    my $i = 1;
    my $telnetport = 10100 + $i;
    my $vncport = (11000 - 5900) + $i;
    my $mac1 = "00:00:00:01:";
    my $mac2 = "00:00:00:02:";
    my $str = sprintf("%02d:%02d", $i / 100, $i % 100);
    $mac1 .= $str;
    $mac2 .= $str;
    system("qemu-system-x86_64 -hda $masterdir/ubuntu-master.qcow2 -m 128 -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
}

if ($command eq "fresh" or $command eq "start") 
{
  for (my $i = $start; $i <= $num; $i++) 
  {
    my $telnetport = 10100 + $i;
    my $vncport = (11000 - 5900) + $i;
    my $mac1 = "00:00:00:01:";
    my $mac2 = "00:00:00:02:";
    my $str = sprintf("%02d:%02d", $i / 100, $i % 100);
    $mac1 .= $str;
    $mac2 .= $str;
    my $vm_num = (($i-1)%4) + 1;
    if ($vm_num == $specialVM) {
    	print "Special VM: $i\n";
        #system("qemu-system-x86_64 -hda $clonedir/debian3-clone.qcow2 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=pcnet,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=$mac2,vlan=1 -vnc :$vncport -M pc-0.12 -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
        #system("qemu-system-x86_64 -hda $clonedir/fedora-clone.qcow2 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=pcnet,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=$mac2,vlan=1 -vnc :$vncport -M pc-0.12 -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
        system("qemu-system-x86_64 -hda $clonedir/windows-8.1-clone$i.qcow2 -m 2048 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=e1000,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=e1000,macaddr=$mac2,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
        #system("qemu-system-x86_64 -hda $clonedir/win95-clone.qcow2 -m 128 -M pc -vga std -no-kvm -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=ne2k_pci,macaddr=$mac1 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -net nic,model=ne2k_pci,macaddr=$mac2,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
    }else{
        system("qemu-system-x86_64 -hda $clonedir/ubuntu-clone$i.qcow2 -m 128 -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
    }
  }
  print "VMs are started\n";

  if ($command eq "fresh")
  {
    print "\nYou just started your fresh VMs\n";
    print "Before proceeding to the next step, \"ping\" your VMs to see if they are up yet\n";
  }
} 

if ($command eq "kill") 
{
  for (my $i = $start; $i <= $num; $i++) 
  {
    my $telnetport = 10100+$i;
    system("echo \"quit\" | telnet localhost $telnetport ");
  }
  print "\nStopped all your VMs\n";
} 


if ($command eq "prep") 
{
  $exec_command = "/usr/local/bin/CustomizeVM.pl";
  for (my $i = $start; $i <= $num; $i++) 
  {
    my $end = 0 + $i;
    my $ip = $ipbase.".$end";
# TODO: if pub key does not exist, gen it
    system("cat ~/.ssh/id_rsa.pub | ssh root\@$ip \"cat >> ~/.ssh/authorized_keys\"");
    if( $login eq "root"){
    	system("cat /root/.ssh/id_rsa.pub | ssh root\@$ip \"cat >> ~/.ssh/authorized_keys\"");
    }
    print "copying CustomizeVM.pl\n";
    system("scp -o StrictHostKeyChecking=no $basedir/CustomizeVM.pl root\@$ip:/usr/local/bin/CustomizeVM.pl");
    print "run $exec_command\n";
    system("ssh -o StrictHostKeyChecking=no root\@$ip \"$exec_command $start $num\"");
  }
  print "\nPreparation is in progress. Please wait...\n";
}

#
# ARP table
#
if ($command eq "arp" or $command eq "prep" )
{
  if ($command eq "start")
  {
    sleep 3;
  }
  my $hwaddr = "";
  my $str = "";
  print "Testing ConnectionAttempts\n";
  for (my $i = $start; $i <= $num; $i++)
  {
    for (my $j = $start; $j <= $num; $j++)
    {
      if ($i != $j)
      {
        $hwaddr = "00:00:00:02:";
        $str = sprintf("%02d:%02d", $j / 100, $j % 100);
        $hwaddr .= $str;
        system("ssh -o ConnectionAttempts=$ssh_attempts root\@$ipbase.$i \"arp -i eth1 -s 10.1.2.$j $hwaddr\"");
      }
    }
  }
  print "Updated ARP tables...\n";
  print "\nYour VMs are ready now!\n";
}

#
# Status 
#
if ($command eq "status")
{
  chomp(my $user = `echo \$USER`); 

  system ("ps -ef | grep $user | grep qemu-system-x86_64 | grep -v grep > /dev/null");
  if ($? == 0)
  {
    print "\nStaus of your VMs: Running\n\n";
    exit;
  }
  else
  {
    print "\nStatus of your VMs: Not running\n\n";
    exit 1;
  }
}
