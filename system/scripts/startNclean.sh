#!/bin/bash

OFFSET="offset 0"
PSSHSTR="";
i=1;

for var in "$@"
do
    let "j=i+1"
    if [ "$var" == "offset" ]
    then
        OFFSET="${!i} ${!j}"
    fi
    if [ "$var" == "pssh" ]
    then
        PSSHSTR="${!j}"
    fi
let "i+=1"
done
echo "OFFSET: $OFFSET"
echo "PSSH: [$PSSHSTR]"
./kvm_scripts/CloneManager.pl start 1 4 $OFFSET -special 3
./kvm_scripts/SnapshotManager.pl resume 1 4 $OFFSET -special 3
echo "Waiting for VMs to start..."
sleep 30
echo "connectivity test"
pssh $PSSHSTR "echo 1"
sleep 30
echo "connectivity test"
pssh $PSSHSTR "echo 1"
echo "all done"
