#!/bin/sh
# create bridges
brctl addbr br-left
brctl addbr br-right
# create tap devices
tunctl -t tap-left
tunctl -t tap-right
# set ip addresses and up
ifconfig tap-left 0.0.0.0 promisc up
ifconfig tap-right 0.0.0.0 promisc up
# bring bridges up
brctl addif br-left tap-left
ifconfig br-left up
brctl addif br-right tap-right
ifconfig br-right up
# just check
brctl show
# make sure kernel has ethernet filtering disabled, otherwise only STP and ARP are allowed
pushd /proc/sys/net/bridge
for f in bridge-nf-*; do echo 0 > $f; done
popd
# we will use XEN
echo "setup XEN"
#lxc-create -n left -f lxc-left.conf
#lxc-create -n right -f lxc-right.conf
