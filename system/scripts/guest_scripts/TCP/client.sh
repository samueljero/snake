#!/bin/bash
#iperf -c 10.1.2.3 -p 1234 -t600
IP=`ifconfig eth1| grep 'inet addr:' | awk {'print $2'} | cut -d ":" -f 2`
TARGET=3
if [ $# -gt 0 ]
    then
TARGET=$(($TARGET+$1))
    fi
timeout 60s stdbuf -i 0 -o 0 /root/wget --bind-address=$IP --bind-port=5555 http://10.1.2.$TARGET/bigfile -O /dev/null --progress=dot:default 2>&1 | ~/perfcollector.pl $1

