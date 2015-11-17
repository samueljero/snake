#!/bin/bash
sudo qemu-system-x86_64 -hda win95-master.qcow2 -m 128 -M pc -vga std -vnc :2 -no-kvm -net tap,ifname=tap-h3,downscript=no,script=no -net nic,model=ne2k_pci,macaddr=00:00:00:01:00:03 -net nic,vlan=1,model=ne2k_pci,macaddr=00:00:00:02:00:03 -net tap,ifname=tap-vm3,downscript=no,script=no,vlan=1
