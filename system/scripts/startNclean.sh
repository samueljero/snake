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
exit
./kvm_scripts/CloneManager.pl start 1 4 $OFFSET -special 3
./kvm_scripts/SnapshotManager.pl resume 1 4 $OFFSET -special 3
echo "Waiting for VMs to start..."
sleep 30
echo "connectivity test"
if [[ `hostname -s ` = sound* ]] || [[ `hostname -s` = ocean1* ]]; then
pssh $PSSHSTR "echo 1"
#pssh -h pssh_all0.txt "echo 1"
#else
#pssh -h pssh_all.txt "echo 1"
fi
sleep 30
echo "connectivity test"
if [[ `hostname -s ` = sound* ]] || [[ `hostname -s` = ocean1* ]]; then
pssh $PSSHSTR "echo 1"
#pssh -h pssh_all0.txt "echo 1"
#else
#pssh -h pssh_all.txt "echo 1"
fi
echo "all done"
