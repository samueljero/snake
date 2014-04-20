#!/usr/bin/env perl

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
  print "======== Snapshot ========\n";
  Utils::logTime("start snapshot VM");
  system ("./kvm_scripts/SnapshotManager.pl save 1 $GatlingConfig::num_vms $snap");
  $snp+=1;
  Utils::logTime("end snapshot VM");
  return;
}

sub pauseVMs() {
  print "======== Pause ========\n";
  Utils::logTime("start pause VM");
  system ("./kvm_scripts/SnapshotManager.pl pause 1 $GatlingConfig::num_vms");
  sleep(0.05);
  Utils::logTime("end pause VM");
  return;
}

sub resumeVMs() {
  print "======== Resume ========\n";
  Utils::logTime("start resume VM");
  system ("./kvm_scripts/SnapshotManager.pl resume 1 $GatlingConfig::num_vms");
  Utils::logTime("end resume VM");
  return;
}

sub killVMs() {
  print "======== Kill ========\n";
  Utils::logTime("start kill VM");
  system ("./kvm_scripts/SnapshotManager.pl kill 1 $GatlingConfig::num_vms");
  Utils::logTime("end kill VM");
  return;
}

sub restoreVMs($) {
  $branch = $_[0];
  $snap_local = "$seq.branch.$branch";
  print "======== Load ========\n";
  Utils::logTime("start load VM");
  system ("./kvm_scripts/SnapshotManager.pl load 1 $GatlingConfig::num_vms $snap_local $snp");
  Utils::logTime("end load VM");
  return;
}

1;
