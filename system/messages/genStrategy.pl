#!/usr/bin/perl -w

use strict;

open(FILE, $ARGV[0]);
my @msgName;
my $parsingMsg = 0;
my @fieldName;
my $name;
my $flen;

print "BaseMessage NONE 0\n";
print "BaseMessage NONE 0\n";

while(<FILE>) {
	s/;//;
	my @token = split();
	#skip empty or commented out lines
	if ($#token < 0 or $token[0] =~ /^\/\//) {
		next;
	}


	if ($parsingMsg == 0) {
		push(@msgName, $token[0]);
		$name = $token[0];
		$parsingMsg = 1;
		@fieldName = ();
		print "$name DROP 0\n";
		print "$name DUP 2\n";
		print "$name DELAY 1.0\n";
		print "$name DIVERT 0\n";

	} else {

		if ($token[0] eq "}") {
			$parsingMsg = 0;
		} else {

			my $field = $token[1];

			#Handle bitfields
			$flen="";
			if($field =~ /:/){
				my @tmp=split(/:/,$field);
				$field=$tmp[0];
				$flen=$tmp[1];
			}
			push(@fieldName, $field);

			if($flen eq "1") {
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =1 $#fieldName\n";
			}elsif($flen eq "4"){
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =2 $#fieldName\n";
				print "$name LIE =3 $#fieldName\n";
				print "$name LIE =4 $#fieldName\n";
				print "$name LIE =8 $#fieldName\n";
			}elsif($flen eq "5"){
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =2 $#fieldName\n";
				print "$name LIE =4 $#fieldName\n";
				print "$name LIE =8 $#fieldName\n";
				print "$name LIE =16 $#fieldName\n";
				print "$name LIE =31 $#fieldName\n";
			}elsif ($token[0] eq "int8_t") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =-128 $#fieldName\n";
				print "$name LIE =128 $#fieldName\n";
				print "$name LIE =-32 $#fieldName\n";
				print "$name LIE =32 $#fieldName\n";
				print "$name LIE =16 $#fieldName\n";
				print "$name LIE =-16 $#fieldName\n";
			} elsif ($token[0] eq "uint8_t") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =257 $#fieldName\n";
				print "$name LIE =128 $#fieldName\n";
				print "$name LIE =64 $#fieldName\n";
				print "$name LIE =32 $#fieldName\n";
				print "$name LIE =16 $#fieldName\n";
				print "$name LIE =8 $#fieldName\n";
			} elsif ($token[0] eq "int16_t" or $token[0] eq "short") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =-32767 $#fieldName\n";
				print "$name LIE =32767 $#fieldName\n";
				print "$name LIE =-1024 $#fieldName\n";
				print "$name LIE =1024 $#fieldName\n";
				print "$name LIE =-128 $#fieldName\n";
				print "$name LIE =128 $#fieldName\n";
			} elsif ($token[0] eq "uint16_t") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =65535 $#fieldName\n";
				print "$name LIE =16384 $#fieldName\n";
				print "$name LIE =4096 $#fieldName\n";
				print "$name LIE =1024 $#fieldName\n";
				print "$name LIE =256 $#fieldName\n";
				print "$name LIE =64 $#fieldName\n";
			} elsif ($token[0] eq "int32_t" or $token[0] eq "int") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =-2147483647 $#fieldName\n";
				print "$name LIE =2147483647 $#fieldName\n";
				print "$name LIE =-131072 $#fieldName\n";
				print "$name LIE =131072 $#fieldName\n";
				print "$name LIE =-4096 $#fieldName\n";
				print "$name LIE =4096 $#fieldName\n";
			} elsif ($token[0] eq "uint32_t" or $token[0] eq "unsigned") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =4294967295 $#fieldName\n";
				print "$name LIE =134217728 $#fieldName\n";
				print "$name LIE =4194304 $#fieldName\n";
				print "$name LIE =131072 $#fieldName\n";
				print "$name LIE =4096 $#fieldName\n";
				print "$name LIE =128 $#fieldName\n";
			} elsif ($token[0] eq "int64_t") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =-9223372036854775807LL $#fieldName\n";
				print "$name LIE =9223372036854775807LL $#fieldName\n";
				print "$name LIE =-17179869184 $#fieldName\n";
				print "$name LIE =17179869184 $#fieldName\n";
				print "$name LIE =-16777216 $#fieldName\n";
				print "$name LIE =16777216 $#fieldName\n";
			} elsif ($token[0] eq "uint64_t") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =4294967295 $#fieldName\n";
				print "$name LIE =134217728 $#fieldName\n";
				print "$name LIE =4194304 $#fieldName\n";
				print "$name LIE =9223372036854775807LL $#fieldName\n";
				print "$name LIE =17179869184 $#fieldName\n";
				print "$name LIE =16777216 $#fieldName\n";
			} elsif ($token[0] eq "double") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =-1E+37 $#fieldName\n";
				print "$name LIE =1E+37 $#fieldName\n";
				print "$name LIE =1000000000 $#fieldName\n";
				print "$name LIE =-1000000000 $#fieldName\n";
				print "$name LIE =1000000000000000000 $#fieldName\n";
				print "$name LIE =-1000000000000000000 $#fieldName\n";
			} elsif ($token[0] eq "float") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =-1E+37 $#fieldName\n";
				print "$name LIE =1E+37 $#fieldName\n";
				print "$name LIE =1000000000 $#fieldName\n";
				print "$name LIE =-1000000000 $#fieldName\n";
				print "$name LIE =1000000000000000000 $#fieldName\n";
				print "$name LIE =-1000000000000000000 $#fieldName\n";
			} elsif ($token[0] eq "bool") {
				print "$name LIE r $#fieldName\n";
				print "$name LIE =0 $#fieldName\n";
				print "$name LIE =1 $#fieldName\n";
			}  


			
		}

	}
	
}

close(FILE);


