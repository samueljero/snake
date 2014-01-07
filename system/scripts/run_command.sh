#!/bin/bash
./arp.sh 13 > ./run_arp.sh
chmod 755 ./run_arp.sh
#echo "setting VM arp table"
#./run_arp.sh
NS3_PATH="/home/sjero/turret/ns-3-dev"
sudo LD_LIBRARY_PATH="$NS3_PATH/build/" $NS3_PATH/build/examples/tcp/$1
