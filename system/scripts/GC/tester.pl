#!/usr/bin/env perl
#Simple pre-encoded strategies---for debug/closer inspection
use strict;
use POSIX;
use Switch;

#print "1:*?*?DataAck INJECT t=10 0 10.1.2.3  10.1.2.2 0=5002 1=5050 2=6 6=1 11=111\n";
#print "1:*?*?DataAck WINDOW w=20000 t=10 10.1.2.3 10.1.2.2 0=5002 1=5050 2=6 6=1\n";
#print "1:*?*?Request WINDOW w=20000 t=10 10.1.2.2 10.1.2.3 0=5050 1=5002 6=1 2=5\n";
#print "1:*?1?DataAck BURST 1.0\n";
#print "1:*?*?DataAck DELAY 1.0\n";
#print "1:*?*?DataAck DROP 50\n";
#print "1:*?*?DataAck DUP 10\n";
#print "1:*?*?DataAck LIE =0 1\n";
#print "1:LISTEN?1?Request LIE =0 0\n";
#print "1:*?*?DataAck LIE =0 3\n";
#print "1:OPEN?*?DataAck LIE =0 2\n";
#print "1:*?*?DataAck LIE =0 4\n";
#print "1:*?*?DataAck LIE =0 6\n";
#print "1:*?*?DataAck LIE =0 7\n";
#print "1:*?*?DataAck LIE =111 9\n";
#print "1:*?*?DataAck LIE =111 10\n";
#print "1:*?*?DataAck LIE =111 11\n";
