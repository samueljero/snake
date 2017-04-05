#!/bin/env python
# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
# Samuel Jero <sjero@purdue.edu>
# Checkpoint parser
import sys
import os
import re
import argparse
import ast


def main(args):
        #Parse Args
        argp = argparse.ArgumentParser(description='Checkpoint viewer')
        argp.add_argument('out_file', help="Checkpoint File")
        args = vars(argp.parse_args(args[1:]))

        #Open Results file
        f = None
        try:    
                f = open(args['out_file'], "r")
        except Exception as e:
                print "Error: could not open %s" % (args['out_file'])
                sys.exit(1)

        lines = f.readlines()
        lines = "\n".join(lines)
        f.close()

        try:
                cp = ast.literal_eval(lines)
        except Exception as e:
                print e
                return 0

        strats = []
        if 'strat_list' in cp:
            strats = cp['strat_list']

        if 'tested_list' in cp:
            strats += cp['tested_list']

        for s in strats:
            if type(s) is str:
                print s
            if type(s) is dict:
                print s['strat']
                if s['strat'].find("NONE") >=0:
                    for i in s['info']:
                        print "\t",i
        
        print "Num Strategies: ", len(strats)


        return 0


if __name__ == "__main__":
        main(sys.argv)
