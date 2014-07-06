#!/bin/bash
timeout 70s stdbuf -i 0 -o 0 /root/iperf/src/iperf -p5001 -s --dccp -l1300 -i 0.5 -f k &
timeout 70s stdbuf -i 0 -o 0 /root/iperf/src/iperf -p5002 -s --dccp -l1300 -i 0.5 -f k | ~/perfcollector.pl $1 &

