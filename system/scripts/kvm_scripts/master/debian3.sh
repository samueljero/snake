#!/bin/bash
if [[ `hostname -s` = sound* ]]; then
qemu-system-x86_64 -hda debian3-master.qcow2 -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=pcnet,macaddr=00:00:00:01:00:03 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=00:00:00:02:00:03,vlan=1 -vnc :2 -M pc-0.12
else
qemu-system-x4 -hda debian3-master.qcow2 -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=pcnet,macaddr=00:00:00:01:00:03 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1 -net nic,model=pcnet,macaddr=00:00:00:02:00:03,vlan=1 -vnc :2 -M pc-0.12
fi
