#!/bin/env python
# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
# Samuel Jero <sjero@purdue.edu>
# Results viewer
import sys
import os
import re
import argparse
import ast


def main(args):
        #Parse Args
        argp = argparse.ArgumentParser(description='Results viewer')
        argp.add_argument('out_file', help="Output File")
        args = vars(argp.parse_args(args[1:]))

        #Open Results file
        resultfile = None
        try:    
                resultfile = open(args['out_file'], "r")
        except Exception as e:
                print "Error: could not open %s" % (args['out_file'])
                sys.exit(1)

        for line in resultfile:
            line = line.strip()
            if line[0] == "#":
                print line
                continue

            result = None
            try:
                    result = ast.literal_eval(line)
            except Exception as e:
                    print e
                    continue

            if 'strat' not in result:
                continue
            if 'bw' not in result:
                continue
            if 'conn' not in result:
                continue
            if 'conn_thres' not in result:
                continue
            if 'bw_thres' not in result:
                continue
            strat = result['strat']
            bw = result['bw']
            conn = result['conn']
            conn_thres = result['conn_thres']
            bw_thres = result['bw_thres']

            print strat + "," + str(bw) + "," + str(conn)


        return 0


if __name__ == "__main__":
        main(sys.argv)
