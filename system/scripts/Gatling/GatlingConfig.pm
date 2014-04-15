#!/usr/bin/perl

use Cwd 'abs_path';
use File::Basename;

package GatlingConfig;

## Configurations
#pick VM env
$VM = "KVM";

#$VM = "XEN";

#$basedir = "";
$basedir = Cwd::abs_path(File::Basename::dirname(__FILE__));

## if NS3 is started already, set 0
$exit_when_break = 0;
$systemname = "";
$startNS3 = 1;
$systemname = "";
$watch_ns3 = 1;
$watch_turret = 1;
$brokenPerf = 9999;
$debug = 1;
$formatFile = "";
$scoreFile = "$basedir/score.client";
$tmpFile = "$basedir/tmp";
$format_dir = "$basedir/../../messages";
$statediagramFile = "diagram.dot";
#$formatFile = "$format_dir/steward.format";
$window_size = 7;
$learning_threashold = 2;
$start_attack = 1;
$server_command = "./counter.pl | ~/logcollector.pl";
$client_command = "./counter.pl | ~/logcollector.pl ";
$serverList = "pssh_servers.txt";
$clientList = "pssh_clients.txt";
$s_parallel = 4;
$c_parallel = 1;
$num_vms = 8;
$runTime = 2000000;
$waitTime = 0;
$watchPort = " ";
$mal = " -mal 0 ";
$timeLog = "timing.txt";
$alreadyLearned = "pre_learned.txt";
$prePerf = "pre_perf.txt";
$perfMeasured = "perfMeasured.txt";
$newlyLearned = "new_learned.txt";
$serverhavessh = 1;
$setupCommand = "";

sub changeServerCommand($@) {
	$server_command = $_[0];
}

sub prepare()
{
  system("pkill serverLog");
  system("pkill clientLog");
  system("screen -S serverLog -p 0 -X stuff '\ncd $basedir/..;\n./serverLog.pl\n'");
  system("screen -S serverLog -p 1 -X stuff '\ncd $basedir/..;\n./clientLogUpdate.pl\n'");
}

sub runSystem()
{
	system("pssh -p $s_parallel -h $serverList \"pkill server\"");
	system("pssh -p $c_parallel -h $clientList \"pkill client\"");
	print "About to execute server command: $server_command\n";
	system("pssh -p $s_parallel -h $serverList -t $runTime \"$server_command\"  &");
}

sub runClient()
{
	print "About to execute client command: $client_command\n";
	system("pssh -p $c_parallel -h $clientList \"pkill client\"");
	system("pssh -p $c_parallel -h $clientList -t $runTime \"$client_command &\" &");
}

sub setSystem() 
{
  print "System $systemname\n";
  if ($systemname eq "BFT") {
    systemBFT();
  }
  if ($systemname eq "Prime") {
    systemPrime();
  }
  if ($systemname eq "Steward") {
    systemSteward();
  }
  # Endadul
  if ($systemname eq "Prime_bug") {
    systemPrime_bug();
  }
  if($systemname eq "TCP"){
	systemTCP();
  }
  system("$setupCommand");
}

sub systemBFT()
{
  $formatFile = "$format_dir/bft.format";
  $server_command = "cd /root/BFT/bin; stdbuf -i 0 -o 0 ./run.sh | ~/logcollector.pl";
  $client_command = "cd /root/BFT/bin; stdbuf -i 0 -o 0 ./run_client.sh | ~/perfcollector.pl ";
  $serverList = "pssh_servers.txt";
  $clientList = "pssh_clients.txt";
  #$s_parallel = 7;
  $s_parallel = 4;
  $c_parallel = 1;
  #$num_vms = 8;
  $num_vms = 5;
  $runTime = 100000;
  #$mal = " -mal 0 -mal 1 ";
  $mal = " -mal 0 ";
  $watchPort = " -udp_port 3669 ";
  $alreadyLearned = "BFT/pre_learned.txt";
  $prePerf = "BFT/pre_perf.txt";
  $perfMeasured = "BFT/perf.txt";
  $newlyLearned = "BFT/new_learned.txt";
  $setupCommand = "mkdir BFT; cp pre*.txt BFT";
  $systemname = "BFT";
}

sub systemPrime()
{
  $systemname = "Prime";
  $formatFile = "$format_dir/prime.format";
  $setupCommand = "mkdir Prime";
  $server_command = "cd /root/Prime/bin/; stdbuf -i 0 -o 0 ./run.sh | ~/logcollector.pl";
  $client_command = "cd /root/Prime/bin/; stdbuf -i 0 -o 0 ./run_client.sh |  ~/perfcollector.pl";
  $serverList = "pssh_servers.txt";
  $clientList = "pssh_clients.txt";
  $s_parallel = 4;
  $c_parallel = 1;
  $runTime = 100;
  #$watchPort = "  -udp_port 7100 -udp_port 7101 -udp_port 7102 -udp_port 7200 -udp_port 7250 -udp_port 7300 -udp_port 7350 -udp_port 7400 -udp_port 8900 -udp_port 7401 -udp_port 7402 -udp_port 7403 -udp_port 7301 -udp_port 7302 -udp_port 7303 -udp_port 7201 -tcp_port 7103 -udp_port 7251 -udp_port 7252 -udp_port 7203 -udp_port 7253 -udp_port 7204 -udp_port 7254 -udp_port 7202 ";
  $watchPort = " -udp_port 7200 -udp_port 7250 -udp_port 7300 ";
  $mal = " -mal 1 ";
  #$mal = " -mal 0  -mal 1";
  $alreadyLearned = "Prime/pre_learned.txt";
  $prePerf = "Prime/pre_perf.txt";
  $perfMeasured = "Prime/perf.txt";
  $newlyLearned = "Prime/new_learned.txt";
  $systemname = "Prime";
}

# Endadul
sub systemPrime_bug()
{
  # $systemname = "Prime_bug";
  $formatFile = "../messages/prime.format";
  $setupCommand = "mkdir Prime_bug; cp pre_learn* Prime_bug;";
  $server_command = "cd /root/Prime_bug/bin/; stdbuf -i 0 -o 0 ./run.sh | ~/logcollector.pl";
  $client_command = "cd /root/Prime_bug/bin/; stdbuf -i 0 -o 0 ./run_client.sh |  ~/perfcollector.pl";
  $serverList = "pssh_servers.txt";
  $clientList = "pssh_clients.txt";
  $s_parallel = 4;
  $c_parallel = 1;
  $runTime = 100;
  #$watchPort = "  -udp_port 7100 -udp_port 7101 -udp_port 7102 -udp_port 7200 -udp_port 7250 -udp_port 7300 -udp_port 7350 -udp_port 7400 -udp_port 8900 -udp_port 7401 -udp_port 7402 -udp_port 7403 -udp_port 7301 -udp_port 7302 -udp_port 7303 -udp_port 7201 -tcp_port 7103 -udp_port 7251 -udp_port 7252 -udp_port 7203 -udp_port 7253 -udp_port 7204 -udp_port 7254 -udp_port 7202 ";
  $watchPort = " -udp_port 7200 -udp_port 7250 -udp_port 7300 ";
  $mal = " -mal 0 ";
  #$mal = " -mal 0  -mal 1";
  $alreadyLearned = "Prime_bug/pre_learned.txt";
  $prePerf = "Prime_bug/pre_perf.txt";
  $perfMeasured = "Prime_bug/perf.txt";
  $newlyLearned = "Prime_bug/new_learned.txt";
  $systemname = "Prime_bug";
}
sub movePrevPerf()
{
  print ("cat $perfMeasured >> $prePerf\n");
  system ("cat $perfMeasured >> $prePerf\n");
  system ("cat $newlyLearned >> $alreadyLearned\n");
}

sub systemSteward()
{
  $formatFile = "../messages/steward.format";
  $setupCommand = "mkdir Steward";
  $server_command = "cd /root/Steward/bin/; stdbuf -i 0 -o 0 ./run.sh | ~/logcollector.pl";
  $client_command = "cd /root/Steward/bin/; stdbuf -i 0 -o 0 ./run_client.sh |  ~/perfcollector.pl";
  $serverList = "pssh_servers.txt";
  $clientList = "pssh_clients.txt";
  $s_parallel = 12;
  $c_parallel = 1;
  $runTime = 100;
  $watchPort = " -udp_port 100 ";
  #$mal = " -mal 0 -mal 4 -mal 8 ";
  $mal = " -mal 1 -mal 5 -mal 9 ";
  #$mal = " -mal 0  -mal 1";
  $alreadyLearned = "Steward/pre_learned.txt";
  $prePerf = "Steward/pre_perf.txt";
  $perfMeasured = "Steward/perf.txt";
  $newlyLearned = "Steward/new_learned.txt";
  $num_vms = 13;
}

sub systemTCP()
{
  $setupCommand = "mkdir TCP";
  $server_command = "";
  $client_command = "/root/TCP/client.sh";
  $serverList = "pssh_servers.txt";
  $clientList = "pssh_clients.txt";
  $runTime = 60;
  $waitTime = 60;
  $watchPort = " -tcp_port 80";
  $mal = " -mal 0 ";
  $alreadyLearned = "TCP/pre_learned.txt";
  $prePerf = "TCP/pre_perf.txt";
  $perfMeasured = "TCP/perf.txt";
  $newlyLearned = "TCP/new_learned.txt";
  $num_vms = 4;
  $window_size = 30;
  $learning_threashold = 1;
  $formatFile = "$format_dir/tcp.format";
  $start_attack = 1;
  $brokenPerf = 99999999;
  $s_parallel = 2;
  $c_parallel = 2;
  $statediagramFile = "$format_dir/tcp.dot";
  $serverhavessh=1;
}

1;
