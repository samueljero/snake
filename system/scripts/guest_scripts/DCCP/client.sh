#!/bin/bash
TARGET=3
if [ $# -gt 0 ]
then
	TARGET=$((TARGET+$1))
fi
timeout 61s stdbuf -i 0 -o 0 /root/iperf/src/iperf --bindport 5050 -c 10.1.2.$TARGET -t 60 -i 0.5 -f k --dccp -l1300 2>&1 | ~/perfcollector.pl $1

