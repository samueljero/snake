#!/bin/bash

OFFSET="offset 0"
if [ $# -gt 1 ]
then
OFFSET="$1 $2"
fi
./kvm_scripts/CloneManager.pl start 1 4 $OFFSET -special 3
./kvm_scripts/SnapshotManager.pl resume 1 4 $OFFSET -special 3
echo "Waiting for VMs to start..."
sleep 50
echo "connectivity test"
if [[ `hostname -s ` = sound* ]] || [[ `hostname -s` = ocean1* ]]; then
pssh -h pssh_all0.txt "echo 1"
else
pssh -h pssh_all.txt "echo 1"
fi
