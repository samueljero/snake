#!/usr/bin/env perl


use warnings;
use strict;
use Cwd 'abs_path';
use File::Basename;

chomp(my $user = `echo \$USER`); # XXX: change this 
print "Running as user: $user\n";
my $dnsconf = "/scratch/cs505/ip-mac-cs505";

my $basedir = abs_path(dirname(__FILE__));

my $hwname1 = "tap-h";   # tap against the eth0 on VM
my $hwname2 = "tap-vm";  # tap against the eth1 on VM
my $tapname = "tap-ns";  # tap used by ns-3
my $brname = "br-vm";
my $bridge = "brhost";

my $usage = "usage: $0 dnsconf|dns|addall|delall start end";

if (@ARGV < 3) {
  print "$usage\n";
  exit;
}

#if ($#ARGV < 1) {
#	print "Need number of clones\n";
#	exit;
#}

my $command = $ARGV[0];
my $start = $ARGV[1];
my $num = $ARGV[2];
if ($start < 1 or $num < 1 or $num < $start) {
  print "Incorrect vm number: start = $start and num = $num\n";
  print "$usage\n";
  exit;
}

print "$command - from $start to $num\n";

if ($command eq "dnsconf")
{
  open (DNSCONF, ">$dnsconf")
    or die ("Can't open $dnsconf");

  for (my $i = $start; $i <= $num; $i++) 
  {
    my $mac1 = "00:00:00:01:";
    my $str = sprintf("%02d:%02d", $i / 100, $i % 100);
    my $ip = sprintf("10.0.1.%d", $i);
    $mac1 .= $str;
    print DNSCONF "$ip $mac1\n";
  }
  close (DNSCONF);
  system ("chmod a+r $dnsconf");
  #system("$basedir/startdns.sh");
}

if ($command eq "dns")
{
  system("sudo ifconfig $bridge down > /dev/null 2>&1");
  system("sudo brctl delbr $bridge > /dev/null 2>&1");
  sleep 1;
  system("sudo brctl addbr $bridge"); # dhcpd requires brhost to be up
  system("sudo ifconfig $bridge 10.0.0.1 netmask 255.0.0.0 up");
  system("$basedir/startdns.sh");
}

if ($command eq "stopdns")
{
  system("sudo ifconfig $bridge down > /dev/null 2>&1");
  system("sudo brctl delbr $bridge > /dev/null 2>&1");
  system("$basedir/startdns.sh");
}

if ($command eq "addall")
{
  # this prepares VM-host network

  for (my $i = $start; $i <= $num; $i++) 
  {
    system("sudo tunctl -u $user -t $hwname1$i");
    system("sudo ifconfig $hwname1$i 0.0.0.0 promisc up");
    system("sudo brctl addif $bridge $hwname1$i");
  }

  # system("$basedir/startdns.sh");
  
  my $tmp = "";
  # prepare NS-3 tap devices	  
  for (my $i = $start; $i <= $num; $i++)
  {
    $tmp = sprintf("%02d", $i);
    system("sudo brctl addbr $brname$i");
    system("sudo tunctl -u $user -t $hwname2$i"); # tap for VM
    system("sudo tunctl -u $user -t $tapname$i"); # tap for NS-3
    system("sudo ifconfig $hwname2$i 0.0.0.0 promisc up");
    system("sudo ifconfig $tapname$i 0.0.0.0 promisc up");
    system("sudo brctl addif $brname$i $hwname2$i");
    system("sudo brctl addif $brname$i $tapname$i");
    system("sudo ifconfig $brname$i up");
  }
}
if ($command eq "delall") 
{
  # NS-3 tap devices
  for (my $i = $start; $i <= $num; $i++) 
  {
    system("sudo ifconfig $brname$i down");
    system("sudo brctl delif $brname$i $hwname2$i");
    system("sudo brctl delif $brname$i $tapname$i");
    system("sudo ifconfig $hwname2$i down");
    system("sudo ifconfig $tapname$i down");
    system("sudo tunctl -d $tapname$i"); # tap for NS-3
    system("sudo tunctl -d $hwname2$i"); # tap for VM
    system("sudo brctl delbr $brname$i");
  }

  for (my $i = $start; $i <= $num; $i++) 
  {
    system("sudo ifconfig $hwname1$i down");
    system("sudo brctl delif $bridge $hwname1$i");
    system("sudo tunctl -d $hwname1$i");
  }

#  system("pkill dnsmasq");
}  

