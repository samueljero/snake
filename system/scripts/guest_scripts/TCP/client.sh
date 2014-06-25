#!/bin/bash
#iperf -c 10.1.2.3 -p 1234 -t600
timeout 60s stdbuf -i 0 -o 0 /root/wget/src/wget --bind-address=10.1.2.5 --bind-port=5555 http://10.1.2.7/bigfile -O /dev/null --progress=dot:default 2>&1 | ~/perfcollector.pl

