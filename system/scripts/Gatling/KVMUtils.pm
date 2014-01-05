#!/usr/bin/perl
package Utils;
use Time::HiRes;

$seq = 0;
$branch = 0;
$snp = 1;
sub updateSnapshot($) {
  $branch = $_[0];
	$snap = "$seq.branch.$branch";
  print "Next snap: $snap\n";
}

## VM related commands
sub snapshotVMs() {
# base snap
	#my $seq = $_[0];
	#my $snap = "$seq.branch.$branch";
  print "======== Take a snapshot: $snap";
  Utils::logTime("start snapshot VM");
	system ("./kvm_scripts/SnapshotManager.pl save 1 $GatlingConfig::num_vms $snap");
  $snp+=1;
  Utils::logTime("end snapshot VM");
	return;
}

sub pauseVMs() {
  print "======== Pause =====";
  Utils::logTime("start pause VM");
	system ("./kvm_scripts/SnapshotManager.pl pause 1 $GatlingConfig::num_vms");
  sleep(0.05);
  Utils::logTime("end pause VM");
	return;
}

sub resumeVMs() {
  print "======== Resume =====";
  Utils::logTime("start resume VM");
	system ("./kvm_scripts/SnapshotManager.pl resume 1 $GatlingConfig::num_vms");
  Utils::logTime("end resume VM");
	return;
}

sub killVMs() {
  print "======== Kill =====";
  Utils::logTime("start kill VM");
	#system ("./kvm_scripts/SnapshotManager.pl pause 1 $GatlingConfig::num_vms");
	#system ("pkill qemu");
  my $ps = `ps -ef|grep qemu`;
  my $nr_of_lines = $ps =~ tr/\n//;
  #while ($nr_of_lines > 3) {
    system ("./kvm_scripts/SnapshotManager.pl kill 1 $GatlingConfig::num_vms");
    $ps = `ps -ef|grep qemu`;
    $nr_of_lines = $ps =~ tr/\n//;
  #}
  print "======== clean == $ps ===";
  Utils::logTime("end kill VM");
	return;
}

sub restoreVMs($) {
  $branch = $_[0];
	$snap_local = "$seq.branch.$branch";
	#my $seq = $_[0];
	#my $snap = "$seq.branch.$branch";
  #killVMs();
  print "======== Load =====";
  Utils::logTime("start load VM");
	system ("./kvm_scripts/SnapshotManager.pl load 1 $GatlingConfig::num_vms $snap_local $snp");
  Utils::logTime("end load VM");
	#system("date --set=\"".$tm."\""); # rollback time
}

1;
