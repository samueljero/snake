#!/bin/bash

#NS3 debugging
#NS_LOG='*=level_all|prefix_func|prefix_time'
#export NS_LOG

username=$(whoami)
hostname=$(hostname)

#Figure out what the absolute path is... this is a terrible hack
NS3_PATH="/home/$username/ns-3-dev"
if [ $username = "lee747" ] && [ $hostname = "cloud12.cs.purdue.edu" ]; then
NS3_PATH="/home/lee747/GIT/sjero-turret-ns3"
fi
if [ $username = "lee747" ] && [ $hostname = "sound.cs.purdue.edu" ]; then
NS3_PATH="/scratch/lee747/turret/ns-3-dev"
fi
if [ $username = "root" ] && [ $hostname = "cloud12.cs.purdue.edu" ]; then
NS3_PATH="/home/lee747/GIT/ns-3-dev" #Runs as root...
fi
if [ $username = "sjero" ] && [ $hostname = "sound.cs.purdue.edu" ]; then
NS3_PATH="/scratch/sjero/ns-3-dev"
fi
if [ $username = "sjero" ] && [ $hostname = "ocean1.cs.purdue.edu" ]; then
NS3_PATH="/scratch2/sjero/ns-3-dev"
fi
if [ $username = "root" ] && [ $hostname = "cloud15.cs.purdue.edu" ]; then
NS3_PATH="/home/sjero/turret/ns-3-dev" #Runs as root...
fi
echo "NS-3 Path: $NS3_PATH"

#Actually set things up and run
PATH=$PATH:"$NS3_PATH/build/src/tap-bridge"
export PATH
echo LD_LIBRARY_PATH="$NS3_PATH/build/"  $NS3_PATH/build/examples/tcp/$1
LD_LIBRARY_PATH="$NS3_PATH/build/"  $NS3_PATH/build/examples/tcp/$1
