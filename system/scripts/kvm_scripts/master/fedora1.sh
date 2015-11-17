#!/bin/bash
sudo qemu-system-x86_64 -hda fedora-master.qcow2 -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=rtl8139,macaddr=00:00:00:01:00:03 -net nic,vlan=1,model=rtl8139,macaddr=00:00:00:02:00:03 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1 -vnc :3 -M pc-0.12
