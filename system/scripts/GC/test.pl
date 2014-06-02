#!/usr/bin/perl

use strict;
use POSIX;
my $str = "";
foreach my $arg (@ARGV) {
    $str .= "$arg";
}
my $i;
#for ($i = 1; $i < 3; $i++) {
    my $n = ceil(rand(100));
print "$n:$str$n\n";
#}
