#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;

my $basedir = abs_path(dirname(__FILE__));
my $classroot = $basedir; 
my $masterdir = $classroot."/master";
my $clonedir = $basedir."/images";
my $special3=0;

my $ssh_delay = 10; # seconds
my $ssh_attempts = 10;
my $ipbase = "10.1.1";

my $usage = "\nusage 1: $0 netstart|netstop|create|fresh|prep|start|kill|status start_vm end_vm\n";

if (@ARGV < 3) {
  print "$usage\n";
  exit;
}

# need root for some things
my $login = (getpwuid $>);

my $command = $ARGV[0];
my $start = $ARGV[1];
my $num = $ARGV[2];
my $exec_command = "/usr/local/bin/CustomizeVM.pl";

if ($num < 1 || $start < 1 || $num < $start)
{ 
  print "Incorrect VM no: start_vm = $start and end_vm = $num\n";
  print "$usage\n";
  exit;
}


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
    if($i==3 && $special3==1){
	#system("qemu-system-x86_64 -hda $clonedir/debian3-clone.qcow2 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=pcnet,macaddr=$mac1 -net tap,ifname=tap-vm3$i,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=$mac2,vlan=1 -vnc :$vncport -M pc-0.12 -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
	#system("qemu-system-x86_64 -hda $clonedir/fedora-clone.qcow2 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=pcnet,macaddr=$mac1 -net tap,ifname=tap-vm3$i,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=$mac2,vlan=1 -vnc :$vncport -M pc-0.12 -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
	system("qemu-system-x86_64 -hda $clonedir/win95-clone.qcow2 -m 128 -M pc -vga std -no-kvm -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,model=ne2k_pci,macaddr=$mac1 -net tap,ifname=tap-vm3$i,downscript=no,script=no,vlan=1 -net nic,model=ne2k_pci,macaddr=$mac2,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
    }else{
	system("qemu-system-x86_64 -hda $clonedir/ubuntu-clone$i.qcow2 -m 128 -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait &");
	#system("qemu-system-x86_64 -hda $clonedir/ubuntu-clone$i.qcow2 -m 128 -k \"en-us\" -net nic,model=virtio,macaddr=$mac1 -net tap,ifname=tap-h$i,downscript=no,script=no -net nic,vlan=1,model=virtio,macaddr=$mac2 -net tap,ifname=tap-vm$i,downscript=no,script=no,vlan=1 -vnc :$vncport -monitor telnet:127.0.0.1:$telnetport,server,nowait -shared-mode F -shared-sequence 1 &");
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
