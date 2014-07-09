#!/usr/bin/env perl

use Cwd 'abs_path';
use File::Basename;
use Sys::Hostname;
use Socket;

package GatlingConfig;

## Global collector
$GlobalCollectorAddr = "128.10.132.182";
$GlobalCollectorPort = 9991;
$useGlobal = 0;
#$attackModule = "StateBasedAttack";
$attackModule = "Executor";
$host = Sys::Hostname::hostname();
my $ip=gethostbyname($host);
$ListenAddr = Socket::inet_ntoa($ip);
$ListenPort = 9992;

## VM Configurations
#pick VM env
$VM = "KVM";
#$VM = "XEN";
$basedir = Cwd::abs_path(File::Basename::dirname(__FILE__));

## Implementation Settings
$exit_when_break = 0;
$systemname = "";
$startNS3 = 1;
$systemname = "";
$watch_ns3 = 1;
$watch_turret = 1;
$brokenPerf = 9999;
$debug = 1;
$start_attack = 1;
$learning_threashold = 2;

## Protocol Settings
$formatFile = "";
$scoreFile = "$basedir/score.client";
$tmpFile = "$basedir/tmp";
$format_dir = "$basedir/../../messages";
$statediagramFile = "diagram.dot";

##Testing Settings
$window_size = 7;
$server_command = "./counter.pl | ~/logcollector.pl";
$client_command = "./counter.pl | ~/logcollector.pl ";
$serverListString = "";
$clientListString = "";
$allListString = "";
push (@clientNumbers, 1);
push (@clientNumbers, 2);
push (@serverNumbers, 3);
push (@serverNumbers, 4);
$baseIP = "10.1.1";
print STDERR "hostname: $host\n";
if ( $host =~ /^sound/ or $host =~ /^ocean1/ ) { #because we can only use 10.0.X.X... sigh
    $baseIP = "10.0.1";
}
$NS3_IP_base="10.1.2";
$offset=0;
$s_parallel = 4;
$c_parallel = 1;
$num_vms = 4;
$runTime = 2000000;
$waitTime = 0;
$watchPort = " ";
@malNumbers;
push (@malNumbers, 0);
$malString = "";
$serverhavessh = 1;
$NumConnsCmd = "echo t; echo t; echo t";
$setupCommand = "";
my $tmp = 3 + $offset;
$hostserverip="$baseIP.$tmp";

##NS-3 Intercept Settings
$clientip = "10.1.2.2";
$serverip = "10.1.2.3";
$malip = "10.1.2.1";
$clientport= 5555;
$serverport= 80;
$malport = 5556;
$defaultwindow=20000;

#Logging Settings
$timeLog = "$basedir/../timing.txt";
$alreadyLearned = "pre_learned.txt";
$prePerf = "pre_perf.txt";
$perfMeasured = "perfMeasured.txt";
$newlyLearned = "new_learned.txt";

sub changeServerCommand($@) {
	$server_command = $_[0];
}

sub prepare()
{
  system("cd $basedir/..;./perfCollector.pl -offset $offset 2>&1 > /dev/null &");
}

sub runSystem()
{
	print STDERR ("pssh -p $s_parallel $serverListString \"pkill server\"\n");
	system("pssh -p $s_parallel $serverListString \"pkill server\"");
	system("pssh -p $c_parallel $clientListString \"pkill client\"");
	print STDERR "About to execute server command: $server_command\n";
	system("pssh -p $s_parallel $serverListString -t $runTime \"$server_command\"  &");
}

sub runClient()
{
	print  STDERR "About to execute client command: $client_command\n";
	system("pssh -p $c_parallel $clientListString \"pkill client\"");
	system("pssh -p $c_parallel $clientListString -t $runTime \"$client_command &\" &");
}

sub setSystem() 
{
  print STDERR "System $systemname\n";
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
  if($systemname eq "DCCP"){
  	systemDCCP();
  }
  system("$setupCommand");
  makeNS3Com();
}

sub systemBFT()
{
  $formatFile = "$format_dir/bft.format";
  $server_command = "cd /root/BFT/bin; stdbuf -i 0 -o 0 ./run.sh | ~/logcollector.pl";
  $client_command = "cd /root/BFT/bin; stdbuf -i 0 -o 0 ./run_client.sh | ~/perfcollector.pl ";
  undef(@serverNumbers);
  push (@serverNumbers, 1);
  push (@serverNumbers, 2);
  push (@serverNumbers, 3);
  push (@serverNumbers, 4);
  undef(@clientNumbers);
  push (@clientNumbers, 5);
  $s_parallel = 4;
  $c_parallel = 1;
  $num_vms = 5;
  $runTime = 100000;
  undef(@malNumbers);
  push (@malNumbers, 0);
  $watchPort = " -port 3669 ";
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
  undef(@serverNumbers);
  push (@serverNumbers, 1);
  push (@serverNumbers, 2);
  push (@serverNumbers, 3);
  push (@serverNumbers, 4);
  undef(@clientNumbers);
  push (@clientNumbers, 5);
  $s_parallel = 4;
  $c_parallel = 1;
  $runTime = 100;
  #$watchPort = "  -port 7100 -port 7101 -port 7102 -port 7200 -port 7250 -port 7300 -port 7350 -port 7400 -port 8900 -port 7401 -port 7402 -port 7403 -port 7301 -port 7302 -port 7303 -port 7201 -port 7103 -port 7251 -port 7252 -port 7203 -port 7253 -port 7204 -port 7254 -port 7202 ";
  $watchPort = " -port 7200 -port 7250 -port 7300 ";
  undef(@malNumbers);
  push (@malNumbers, 1);
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
  undef(@serverNumbers);
  push (@serverNumbers, 1);
  push (@serverNumbers, 2);
  push (@serverNumbers, 3);
  push (@serverNumbers, 4);
  undef(@clientNumbers);
  push (@clientNumbers, 5);
  $s_parallel = 4;
  $c_parallel = 1;
  $runTime = 100;
  #$watchPort = "  -port 7100 -port 7101 -port 7102 -port 7200 -port 7250 -port 7300 -port 7350 -port 7400 -port 8900 -port 7401 -port 7402 -port 7403 -port 7301 -port 7302 -port 7303 -port 7201 -port 7103 -port 7251 -port 7252 -port 7203 -port 7253 -port 7204 -port 7254 -port 7202 ";
  $watchPort = " -port 7200 -port 7250 -port 7300 ";
  undef(@malNumbers);
  push (@malNumbers, 0);
  $alreadyLearned = "Prime_bug/pre_learned.txt";
  $prePerf = "Prime_bug/pre_perf.txt";
  $perfMeasured = "Prime_bug/perf.txt";
  $newlyLearned = "Prime_bug/new_learned.txt";
  $systemname = "Prime_bug";
}
sub movePrevPerf()
{
  print STDERR ("cat $perfMeasured >> $prePerf\n");
  system ("cat $perfMeasured >> $prePerf\n");
  system ("cat $newlyLearned >> $alreadyLearned\n");
}

sub systemSteward()
{
  $formatFile = "../messages/steward.format";
  $setupCommand = "mkdir Steward";
  $server_command = "cd /root/Steward/bin/; stdbuf -i 0 -o 0 ./run.sh | ~/logcollector.pl";
  $client_command = "cd /root/Steward/bin/; stdbuf -i 0 -o 0 ./run_client.sh |  ~/perfcollector.pl";
  undef(@serverNumbers);
  push (@serverNumbers, 1);
  push (@serverNumbers, 2);
  push (@serverNumbers, 3);
  push (@serverNumbers, 4);
  undef(@clientNumbers);
  push (@clientNumbers, 5);
  $s_parallel = 12;
  $c_parallel = 1;
  $runTime = 100;
  $watchPort = " -port 100 ";
  undef(@malNumbers);
  push (@malNumbers, 1);
  push (@malNumbers, 5);
  push (@malNumbers, 9);
  $alreadyLearned = "Steward/pre_learned.txt";
  $prePerf = "Steward/pre_perf.txt";
  $perfMeasured = "Steward/perf.txt";
  $newlyLearned = "Steward/new_learned.txt";
  $num_vms = 13;
}

sub systemTCP()
{
  $setupCommand = "mkdir $basedir/../TCP";
  $server_command = "";
  $client_command = "/root/TCP/client.sh $offset";
  undef(@serverNumbers);
  push (@serverNumbers, 3);
  push (@serverNumbers, 4);
  undef(@clientNumbers);
  push (@clientNumbers, 1);
  push (@clientNumbers, 2);
  print STDERR "using $serverList $host\n";
  $runTime = 90;
  $window_size=70;
  $waitTime = 60;
  $watchPort = " -tcp_port 80 -port 80";
  undef(@malNumbers);
  push (@malNumbers, 0);
  $alreadyLearned = "$basedir/../TCP/pre_learned.txt";
  $prePerf = "$basedir/../TCP/pre_perf.txt";
  $perfMeasured = "$basedir/../TCP/perf.txt";
  $newlyLearned = "$basedir/../TCP/new_learned.txt";
  $num_vms = 4;
  $learning_threashold = 1;
  $formatFile = "$format_dir/tcp.format";
  $start_attack = 1;
  $brokenPerf = 99999999;
  $s_parallel = 2;
  $c_parallel = 2;
  $statediagramFile = "$format_dir/tcp.dot";
  $serverhavessh=1;
  $NumConnsCmd = "netstat --inet --inet6 -n";
  $clientip = "10.1.2.2";
  $serverip = "10.1.2.3";
  $malip = "10.1.2.1";
  $clientport= 5555;
  $serverport= 80;
  $malport = 5556;
  $defaultwindow=20000;
  $useGlobal=1;
}

sub systemDCCP()
{
  $setupCommand = "mkdir $basedir/../DCCP";
  $server_command = "/root/DCCP/server.sh $offset";
  $client_command = "/root/DCCP/client.sh $offset";
  undef(@serverNumbers);
  push (@serverNumbers, 3);
  push (@serverNumbers, 4);
  undef(@clientNumbers);
  push (@clientNumbers, 1);
  push (@clientNumbers, 2);
  $runTime = 90;
  $window_size=70;
  $waitTime = 60;
  undef(@malNumbers);
  push (@malNumbers, 0);
  $watchPort = " -port 5001";
  $alreadyLearned = "$basedir/../DCCP/pre_learned.txt";
  $prePerf = "$basedir/../DCCP/pre_perf.txt";
  $perfMeasured = "$basedir/../DCCP/perf.txt";
  $newlyLearned = "$basedir/../DCCP/new_learned.txt";
  $num_vms = 4;
  $learning_threashold = 1;
  $formatFile = "$format_dir/dccp.format";
  $start_attack = 1;
  $brokenPerf = 99999999;
  $s_parallel = 2;
  $c_parallel = 2;
  $statediagramFile = "$format_dir/dccp.dot";
  $NumConnsCmd = "echo t;echo t; ss -d";
  $serverhavessh=1;
  $useGlobal=1;
  $clientip = "10.1.2.2";
  $serverip = "10.1.2.3";
  $malip = "10.1.2.1";
  $clientport= 5050;
  $serverport= 5002;
  $malport = 5050;
  $defaultwindow=20000;
}

sub formPsshStrings {
    foreach my $i (@serverNumbers) {
        $i = $i + $offset;
        $serverListString .= " --host=root\@$baseIP.$i";
        $allListString .= " --host=root\@$baseIP.$i";
    }
    foreach my $i (@clientNumbers) {
        $i = $i + $offset;
        $clientListString .= " --host=root\@$baseIP.$i";
        $allListString .= " --host=root\@$baseIP.$i";
    }
    foreach my $i (@malNumbers) {
        $i = $i;# + $offset;
        $malString .= " -mal $i";
    }
   
}

sub makeNS3Com {
    formPsshStrings();
    $NS3_command = "./run_command.sh \"malproxy_simple $malString -num_vms $num_vms -ip_base $NS3_IP_base -tap_base tap-ns $watchPort -offset $offset -runtime $runTime\"";
    $ListenPort = $ListenPort + $offset;
    $scoreFile = "$basedir/score_$offset.client";
    print  STDERR "using $serverListString $host\n";
}

1;
