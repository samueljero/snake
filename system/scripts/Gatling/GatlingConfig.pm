#!/usr/bin/env perl

use Cwd 'abs_path';
use File::Basename;
use Sys::Hostname;
use Socket;

package GatlingConfig;

## Global collector
$GlobalCollectorAddr = "localhost";
$GlobalCollectorPort = 9991;
$useGlobal = 0;
$attackModule = "Executor";
$host = Sys::Hostname::hostname();
my $ip=gethostbyname($host);
$ListenAddr = Socket::inet_ntoa($ip);
$ListenPort = 9992;

## VM Configurations
#pick VM env
$VM = "KVM";
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
$capture_packets = 0;
$start_attack = 1;
$learning_threashold = 2;

## Protocol Settings
$formatFile = "";
$scoreFile = "$basedir/score.client";
$resourcesFile = "$basedir/resources.client";
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
$tcpdump_cmd = "tcpdump";
print STDERR "hostname: $host\n";
if ( $host =~ /^sound/ or $host =~ /^ocean1/ ) { #because we can only use 10.0.X.X... sigh
    $baseIP = "10.0.1";
    $tcpdump_cmd = "sudo /usr/local/sbin/tcpdumpwrap"; #this doesn't work very well with timeout... sigh
}
$NS3_IP_base="10.1.2";
$offset=0;
$s_parallel = 4;
$c_parallel = 1;
$num_vms = 4;
$specialVMoption = "";
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
$state_dir = "$basedir/";
$timeLog = "$basedir/../timing.txt";
$alreadyLearned = "pre_learned.txt";
$prePerf = "pre_perf.txt";
$perfMeasured = "perfMeasured.txt";
$newlyLearned = "new_learned.txt";
$packet_cap_len = 64;

sub changeServerCommand($@) {
	$server_command = $_[0];
}

sub prepare()
{
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
  if($systemname eq "TCP"){
	systemTCP();
  }
  if($systemname eq "DCCP"){
  	systemDCCP();
  }
  system("$setupCommand");
  makeNS3Com();
}

sub movePrevPerf()
{
  print STDERR ("cat $perfMeasured >> $prePerf\n");
  system ("cat $perfMeasured >> $prePerf\n");
  system ("cat $newlyLearned >> $alreadyLearned\n");
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
  $state_dir = "$basedir/../TCP/";
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
  $packet_cap_len = 64;


  $clientip = "10.1.2.2";
  $serverip = "10.1.2.3";
  $malip = "10.1.2.1";

  $clientport= 5555;
  $serverport= 80;
  $malport = 5556;
  $defaultwindow=20000;
  $useGlobal=1;
  $specialVMoption = "";
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
  $state_dir = "$basedir/../DCCP/";
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
  $packet_cap_len = 120;
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
    my $tmp = 3 + $offset;
    $hostserverip="$baseIP.$tmp";
    offsetScoreFile();
    print  STDERR "using $serverListString $host\n";
}

sub offsetScoreFile {
    $scoreFile = "$basedir/score_$offset.client";
    $resourcesFile = "$basedir/resources_$offset.client";

}

1;
