#!/usr/bin/env perl

package Utils;

## VM related commands
sub snapshotVMs($) {
	print "XenSnap\n";
	#system ("./snapshot.sh");
#system ("xm save small1 /tmp/ramdisk/sm1 &");
#system ("xm save small2 /tmp/ramdisk/sm2 &");
#system ("xm save small3 /tmp/ramdisk/sm3 &");
#system ("xm save small4 /tmp/ramdisk/sm4 &");
#system ("xm save small5 /tmp/ramdisk/sm5 &");
#system ("wait");
	return;
}
sub snapshotBranches($) {
	system ("xm destroy small1 > /dev/null");
	system ("xm destroy small2 > /dev/null");
	system ("xm destroy small3 > /dev/null");
	system ("xm destroy small4 > /dev/null");
	system ("xm destroy small5 > /dev/null");
#system ("xm save small1 sm1_branch");
#system ("xm save small2 sm2_branch");
#system ("xm save small3 sm3_branch");
#system ("xm save small4 sm4_branch");
#system ("xm save small5 sm5_branch");
	return;
}

sub restoreVMs() {
	system("date --set=\"".$tm."\""); # rollback time
		print "xm restore";
	system ("./restore.sh > /dev/null");
#system ("xm restore /tmp/ramdisk/sm1 &");
#system ("xm restore /tmp/ramdisk/sm2 &");
#system ("xm restore /tmp/ramdisk/sm3 &");
#system ("xm restore /tmp/ramdisk/sm4 &");
#system ("xm restore /tmp/ramdisk/sm5 &");
#system ("wait");
}

1;
