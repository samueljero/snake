#!/bin/bash
./arp.sh 13 > ./run_arp.sh
chmod 755 ./run_arp.sh
#echo "setting VM arp table"
#./run_arp.sh

#NS3 debugging
#NS_LOG='*=level_all|prefix_func|prefix_time'
#export NS_LOG

username=$(whoami)

#Figure out what the absolute path is... this is a terrible hack
NS3_PATH="/home/$username/ns-3-dev"
if [ $username = "lee747" ]; then
NS3_PATH="/scratch/lee747/turret/ns-3-dev" #sound
fi
if [ $username = "sjero" ]; then
NS3_PATH="/scratch2/sjero/ns-3-dev" #ocean1
fi
if [ $username = "root" ]; then
NS3_PATH="/home/sjero/turret/ns-3-dev" #cloud15
fi
echo "NS-3 Path: $NS3_PATH"

#Actually set things up and run
PATH=$PATH:"$NS3_PATH/build/src/tap-bridge"
export PATH
LD_LIBRARY_PATH="$NS3_PATH/build/"  $NS3_PATH/build/examples/tcp/$1
