#!/usr/bin/perl -w

use strict;
use POSIX;

if (@ARGV < 2) {
    print "Usage: $0 <start_vm> <end_vm>\n";
    exit 1;
}
my $start = $ARGV[0];
my $end = $ARGV[1];
if($start < 0 || $end < 0 || $end <= $start)
{
  print "Incorrect start_vm = $start and end_vm = $end\n";
  exit 1;
}

chomp(my $clone=`ifconfig eth0 | grep HWaddr  | sed 's/^.*00:01://' | awk '{split(\$0,array,":")} END {print array[1]*100+array[2]}'`);

my $mac = sprintf("00:00:00:02:%02d:%02d", $clone / 100, $clone % 100);

open(FILE, ">/etc/hostname");
print FILE "ubuntu-clone$clone\n";
close FILE;

open(FILE, ">/etc/hosts");
print FILE "127.0.0.1	localhost\n";
print FILE "127.0.1.1	ubuntu-clone$clone\n\n";
for(my $i = $start; $i <= $end; $i++)
{
  print FILE "10.1.2.$i	ubuntu-clone$i\n";
}
close FILE;

system("hostname ubuntu-clone$clone");

open(FILE, ">/etc/network/interfaces");
print FILE <<END;
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp

auto eth1
iface eth1 inet static
address 10.1.2.$clone
netmask 255.255.255.0
broadcast 10.1.2.255
network 10.1.2.0
# gateway 10.1.2.254
# hwaddress ether $mac

# up route add -net 10.1.2.0 netmask 255.255.255.0 dev eth1
END
close(FILE);

#open(FILE, ">/etc/udev/rules.d/70-persistent-net.rules");
#print FILE "SUBSYSTEM==\"net\", ACTION==\"add\", DRIVERS==\"?*\", ATTR{address}==\"$mac\", ATTR{dev_id}==\"0x0\", ATTR{type}==\"1\", KERNEL==\"eth*\", NAME=\"eth0\"\n";
#close(FILE);
system("ifconfig eth1 down > /dev/null 2>\&1");
system("ifconfig eth1 10.1.2.$clone netmask 255.255.255.0 broadcast 10.1.2.255 up");
#system("/etc/init.d/networking restart");
