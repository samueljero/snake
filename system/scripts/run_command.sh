#!/bin/bash
./arp.sh 13 > ./run_arp.sh
chmod 755 ./run_arp.sh
#echo "setting VM arp table"
#./run_arp.sh
#NS_LOG='*=level_all|prefix_func|prefix_time'
#export NS_LOG

username=$(whoami)

NS3_PATH="/home/sjero/turret/ns-3-dev"
if [ $username = "lee747" ]; then
NS3_PATH="/scratch/lee747/turret/ns-3-dev"
fi
echo $NS3_PATH
PATH=$PATH:"$NS3_PATH/build/src/tap-bridge"
export PATH
LD_LIBRARY_PATH="$NS3_PATH/build/"  $NS3_PATH/build/examples/tcp/$1
