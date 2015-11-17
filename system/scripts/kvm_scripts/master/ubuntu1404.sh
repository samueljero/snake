#!/bin/bash
if [[ `hostname -s` = sound* ]]; then
qemu-system-x86_64 -hda ubuntu-1404-master.qcow2 -m 512 -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=virtio,macaddr=00:00:00:01:00:03 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1 -net nic,model=virtio,macaddr=00:00:00:02:00:03,vlan=1 -vnc :2
else
qemu-system-x86_64 -hda ubuntu-1404-master.qcow2 -m 512 -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=virtio,macaddr=00:00:00:01:00:03 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1 -net nic,model=virtio,macaddr=00:00:00:02:00:03,vlan=1 -vnc :2
fi
