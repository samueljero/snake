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
  my $start = 1 + $GatlingConfig::offset;
  my $end = $GatlingConfig::num_vms + $GatlingConfig::offset;
  system ("./kvm_scripts/SnapshotManager.pl save $start $end $snap $GatlingConfig::specialVMoption");
  $snp+=1;
  Utils::logTime("end snapshot VM");
  return;
}

sub pauseVMs() {
  print "======== Pause ========\n";
  Utils::logTime("start pause VM");
  my $start = 1 + $GatlingConfig::offset;
  my $end = $GatlingConfig::num_vms + $GatlingConfig::offset;
  system ("./kvm_scripts/SnapshotManager.pl pause $start $end $GatlingConfig::specialVMoption");
  Utils::logTime("end pause VM");
  return;
}

sub resumeVMs() {
  print "======== Resume ========\n";
  Utils::logTime("start resume VM");
  my $start = 1 + $GatlingConfig::offset;
  my $end = $GatlingConfig::num_vms + $GatlingConfig::offset;
  system ("./kvm_scripts/SnapshotManager.pl resume $start $end $GatlingConfig::specialVMoption");
  Utils::logTime("end resume VM");
  return;
}

sub killVMs() {
  print "======== Kill ========\n";
  my $start = 1 + $GatlingConfig::offset;
  my $end = $GatlingConfig::num_vms + $GatlingConfig::offset;
  Utils::logTime("start kill VM");
  system ("./kvm_scripts/SnapshotManager.pl kill $start $end $GatlingConfig::specialVMoption");
  system ("ps -ef|grep kvm");
  Utils::logTime("end kill VM");
  return;
}

sub restoreVMs($) {
  $branch = $_[0];
  $snap_local = "$seq.branch.$branch";
  print "======== Load ========\n";
  my $start = 1 + $GatlingConfig::offset;
  my $end = $GatlingConfig::num_vms + $GatlingConfig::offset;
  Utils::logTime("start load VM");
  system ("./kvm_scripts/SnapshotManager.pl load $start $end $snap_local $snp $GatlingConfig::specialVMoption");
  Utils::logTime("end load VM");
  return;
}

1;
