#!/bin/bash
if [ $HOSTNAME = "ocean1.cs.purdue.edu" ] || [ $HOSTNAME = "sound.cs.purdue.edu" ]
then
	sudo /usr/local/sbin/restart-dhcpd < /tmp/ip-mac
else
	pkill dnsmasq
	where_am_i=$(
	cd -P -- "$(dirname -- "$0")" && pwd -P
	)
	sudo dnsmasq --strict-order --bind-interfaces --pid-file=$where_am_i/host.pid --conf-file=$where_am_i/dnsmasq.conf --except-interface lo --listen-address 10.0.0.1 --dhcp-range 10.0.1.1,10.1.1.254 --dhcp-leasefile=$where_am_i/host.leases --dhcp-lease-max=254 --dhcp-no-override
fi
