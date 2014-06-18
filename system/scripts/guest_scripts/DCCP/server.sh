#!/bin/bash
#iperf -c 10.1.2.3 -p 1234 -t600
timeout 60s stdbuf -i 0 -o 0 /root/iperf/src/iperf -s -i 0.5

