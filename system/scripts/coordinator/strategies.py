#Samuel Jero <sjero@purdue.edu>
#vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
import os
import sys
import time
from datetime import datetime
import re
import pprint
from types import NoneType
import ast
import manipulations
import fields

system_home = os.path.split(os.path.dirname(os.path.realpath(__file__)))[0]
import config

class StrategyGenerator:
        #Constructor
        def __init__(self, lg, res_lg):
            self.lg = lg
            self.results = res_lg
            self.ck_file = None
            self.do_ckpt = False

            self.pkt_actions = dict()
            self.strat_list = []
            self.tested_list = []
            self.unique_tested = 0
            self.bw_threshold = None
            self.connection_threshold = None
            self.bw_collection = []
            self.connection_collection = []

        def next_strategy(self):
            if len(self.strat_list) == 0:
                self.checkpoint()
                return None
            if len(self.strat_list) % 10 == 0:
                print "Strategies Left: %d" % (len(self.strat_list))
                self.lg.write("[%s] Strategies Left: %d" % (str(datetime.today()),len(self.strat_list)))
            if len(self.strat_list) % 100 == 0: 
                self.checkpoint()
            return self.strat_list.pop(0)

        def return_strategy(self, strat):
            self.strat_list.insert(0,strat)

        def report_results(self, strat, info, perf):
            bw,conn = perf

            if strat.find("NONE") >= 0:
                #Set Performance Thresholds
                self.bw_collection.append(bw)
                self.connection_collection.append(conn)
                self.bw_threshold = sum(self.bw_collection)/len(self.bw_collection)
                self.connection_threshold = sum(self.connection_collection)/len(self.connection_collection)
    
            #Build info
            d = dict()
            d['strat'] = strat
            d['bw'] = bw
            d['conn'] = conn
            d['info'] = info
            d['bw_thres'] = self.bw_threshold
            d['conn_thres'] = self.connection_threshold
            d['time'] = str(datetime.today())

            #Determine if attack
            if self._is_attack(bw,conn):
                d['result'] = "FAILED"
                #check if this is a retry
                retried = False
                for t in self.tested_list:
                    if t['strat'] == strat:
                        retried = True
                        break
                
                #Retry one time
                if not retried:
                    self.strat_list.insert(0,strat)
                    self.lg.write("[%s] Strategy \"%s\" Failed! RETRYING\n" % (str(datetime.today()),strat))
                    self.lg.flush()
                else:
                    self.results.write("%s\n"%(repr(d)))
                    self.results.flush()
                    self.lg.write("[%s] Strategy \"%s\" Failed! RECORDING\n" % (str(datetime.today()),strat))
                    self.lg.flush()
                    print "[%s] Strategy \"%s\" Failed! RECORDING\n" % (str(datetime.today()),strat)
                    self.unique_tested += 1
            else:
                d['result'] = "PASSED"
                self.unique_tested += 1

            #Add to strategy list
            self.tested_list.append(d)

            self._process_feedback(strat,info)
            return

        def _is_attack(self,bw,conn):
            if self.bw_threshold == None or self.connection_threshold == None:
                print "Warning: Performance Thresholds not established yet!!"
                return False
            if bw < 15:
                return True
            if bw > 1.5*self.bw_threshold:
                return True
            if bw < 0.5*self.bw_threshold:
                return True
            if conn > self.connection_threshold:
                return True
            return False

        def _process_feedback(self,strat,fb):
            adding = False
            fb = self._build_fb_dict(fb)
            if 'server' not in fb:
                return
            for state in fb['server']:
                for metric in fb['server'][state]:
                    if metric.find("r_pkt_cnt_") >= 0:
                        pkt = metric.replace("r_pkt_cnt_","")
                        if pkt in self.pkt_actions:
                            if not self.pkt_actions[pkt]['fw_manip_testing']:
                                self.pkt_actions[pkt]['fw_manip_testing'] = True
                                for s in self.pkt_actions[pkt]['manip_list']:
                                    strategy = "{st}?{d}?{act}".format(st=state,d="0",act=s)
                                    self.strat_list.append(strategy)
                                    adding = True
                    if metric.find("s_pkt_cnt_") >= 0:
                        pkt = metric.replace("s_pkt_cnt_","")
                        if pkt in self.pkt_actions:
                            if not self.pkt_actions[pkt]['rv_manip_testing']:
                                self.pkt_actions[pkt]['rv_manip_testing'] = True
                                for s in self.pkt_actions[pkt]['manip_list']:
                                    strategy = "{st}?{d}?{act}".format(st=state,d="1",act=s)
                                    self.strat_list.append(strategy)
                                    adding = True

            if adding:
                print "Strategies: %d" % (len(self.strat_list) + self.unique_tested)
                self.lg.write("[%s] Strategies: %d" % (str(datetime.today()),len(self.strat_list)+self.unique_tested))
                self.checkpoint()
            return


        def _build_fb_dict(self, fb):
            d = dict()
            for l in fb:
                parts = l.split(",")
                if len(parts) != 4:
                    continue
                metric = parts[0]
                host = parts[1]
                state = parts[2]
                num = parts[3]

                if host not in d:
                    d[host] = dict()
                if state not in d[host]:
                    d[host][state] = dict()
                d[host][state][metric] = num
            return d

        def build_strategies(self):
            #Generate Packet Actions
            for k in fields.packet_format:
                p = fields.packet_format[k]
                if p['name'] not in self.pkt_actions:
                    self.pkt_actions[p['name']] = dict()
                    self.pkt_actions[p['name']]['fw_manip_testing'] = False
                    self.pkt_actions[p['name']]['rv_manip_testing'] = False
                    self.pkt_actions[p['name']]['manip_list'] = []
                    self.pkt_actions[p['name']]['inject_list'] = []
                self._build_inject_strats(p)
                i = 0
                for f in p['fields']:
                    if 'bitfield' in f:
                        for s in manipulations.bit_field_manipulations[f['bitfield']]:
                            strat = "{msg} {act} {field}".format(msg=p['name'],act=s,field=i)
                            self.pkt_actions[p['name']]['manip_list'].append(strat)
                    else:
                        for s in manipulations.field_manipulations[f['length']]:
                            strat = "{msg} {act} {field}".format(msg=p['name'],act=s,field=i)
                            self.pkt_actions[p['name']]['manip_list'].append(strat)
                    i +=1
            
            #Prime Strategies
            self.strat_list.append("*?*?BaseMessage NONE 0")
            self.strat_list.append("*?*?BaseMessage NONE 1")
            self.strat_list.append("*?*?BaseMessage NONE 2")
            for k in self.pkt_actions:
                for s in self.pkt_actions[k]['inject_list']:
                    self.strat_list.append("*?*?" + s)
            print "Initial Strategies: %d" % (len(self.strat_list))
            self.lg.write("[%s] Initial Strategies: %d" % (str(datetime.today()),len(self.strat_list)))


        def _build_inject_strats(self,p):
            if config.protocol == "TCP":
                strat = "{msg} INJECT t=10 0 {cip} {sip} 0={cp} 1={sp} 2=111 5=5 10={win}".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
                strat = "{msg} INJECT t=10 0 {sip} {cip} 0={sp} 1={cp} 2=111 5=5 10={win}".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
                strat = "{msg} WINDOW w={win} t=10 {cip} {sip} 0={cp} 1={sp} 5=5".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
                strat = "{msg} WINDOW w={win} t=10 {sip} {cip} 0={sp} 1={cp} 5=5".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
            elif config.protocol == "DCCP":
                size = 0
                if p['name'] == "BaseMessage":
                    size = 3
                elif p['name'] == "Data":
                    size = 4
                elif p['name'] == "Request":
                    size = 5
                elif p['name'] == "Reset":
                    size = 7
                elif p['name'] == "Response":
                    size = 7
                else:
                    size = 6
                strat = "{msg} INJECT t=10 0 {cip} {sip} 0={cp} 1={sp} 2={sz} 6=1 11=111".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window,sz=size)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
                strat = "{msg} INJECT t=10 0 {sip} {cip} 0={sp} 1={cp} 2={sz} 6=1 11=111".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window,sz=size)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
                strat = "{msg} WINDOW w={win} t=10 {cip} {sip} 0={cp} 1={sp} 2={sz} 6=1".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window,sz=size)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
                strat = "{msg} WINDOW w={win} t=10 {sip} {cip} 0={sp} 1={cp} 2={sz} 6=1".format(msg=p['name'],cip=config.client_ip,sip=config.server_ip,cp=config.client_port,sp=config.server_port,win=config.default_window,sz=size)
                self.pkt_actions[p['name']]['inject_list'].append(strat)
            else:
                print "Warning: Unknown protocol, no Injection strategies generated!"


        def enable_checkpointing(self, f):
            self.ck_file = f
            self.do_ckpt = True
            self.checkpoint()

        def checkpoint(self):
            if self.do_ckpt and self.ck_file is not None:
                self.lg.write("[%s] Making Checkpoint\n" % (str(datetime.today())))
                print "[%s] Making Checkpoint" % (str(datetime.today()))

                #Create backup
                bkup = {}
                bkup['version'] = 0
                bkup['pkt_actions'] = self.pkt_actions
                bkup['strat_list'] = self.strat_list
                bkup['tested_list'] = self.tested_list
                bkup['unique_tested'] = self.unique_tested
                bkup['bw_threshold'] = self.bw_threshold
                bkup['connection_threshold'] = self.connection_threshold
                bkup['bw_collection'] = self.bw_collection
                bkup['connection_collection'] = self.connection_collection

                #Format
                pp = pprint.PrettyPrinter()
                fmtbkup = pp.pformat(bkup)

                #Write backup
                try:
                    self.ck_file.seek(0)
                    self.ck_file.truncate()
                    self.ck_file.write(fmtbkup)
                    self.ck_file.flush()
                except Exception as e:
                    print "[%s] Checkpoint Failed: %s" % (str(datetime.today()),str(e))
                    return
                self.lg.write("[%s] Checkpoint Finished\n" % (str(datetime.today())))
                print "[%s] Checkpoint Finished" % (str(datetime.today()))

        def restore(self, f):
            #Read backup
            try:
                inp = f.readlines()
                inp = "\n".join(inp)
                bkup = ast.literal_eval(inp)
            except Exception as e:
                print "[%s] Failed to read checkpoint: %s" % (str(datetime.today()),str(e))
                f.close()
                return False

            #Restore Backup
            if bkup['version'] != 0:
                print "Warning: Checkpoint is incompatable!!!"
                f.close()
                return False
            self.pkt_actions = bkup['pkt_actions']
            self.strat_list = bkup['strat_list']
            self.tested_list = bkup['tested_list']
            self.unique_tested = bkup['unique_tested']
            self.bw_threshold = bkup['bw_threshold']
            self.connection_threshold = bkup['connection_threshold']
            self.bw_collection = bkup['bw_collection']
            self.connection_collection = bkup['connection_collection']

            f.close()
            self.lg.write("[%s] Restore Finished\n" % (str(datetime.today())))
            print "[%s] Restore Finished" % (str(datetime.today()))
            return True

