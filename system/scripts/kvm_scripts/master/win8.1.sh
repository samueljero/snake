#!/bin/bash
qemu-system-x86_64 -hda windows-8.1-master.qcow2 -enable-kvm -m 2048 -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=e1000,macaddr=00:00:00:01:00:03 -net nic,model=e1000,macaddr=00:00:00:02:00:03,vlan=1 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1 -vnc :2
