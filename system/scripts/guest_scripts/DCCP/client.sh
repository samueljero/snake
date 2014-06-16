#!/bin/bash
timeout 61s stdbuf -i 0 -o 0 /root/iperf/src/iperf --bindport 5050 -c 10.1.2.3 -t 60 -i 0.5 -f k 2>&1 | ~/perfcollector.pl

