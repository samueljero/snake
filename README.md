SNAKE: State-based Network AttacK Explorer
==========================================

Tool to automatically find attacks on unmodified implementations of transport protocols using a protocol state machine based search strategy

Details described in:

Samuel Jero, Hyojeong Lee, and Cristina Nita-Rotaru. Leveraging State Information for Automated Attack Discovery in Transport Protocol Implementations. 45th IEEE/IFIP International Conference on Dependable Systems and Networks (DSN), June 2015, pp 1-12.

###Preqs
* Python 2.7
* Perl 5.10
* Qemu (Tested with 1.7.0 / 2.3.0)
* pssh
* bridge-utils (for brctl)
* uml-utilities (for tunctl)
* Limited sudo access. In particular, the ability to run the following commands:

		/usr/bin/tunctl -u [a-z]* -t tap*
		/usr/bin/tunctl -d tap*
		/bin/ifconfig tap* hw ether 00:00:00:*:*:*
		/bin/ifconfig tap* up
		/bin/ifconfig tap* 0.0.0.0 up
		/bin/ifconfig tap* 10.0.*.* netmask 255.255.*.* up
		/bin/ifconfig tap* down
		/bin/ifconfig br* up
		/bin/ifconfig br* 10.0.*.* netmask 255.255.*.* up
		/bin/ifconfig br* down
		/sbin/brctl addbr br*
		/sbin/brctl delbr br*
		/sbin/brctl addif br* tap*
		/sbin/brctl delif br* tap*
		/usr/local/sbin/restart-dhcpd
		/bin/ifconfig tap*

###Usage
* Configure Network:

		cd system/scripts/kvm_scripts/
		NetworkManager.pl dnsconf 1 <max>
		NetworkManager.pl dns 1 <max>
		NetworkManager.pl addall 1 <max>

* Configure VMs:

		cd system/scripts/kvm_scripts/
		wget http://www.cs.purdue.edu/~sjero/snake_vm.tar.gz
		(or http://sjero.net/ds2/snake_vm.tar.gz)
		tar xf snake_vm.tar.gz
		for each vm:
			ssh-copy-id root@10.0.1.<id>

	For reference, the VM users are `root` and the password is `Gat11ng` in our default VMs

* Configure and Build NS-3

		cd ns-3-dev
		./waf configure --enable-examples
		cd system/messages/
		./switchToSystem.sh tcp

* In one terminal, run the global controller

		cd system/scripts/coordinator
		./coordinator.py

* In annother terminal, run an executor:

		cd system/scripts/
		./gatling.pl


Samuel Jero <sjero@purdue.edu>  
3-30-2017
