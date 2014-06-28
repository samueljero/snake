#!/bin/bash

OFFSET="offset 0"
if [ $# -gt 1 ]
then
OFFSET="$1 $2"
fi
./kvm_scripts/CloneManager.pl start 1 4 $OFFSET -special 3
./kvm_scripts/SnapshotManager.pl resume 1 4 $OFFSET -special 3
echo "Waiting for VMs to start..."
sleep 30
echo "killing server"
if [[ `hostname -s ` = sound* ]] || [[ `hostname -s` = ocean1* ]]; then
pssh -h pssh_all0.txt "pkill server"
else
pssh -h pssh_all.txt "pkill server"
fi
echo "killing client"
if [[ `hostname -s ` = sound* ]] || [[ `hostname -s` = ocean1* ]]; then
pssh -h pssh_all0.txt "pkill client"
else
pssh -h pssh_all.txt "pkill client"
fi
echo "ifconfig"
if [[ `hostname -s ` = sound* ]] || [[ `hostname -s` = ocean1* ]]; then
pssh -h pssh_all0.txt "ifconfig eth1 mtu 1400"
else
pssh -h pssh_all.txt "ifconfig eth1 mtu 1400"
fi
