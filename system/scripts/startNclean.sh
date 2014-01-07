#!/bin/sh
./kvm_scripts/CloneManager.pl start 1 5
./kvm_scripts/SnapshotManager.pl resume 1 5
echo "Waiting for VMs to start..."
sleep 30
echo "killing server"
pssh -h pssh_all.txt "pkill server"
echo "killing client"
pssh -h pssh_all.txt "pkill client"
echo "ifconfig"
pssh -h pssh_all.txt "ifconfig eth1 mtu 1400"
