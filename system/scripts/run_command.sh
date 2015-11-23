#!/bin/bash

#NS3 debugging
#NS_LOG='*=level_all|prefix_func|prefix_time'
#export NS_LOG

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

NS3_PATH="$DIR/../../ns-3-dev"
echo "NS-3 Path: $NS3_PATH"

#Actually set things up and run
PATH=$PATH:"$NS3_PATH/build/src/tap-bridge"
export PATH
echo LD_LIBRARY_PATH="$NS3_PATH/build/"  $NS3_PATH/build/examples/tcp/$1
LD_LIBRARY_PATH="$NS3_PATH/build/"  $NS3_PATH/build/examples/tcp/$1
