#!/usr/bin/env perl

use strict;
package Executor;
require Utils;
require Perf;
require GatlingConfig;
use Cwd;
use List::Util qw(max min);
use IO::Socket::INET;


my @WaitingStrategyList :shared; # Queue of strategies waiting to execute



$SIG{TERM} = 'term_handler';
$SIG{KILL} = 'term_handler';

sub term_handler {
	print "term_handler\n";
	$GatlingConfig::watch_ns3 = 2;
	exit;
}



sub ns3_thread {
	my @args = @_;
	print "NS-3 start: [".(join (' ', @args))."]\n";
	system (join (' ', @args, "\n"));
	$GatlingConfig::watch_ns3 = 0;
	system ("pkill serverLog\n");
	system ("pkill clientLog\n");
	print "NS3 finished\n";
	exit();
}

sub tcpdump_thread {
	my $strat = shift;
	my $filename = Utils::make_file_name($strat);
	my $timeout = $GatlingConfig::window_size + 10;
	my $tap = 3 + $GatlingConfig::offset;
	my $interface = "tap-vm$tap";
	my $path = "$GatlingConfig::state_dir/pcap_$GatlingConfig::offset/$filename.dmp";
	system("timeout -2 $timeout $GatlingConfig::tcpdump_cmd -i $interface -s $GatlingConfig::packet_cap_len -w -  ip > $path");
	exit();
}

# strategy listener -- get strategies from the generator
my $quit :shared;

sub perf_collection_thread {
	GatlingConfig::offsetScoreFile();
	my $socknumber = 7779 + $GatlingConfig::offset;

	my $socket = new IO::Socket::INET (
			LocalHost => '10.0.0.1',
			LocalPort => $socknumber,
			Proto => 'udp',
			) or die "Error in Socket Creation for perfMonitor : $!\n";

	open (PERFFILE, ">$GatlingConfig::scoreFile");
	print PERFFILE "0\n";
	close (PERFFILE);
	print "perfMonitor listens on port $socknumber\n";

	my $line;
	while (not $quit)
	{
		$socket->recv($line, 1024);
		my @lines = split /\n/, $line;
		foreach my $eachline (@lines) {
			if($GatlingConfig::systemname eq "Prime"){
				Perf::Prime_bugPerf($eachline);
				#Perf::PrimePerf($eachline);
			}elsif($GatlingConfig::systemname eq "BFT"){
				Perf::BFTPerf($eachline);
			}elsif($GatlingConfig::systemname eq "Steward"){
				Perf::StewardPerf($eachline);
			}elsif($GatlingConfig::systemname eq "TCP"){
				Perf::TCP_Perf($eachline);
			}elsif($GatlingConfig::systemname eq "DCCP"){
				Perf::DCCP_Perf($eachline);
			}else{
				print "ERROR: Unknown system!\n";
			}
		}
	}
}

sub reportResult {
    my $sock = shift;
    my $strategy = shift;
    my $perfscore = shift;
    my $resourceusage = shift;

    #Print results for local monitor
    print "===perfScore $perfscore for strategy $strategy used $resourceusage\n";
    # send the result to the server
    Utils::reportGC($sock, "perf:$strategy:$perfscore:$resourceusage");
}

my $listenerStarted: shared;
sub strategyListener {
    print "======> strategyListener started\n";
    my $socket = new IO::Socket::INET (
            LocalHost => $GatlingConfig::ListenAddr,
            LocalPort => $GatlingConfig::ListenPort, # read from config
            Proto => 'tcp',
            Listen => 50,
            Reuse => 1
            ) or die "Error in Socket Creation for strategyListener : $!\n";
    
    print "====================== strategyListener is Listening at $GatlingConfig::ListenAddr:$GatlingConfig::ListenPort\n";
    while (1) 
    {
        $listenerStarted = 1;
        my $sock = $socket->accept();
        my $line = "";
        $sock->recv($line, 1024);
        chomp $line;
        push (@WaitingStrategyList, $line);
        if ($line =~ "STOP") {
	    print "Received Request to STOP... stopping\n";
            $quit = 1;
            last;
        }
        print "$line added to WaitingStrategyList, now $#WaitingStrategyList\n";
        print $sock "got it\n";
    }

    return;
}

# reporter -- report the result to the generator

sub start {
    ################  get ready
    my $pcap_thr;
    $quit = 0;
    my $sock;
    $listenerStarted = 0;
    my $strListener = threads->create('strategyListener', '');
    while ($listenerStarted != 1) {
        sleep 1;
    }
    $strListener->detach();

    my $perfListener = threads->create('perf_collection_thread','');
    $perfListener->detach();

    Utils::updateSnapshot(-1);
    Utils::pauseVMs();
    Utils::snapshotVMs();
    $sock = Utils::openGC();
    Utils::reportGC($sock,"ready");
    Utils::closeGC($sock);

    # ROUTINE STARTS
    while ($quit != 1) { # set by listener, GC should command
        if (@WaitingStrategyList) {
            my $strategy=shift(@WaitingStrategyList);
            my $attackscore;
            print "Current strategy: $strategy\n";

            #Setup VMs/NS-3	
            Utils::killVMs();
            print "3 seconds later, VMs will be loaded\n";
            sleep(1);
            Utils::restoreVMs(-1);
	    sleep(3);
            $GatlingConfig::watch_ns3=1;
            my $ns3_thr = threads->create( 'ns3_thread', $GatlingConfig::NS3_command);
            sleep ($GatlingConfig::start_attack);
            if($GatlingConfig::watch_ns3==0){
                print "PANIC: NS-3 didn't start!!\n";
                exit;
            }

            #Load Strategy
            print "Trying strategy $strategy...\n";
            my $fullpath = Cwd::abs_path($GatlingConfig::statediagramFile);
            my $command = "C GatlingLoadStateDiagram $fullpath";
            Utils::logTime("command $command");
            Utils::directTopology($command);
            $command = "C $strategy";
            Utils::logTime("command $command");
            Utils::directTopology($command);
            Utils::directTopology("C Gatling Resume");

	    if($GatlingConfig::capture_packets){
            	$pcap_thr = threads->create( 'tcpdump_thread', $strategy);
	    }

            #Start clients
            print "Starting System\n";
            GatlingConfig::runSystem();
	    sleep(1);
            GatlingConfig::runClient();
            Utils::logTime("system start");


            #Wait for NS-3 to do test
            sleep($GatlingConfig::window_size);

            #Measure State Information
            $command = "C GatlingSendStateStats";
            Utils::logTime("command $command");
            my $statstr=Utils::directTopology($command);
            print STDERR "Got StateStats: after $command --> $statstr\n";
            my %db=Utils::makeMetricDB($statstr);

            #Wait for system to normally close resources... i.e. TIMEWAIT
            print STDERR "sleep for $GatlingConfig::waitTime\n";
            sleep($GatlingConfig::waitTime);
            while($GatlingConfig::watch_ns3 !=0){
                print STDERR "sleep until ns3 done\n";
                sleep(1);
            }

            #Measure perf
            my $perfscore = Utils::getPerfScore();
            print STDERR "PerfScore $perfscore from $GatlingConfig::scoreFile\n";
            Utils::resetPerfScore();

            #Measure Resource Utilization
            my $resourceusage=0;
            if($GatlingConfig::serverhavessh==1){
                $resourceusage = Utils::getNumConnections("root\@$GatlingConfig::hostserverip");
            }else{
                $resourceusage = Utils::PingHost("$GatlingConfig::hostserverip");
            }

            # report result
	    $sock = Utils::openGC();
            reportResult($sock,$strategy, $perfscore, $resourceusage);

            #Join NS-3 Thread (so it goes away)
            $ns3_thr->join();
	    if($GatlingConfig::capture_packets){
            	$pcap_thr->join();
	    }

            # report additional information:
            #Debug State Results
	    foreach my $host (keys %db){
	        foreach my $metric (keys %{$db{$host}}){
		    foreach my $state (keys %{$db{$host}{$metric}}){
			if ( $GatlingConfig::debug > 0 ) {
			    print "$host,$metric,$state,$db{$host}{$metric}{$state}\n";
			}
			Utils::reportGC($sock,"info:$metric,$host,$state,$db{$host}{$metric}{$state}");
                    }
                }
            }
            Utils::reportGC($sock,"info:end");
        }
        else {
            print "no strategy in the queue.";
            print " waiting at $GatlingConfig::ListenAddr:$GatlingConfig::ListenPort ... $#WaitingStrategyList\n";
            # TODO wait signal (otherwise it will be busy waiting)
	    $sock = Utils::openGC();
            Utils::reportGC($sock,"ready");
            sleep 1;
    	}
	Utils::closeGC($sock);
    }

    # TODO think if there's any post processing needed
}

1;
