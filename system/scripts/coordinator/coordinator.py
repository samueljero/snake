#!/bin/env python
# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
# Samuel Jero <sjero@purdue.edu>
# Testing Coordinator

import os
import sys
import time
from datetime import datetime
import SocketServer as ss
import argparse
import threading
import socket
from strategies import StrategyGenerator

system_home = os.path.split(os.path.dirname(os.path.realpath(__file__)))[0]
log_path = os.path.abspath(os.path.join(system_home, 'TCP'))

# Threaded Server Class
class ThreadingTCPServer(ss.ThreadingMixIn, ss.TCPServer):
    pass

# Global Strategy Generator
strat_gen = None
strat_lock = threading.Lock()

# Global Logger
lg_lock = threading.Lock()
lg = None
res_lg = None

# Executor List
exec_lst = {}
exec_lst_lock = threading.Lock()


class ExecutorHandler(ss.StreamRequestHandler):
    
    # Called on every new TCP connection
    def handle(self):
        instance = ""
        init = False
        sending_address = ""
        perf = ""
        info = []

        while True:
            # Get Line
            line = ""
            try:
                line = self.rfile.readline()
            except Exception as e:
                print e
            if line == "" and init:
                # Connection Ended
                break

            #Parse Line
            if init is False:
                instance = line.strip()
                try:
                    sa,sp = instance.split(":")
                    sending_address = sa,int(sp)
                except Exception as e:
                    print "Error: Bad sending address!"
                    break
                init = True

                # Check if Executor is new
                exec_lst_lock.acquire()
                if instance not in exec_lst:
                    # New executor
                    lg_lock.acquire()
                    lg.write("[%s] New Executor: %s\n" % (str(datetime.today()), instance))
                    print "[%s] New Executor: %s" % (str(datetime.today()), instance)
                    lg_lock.release()
                    exec_lst[instance] = []
                exec_lst_lock.release()
            elif line.find("ready") >= 0:
                perf = ""
                info = []

                # Get Next Strategy
                strat_lock.acquire()
                strat = strat_gen.next_strategy()
                strat_lock.release()

                # Check if Finished
                if strat is None:
                    lg_lock.acquire()
                    lg.write("[%s] Finished Testing\n" % (str(datetime.today())))
                    print "[%s] Finished Testing" % (str(datetime.today()))
                    lg_lock.release()
                    msg = "STOP"
                else:
                    msg = strat
                
                # Send strategy
                lg_lock.acquire()
                lg.write("[%s] Executor (%s) testing strategy: %s\n" % (str(datetime.today()), instance, strat))
                print "[%s] Executor (%s) testing strategy: %s" % (str(datetime.today()), instance, strat)
                lg_lock.release()
                try:
                    sending_connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    sending_connection.connect(sending_address)
                    sending_connection.send("%s\n" % (msg))
                    sending_connection.close()
                except Exception as e:
                    print str(e)
                    # Connection Error!
                    if strat is not None:
                        strat_lock.acquire()
                        strat_gen.return_strategy(strat)
                        strat_lock.release()
                    exec_lst_lock.acquire()
                    del exec_lst[instance]
                    exec_lst_lock.release()
                    lg_lock.acquire()
                    lg.write("[%s] Executor (%s) failed\n" % (str(datetime.today()), instance))
                    print "[%s] Executor (%s) failed" % (str(datetime.today()), instance)
                    lg_lock.release()
                    break
            elif line.find("info:end") >= 0:
                try:
                    parts = perf.split(":")
                    strat = parts[1]
                    perf = (int(parts[2]),int(parts[3]))
                except Exception as e:
                    print e
                    continue

                if len(strat) == 0:
                    print "Error: Strategy got lost! Resync!"
                    break
                
                strat_lock.acquire()
                strat_gen.report_results(strat,info,perf)
                strat_lock.release()
            elif line.find("info") >= 0:
                try:
                    parts = line.strip().split(":")
                    info.append(parts[1])
                except Exception as e:
                    print e
            elif line.find("perf") >= 0:
                perf = line.strip()
            else:
                print "Error: unknown command \"%s\"" % (line.strip())
            lg_lock.acquire()
            lg.flush()
            lg_lock.release()


def main(args):
    global lg, res_lg, strat_gen
    # Parse Args
    argp = argparse.ArgumentParser(description='Testing Coordinator')
    argp.add_argument('-p', '--port', type=int, default=9991)
    argp.add_argument('-c', '--checkpoint', default=log_path + "/checkpoint.ck")
    argp.add_argument('-r', '--restore', action='store_true')
    argp.add_argument('-l', '--load', default="")
    args = vars(argp.parse_args(args[1:]))
    if args['restore'] == True:
        mode = "a"
    else:
        mode = "w"
    print "Starting Coordinator..."

    # Open Log file
    lg = open(log_path + "/coordinator.log", mode)
    lg.write("[%s] Starting Coordinator\n" % (str(datetime.today())))
    lg.flush()

    # Open Results File
    res_lg = open(log_path + "/results.log", mode)
    res_lg.write("#Started %s\n" % (str(datetime.today())))
    res_lg.flush()

    # Create Strategy Generator
    strat_gen = StrategyGenerator(lg,res_lg)

    # Initialize Strategy Generator
    print "Generating Strategies..."
    lg.write("[%s] Generating Strategies\n" % (str(datetime.today())))
    strat_gen.build_strategies()

    # Restore, if needed
    if args['restore'] == True:
        ck = open(args['checkpoint'], "r")
        if strat_gen.restore(ck) == False:
            return

    # Open Checkpoint file
    if len(args['checkpoint']) > 0:
        ck = open(args['checkpoint'], "w")
        strat_gen.enable_checkpointing(ck)

    # Start Server
    ThreadingTCPServer.allow_reuse_address = True
    server = ThreadingTCPServer(('', args['port']), ExecutorHandler)
    server.serve_forever()

    # Close log
    lg.write(str(datetime.today()) + "\n")
    lg.close()
    res_lg.close()

if __name__ == "__main__":
    main(sys.argv)
