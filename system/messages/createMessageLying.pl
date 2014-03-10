#!/usr/bin/perl -w

use strict;

open(DOTH, ">message.h");
open(DOTC, ">message.cc");
open(STRAT, ">strategy");

my $addToSize = 0;

if ($ARGV[0] eq "upright.format") {
  $addToSize = 4;
}

print STRAT "BaseMessage NONE 0\nBaseMessage NONE 0\n";

# print out boilerplate headers of files
print DOTH <<END;
#include "ns3/uinteger.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#ifndef MESSAGE_H
#define MESSAGE_H
END

print DOTC <<END;
#include "message.h"

Message::Message(uint8_t* m) {
	//std::cout << "Entering Message" << std::endl;
	msg = m;
	type = FindMsgType();
	size = FindMsgSize();

	encMsg = NULL;
	uint8_t *ptr = EncMsgOffset();
	if (ptr != NULL) {
		encMsg = new Message(ptr);
	}
	//std::cout << "Exiting Message" << std::endl;
}

int Message::FindMsgType() {
	return ((BaseMessage*)msg)->type;
}

int Message::FindMsgSize() {
	return ((BaseMessage*)msg)->size + $addToSize;	
}

END

my @msgName;
my @msgType;
my $maxField = 0;

open(FILE, $ARGV[0]);

my $parsing = 0;
my $fieldNum = 0;
my $fieldOffset;
my %fieldToType;
my $needOffset = 0;
my $flen = "";
my $field = "";
my $type = "";
my $index = "1";
my $indexField = "";
my %struct;
my $structName;
my $name;
my %encMsgOffset;


while(<FILE>) {
	s/;//;
	my @token = split();
#skip empty or commented out lines
	if ($#token < 0 or $token[0] =~ /^\/\//) {
		next;
}

#if ($token[0] eq "#define") {
#	print "$_\n";
#	next;
#}

#make sure first message is base message type
if ($parsing == 0 and $#msgName == -1 and $token[0] ne "BaseMessage") {
	print STDERR "First message type is not BaseMessage!\n";
	exit;
}
$flen= "";
$field = "";
$type = "";
$index = "1";
$indexField = "";

if ($parsing == 0) {
	if ($token[0] eq "struct") {
		$parsing = 1;
		$structName = $token[1];
		print DOTH "$token[0] $token[1] {\n";

	} else {
		push(@msgName, $token[0]);
		$name = $token[0];
		$parsing = 2;
		print DOTH "typedef struct {\n";
		print DOTC "void Message::Change$name(int field, char* value) {\n";
		print DOTC "\t//std::cout << \"Entering Change$name\" << std::endl;\n";
		print DOTC "\t$name *ptr = ($name*)msg;\n\n";
		print STRAT "$name DROP 100\n";
		print STRAT "$name DUP 2\n";
		print STRAT "$name DELAY 1.0\n";
		print STRAT "$name DIVERT 0\n";
		print STRAT "$name REPLAY 0 $name DIVERT 0\n";
		$fieldNum = -1;
		%fieldToType = ();
		$fieldOffset = "msg";
		$needOffset = 0;
	}
} elsif ($parsing == 1) {
#parsing a struct
	if ($token[0] eq "}") {
		$parsing = 0;
		print DOTH "};\n\n";
	} else {
		if ($#token == 1) {
			$type = $token[0];
		} else {
			$type = join(" ", @token[0,-2]);
		}
		$field = $token[-1];
		push(@{$struct{$structName}}, $type);
		push(@{$struct{$structName}}, $field); 
		print DOTH "\t$type $field;\n";
	}
} elsif ($parsing == 2) {
#parsing a msg

	if ($token[0] eq "}") {

		$parsing = 0;
		print DOTH "} $msgName[$#msgName];\n\n";
		print DOTC "\t//std::cout << \"Exiting Change$name\" << std::endl;\n";
		print DOTC "}\n\n";

	} else {
		my $j;
		for ($j = 0; $j <= $#token; $j++) {
			if ($token[$j] eq "=") {
				last;
			}
		}
		if ($j-2 == 0) {
			$type = $token[0];
		} else {
			$type = join(" ", @token[0,$j-2]);
		}

		if ($token[$j-1] =~ /\[/) {
			$field = substr($token[$j-1], 0, index($token[$j-1], "["));
			$index = substr($token[$j-1], index($token[$j-1], "[")+1);
			$index = substr($index, 0, index($index, "]"));
			unless ($index =~ m/^\d+$/) {
				$indexField = $index;
				$index = "ptr->$index";
				$needOffset = 1;
			}
		} else {
			$field = $token[$j-1];
		}

		#Handle bitfields
		if($field =~ /:/){
			my @tmp=split(/:/,$field);
			$field=$tmp[0];
			$flen=$tmp[1];
		}

		if ($field eq "type") {
			if ($#token >= 3) {
				push(@msgType, $token[3]);
			} else {
				print STDERR "$msgName[$#msgName] does not include type number!\n";
				exit;
			}
		}

		if ($type =~ /^BaseMessage/) {
			#assuming this would be last thing in message...
			$encMsgOffset{$name} = $fieldOffset;

		} elsif ($type =~ /^struct/) {
			my @vars = @{$struct{$token[1]}};
			for (my $j = 0; $j <= $#vars; $j += 2) {
				$type = $vars[$j];
				$field = $vars[$j+1];
				$fieldToType{$field} = $type;
				$fieldNum += 1;
				printStrategy();
				if ($fieldNum > $maxField) {
					$maxField = $fieldNum;
				}
				if (!$needOffset) {
					print DOTH "\t$type $field;\n";
				}

				print DOTC "\tif (field == $fieldNum) {\n";
				print DOTC "\t\tstruct $token[1] *cur ";

				if (!$needOffset) {
					print DOTC "= &(ptr->$token[2]);\n";
				} else {
					print DOTC "= (struct $token[1]*)($fieldOffset);\n";
				}


				my $tabs = "\t\t";
				if ($index ne "1" and $type ne "char") {
					print DOTC "$tabs";
					if ($indexField eq "") {
						print DOTC "for (int i = 0; i < $index; i++) {\n";
					} else {
						print DOTC "for ($fieldToType{$indexField} i = 0; i < $index; i++) {\n";
					}
					$tabs .= "\t";
				}

				my $tempIndex = 1;
				my $baseField;
				if ($field =~ /\[/) {
					$baseField = substr($field, 0, index($field, "["));
					my $length = index($field, "]") - index($field, "[") - 1;
					$tempIndex = substr($field, index($field, "[")+1, $length);
#print "$tempField $length $tempIndex\n";
				}

				if ($type eq "char") {
					print DOTC "$tabs";
					if ($tempIndex == 1) {
						print DOTC "cur->$field = value;\n";
					} else {
						print DOTC "strncpy(cur->$baseField, value, $tempIndex);\n";
					}
				} else {
					for (my $k = 0; $k < $tempIndex; $k++) {
						if ($tempIndex > 1) {
							$field = "$baseField". "[$k]";
						}

						if ($type eq "double" or $type eq "float") {
							print DOTC "$tabs";
							print DOTC "if (value[0] == '=') {\n";
							print DOTC "$tabs\tcur->$field = ($type)atof(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '+') {\n";
							print DOTC "$tabs\tcur->$field += ($type)atof(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '-') {\n";
							print DOTC "$tabs\tcur->$field -= ($type)atof(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '*') {\n";
							print DOTC "$tabs\tcur->$field *= ($type)atof(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '/') {\n";
							print DOTC "$tabs\tcur->$field /= ($type)atof(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == 'r') {\n";
							print DOTC "$tabs\tcur->$field = ($type)rand();\n";
							print DOTC "$tabs";
							print DOTC "}\n";
						} elsif($type eq "uint16_t"){
							print DOTC "$tabs$type tmp=ntohs(*cur);\n";
							print DOTC "$tabs";
							print DOTC "if (value[0] == '=') {\n";
							print DOTC "$tabs\ttmp = ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '+') {\n";
							print DOTC "$tabs\ttmp += ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '-') {\n";
							print DOTC "$tabs\ttmp -= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '*') {\n";
							print DOTC "$tabs\ttmp *= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '/') {\n";
							print DOTC "$tabs\ttmp /= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == 'r') {\n";
							print DOTC "$tabs\ttmp = ($type)rand();\n";
							print DOTC "$tabs";
							print DOTC "}\n";
							print DOTC "$tabs *cur=htons(tmp);\n";
						} elsif($type eq "uint32_t"){
							print DOTC "$tabs$type tmp=ntohl(*cur);\n";
							print DOTC "$tabs";
							print DOTC "if (value[0] == '=') {\n";
							print DOTC "$tabs\ttmp = ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '+') {\n";
							print DOTC "$tabs\ttmp += ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '-') {\n";
							print DOTC "$tabs\ttmp -= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '*') {\n";
							print DOTC "$tabs\ttmp *= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '/') {\n";
							print DOTC "$tabs\ttmp /= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == 'r') {\n";
							print DOTC "$tabs\ttmp = ($type)rand();\n";
							print DOTC "$tabs";
							print DOTC "}\n";
							print DOTC "$tabs *cur=htonl(tmp);\n";
						} else {
							print DOTC "$tabs";
							print DOTC "if (value[0] == '=') {\n";
							print DOTC "$tabs\tcur->$field = ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '+') {\n";
							print DOTC "$tabs\tcur->$field += ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '-') {\n";
							print DOTC "$tabs\tcur->$field -= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '*') {\n";
							print DOTC "$tabs\tcur->$field *= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == '/') {\n";
							print DOTC "$tabs\tcur->$field /= ($type)atoi(value+1);\n";
							print DOTC "$tabs";
							print DOTC "} else if (value[0] == 'r') {\n";
							print DOTC "$tabs\tcur->$field = ($type)rand();\n";
							print DOTC "$tabs";
							print DOTC "}\n";
						}
					}
					if ($index ne "1" and $type ne "char") {
						print DOTC "\t\t\tcur++;\n";
						print DOTC "\t\t}\n";
					}
				}
#print DOTC "\t\treturn;\n";
				print DOTC "\t}\n";

			}
			$fieldOffset .= "+sizeof(struct $token[1])";
			if ($index ne "1") {
				$fieldOffset .= "*$index";
			}

		} else {

			$fieldToType{$field} = $type;
			$fieldNum += 1;
			printStrategy();
			if ($fieldNum > $maxField) {
				$maxField = $fieldNum;
			}

			print DOTC "\tif (field == $fieldNum) {\n";
			if($flen eq ""){
				print DOTC "\t\t$type *cur ";
			}

			if (!$needOffset) {
				if ($index eq "1") {
					print DOTH "\t$type $field";
					if($flen ne ""){
						print DOTH " : $flen";
					}
					print DOTH ";\n";
					if($flen eq ""){
						print DOTC "= &(ptr->$field);\n";
					}

				} else {
					print DOTH "\t$type $field"; 
					print DOTH "[$index];\n";
					print DOTC "= ptr->$field;\n";
				}
			} else {
				print DOTC "= ($type*)($fieldOffset);\n";
			}

			$fieldOffset .= "+sizeof($type)";
			if ($index ne "1") {
				$fieldOffset .= "*$index";
			}


			my $tabs = "\t\t";
			if ($index ne "1" and $type ne "char") {
				print DOTC "$tabs";
				if ($indexField eq "") {
					print DOTC "for (int i = 0; i < $index; i++) {\n";
				} else {
					print DOTC "for ($fieldToType{$indexField} i = 0; i < $index; i++) {\n";
				}
				$tabs .= "\t";
			}
			if ($type eq "char") {
				print DOTC "$tabs";
				print DOTC "strncpy(cur, value, $index);\n";
			} elsif ($type eq "double" or $type eq "float") {
				print DOTC "$tabs";
				print DOTC "if (value[0] == '=') {\n";
				print DOTC "$tabs\t*cur = ($type)atof(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '+') {\n";
				print DOTC "$tabs\t*cur += ($type)atof(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '-') {\n";
				print DOTC "$tabs\t*cur -= ($type)atof(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '*') {\n";
				print DOTC "$tabs\t*cur *= ($type)atof(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '/') {\n";
				print DOTC "$tabs\t*cur /= ($type)atof(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == 'r') {\n";
				print DOTC "$tabs\t*cur = ($type)rand();\n";
				print DOTC "$tabs";
				print DOTC "}\n";
			} elsif($flen ne ""){
				print DOTC "$tabs";
				print DOTC "if (value[0] == '=') {\n";
				print DOTC "$tabs\tptr->$field = ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '+') {\n";
				print DOTC "$tabs\tptr->$field += ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '-') {\n";
				print DOTC "$tabs\tptr->$field -= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '*') {\n";
				print DOTC "$tabs\tptr->$field *= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '/') {\n";
				print DOTC "$tabs\tptr->$field /= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == 'r') {\n";
				print DOTC "$tabs\tptr->$field = ($type)rand();\n";
				print DOTC "$tabs";
				print DOTC "}\n";
			} elsif($type eq "uint16_t"){
				print DOTC "$tabs$type tmp=ntohs(*cur);\n";
				print DOTC "$tabs";
				print DOTC "if (value[0] == '=') {\n";
				print DOTC "$tabs\ttmp = ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '+') {\n";
				print DOTC "$tabs\ttmp += ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '-') {\n";
				print DOTC "$tabs\ttmp -= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '*') {\n";
				print DOTC "$tabs\ttmp *= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '/') {\n";
				print DOTC "$tabs\ttmp /= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == 'r') {\n";
				print DOTC "$tabs\ttmp = ($type)rand();\n";
				print DOTC "$tabs";
				print DOTC "}\n";
				print DOTC "$tabs *cur=htons(tmp);\n";
			} elsif($type eq "uint32_t"){
				print DOTC "$tabs$type tmp=ntohl(*cur);\n";
				print DOTC "$tabs";
				print DOTC "if (value[0] == '=') {\n";
				print DOTC "$tabs\ttmp = ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '+') {\n";
				print DOTC "$tabs\ttmp += ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '-') {\n";
				print DOTC "$tabs\ttmp -= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '*') {\n";
				print DOTC "$tabs\ttmp *= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '/') {\n";
				print DOTC "$tabs\ttmp /= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == 'r') {\n";
				print DOTC "$tabs\ttmp = ($type)rand();\n";
				print DOTC "$tabs";
				print DOTC "}\n";
				print DOTC "$tabs *cur=htonl(tmp);\n";
			} else {
				print DOTC "$tabs";
				print DOTC "if (value[0] == '=') {\n";
				print DOTC "$tabs\t*cur = ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '+') {\n";
				print DOTC "$tabs\t*cur += ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '-') {\n";
				print DOTC "$tabs\t*cur -= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '*') {\n";
				print DOTC "$tabs\t*cur *= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == '/') {\n";
				print DOTC "$tabs\t*cur /= ($type)atoi(value+1);\n";
				print DOTC "$tabs";
				print DOTC "} else if (value[0] == 'r') {\n";
				print DOTC "$tabs\t*cur = ($type)rand();\n";
				print DOTC "$tabs";
				print DOTC "}\n";
			}
			if ($index ne "1" and $type ne "char") {
				print DOTC "\t\t\tcur++;\n";
				print DOTC "\t\t}\n";
			}
#print DOTC "\t\treturn;\n";
			print DOTC "\t}\n";
		}
	}

}

}


close(FILE);

print DOTC <<END;
void Message::ChangeValue(int field, char* value) {
	//std::cout << "Entering ChangeValue " << type << " " << field << " " << value << std::endl;
	switch (type) {
END

print DOTH "enum MessageType {";
for (my $i = 0; $i <= $#msgName; $i++) {
	my $temp = uc $msgName[$i];
	print DOTH "$temp=$msgType[$i]";
	if ($i != $#msgName) {
		print DOTH ", ";
	}
	print DOTC "\t\tcase $temp:\n\t\t\tChange$msgName[$i](field, value);\n\t\t\tbreak;\n";

}
print DOTH "};\n\n";
print DOTC "\t}\n";
print DOTC "\t//std::cout << \"Exiting ChangeValue\" << std::endl;\n";
print DOTC "}\n\n";


print DOTH <<END;
class Message {
	public:

	uint8_t *msg;
	int type;
	int size;

	Message *encMsg;

	Message(uint8_t* msg);

	int FindMsgType();
	int FindMsgSize(); 
	uint8_t* EncMsgOffset();
	static int StrToType(const char* str);
	static std::string TypeToStr(int type);
	void ChangeValue(int field, char* value);
END

print DOTC "int Message::StrToType(const char *str) {\n";
print DOTC "\t//std::cout << \"Entering StrToType\" << std::endl;\n";

for (my $i = 0; $i <= $#msgName; $i++) {
	print DOTH "\tvoid Change$msgName[$i](int field, char* value);\n";
	print DOTC "\tif (strcmp(str, \"$msgName[$i]\") == 0) {\n";
	my $temp = uc $msgName[$i];
	print DOTC "\t\treturn $temp;\n\t}\n";
}

print DOTC "\t//std::cout << \"Exiting StrToType\" << std::endl;\n";
print DOTC "\treturn -1;\n}\n\n";

print DOTC "std::string Message::TypeToStr(int type){\n";
print DOTC "\t//std::cout << \"Entering TypeToStr\" << std::endl;\n";
for (my $i = 0; $i <= $#msgName; $i++) {
	my $temp = uc $msgName[$i];
	print DOTC "\tif (type == $temp ) {\n";
	print DOTC "\t\treturn \"$msgName[$i]\";\n\t}\n";
}
print DOTC "\t//std::cout << \"Exiting TypeToStr\" << std::endl;\n";
print DOTC "\treturn \"Invalid\";\n}\n\n";

print DOTC "uint8_t* Message::EncMsgOffset() {\n";
print DOTC "\t//std::cout << \"Entering EncMsgOffset\" << std::endl;\n";

for (my $i = 0; $i <= $#msgName; $i++) {
	my $temp = uc $msgName[$i];
	print DOTC "\tif (type == $temp)\n";
	if (!exists $encMsgOffset{$msgName[$i]}) {
		print DOTC "\t\t\treturn NULL;\n";	
	} else {
		print DOTC "\t\t\treturn $encMsgOffset{$msgName[$i]};\n";
	}
}

print DOTC "\t//std::cout << \"Exiting EncMsgOffset\" << std::endl;\n";
print DOTC "\treturn NULL;\n}\n\n";

print DOTH "\tvoid CreateMessage(int type, const char *spec);\n";
print DOTC "void Message::CreateMessage(int type, const char *spec){\n";
print DOTC "\t//std::cout<< \"Entering CreateMessage\"<<std::endl;\n";
print DOTC "\tint field;\n";
print DOTC "\tchar fspec[1000];\n";
print DOTC "\tint len, ret;\n\n\n";
print DOTC "\tthis->type=type;\n";
print DOTC "\t((BaseMessage*)msg)->type=type;\n\n";
print DOTC "\tfspec[0]='=';\n";
print DOTC "\tret=sscanf(spec, \"%i=%999s%n\",&field,&fspec[1],&len);\n";
print DOTC "\tspec+=len;\n";
print DOTC "\twhile(ret!=EOF){\n";
print DOTC "\t\tChangeValue(field,fspec);\n";
print DOTC "\t\tret=sscanf(spec, \"%i=%999s%n\",&field,&fspec[1],&len);\n";
print DOTC "\t\tspec+=len;\n";
print DOTC "\t};\n";
print DOTC "\t//std::cout<< \"Exiting CreateMessage\"<<std::endl;\n";
print DOTC "};\n\n";

print DOTH "};\n#endif\n\n";

my $numMsg = $#msgName+1;
print DOTH "#define MSG $numMsg\n#define FIELD $maxField\n\n";


close(DOTH);
close(DOTC);
close(STRAT);


sub printStrategy {
	if($flen eq "1") {
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =1 $fieldNum\n";
	}elsif($flen eq "4"){
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =2 $fieldNum\n";
		print STRAT "$name LIE =3 $fieldNum\n";
		print STRAT "$name LIE =4 $fieldNum\n";
		print STRAT "$name LIE =8 $fieldNum\n";
	}elsif($flen eq "5"){
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =2 $fieldNum\n";
		print STRAT "$name LIE =4 $fieldNum\n";
		print STRAT "$name LIE =8 $fieldNum\n";
		print STRAT "$name LIE =16 $fieldNum\n";
		print STRAT "$name LIE =31 $fieldNum\n";
	}elsif ($type eq "int8_t") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =-128 $fieldNum\n";
		print STRAT "$name LIE =128 $fieldNum\n";
		print STRAT "$name LIE =-32 $fieldNum\n";
		print STRAT "$name LIE =32 $fieldNum\n";
		print STRAT "$name LIE =16 $fieldNum\n";
		print STRAT "$name LIE =-16 $fieldNum\n";
	} elsif ($type eq "uint8_t") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =257 $fieldNum\n";
		print STRAT "$name LIE =128 $fieldNum\n";
		print STRAT "$name LIE =64 $fieldNum\n";
		print STRAT "$name LIE =32 $fieldNum\n";
		print STRAT "$name LIE =16 $fieldNum\n";
		print STRAT "$name LIE =8 $fieldNum\n";
	} elsif ($type eq "int16_t" or $type eq "short") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =-32767 $fieldNum\n";
		print STRAT "$name LIE =32767 $fieldNum\n";
		print STRAT "$name LIE =-1024 $fieldNum\n";
		print STRAT "$name LIE =1024 $fieldNum\n";
		print STRAT "$name LIE =-128 $fieldNum\n";
		print STRAT "$name LIE =128 $fieldNum\n";
	} elsif ($type eq "uint16_t") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =65535 $fieldNum\n";
		print STRAT "$name LIE =16384 $fieldNum\n";
		print STRAT "$name LIE =4096 $fieldNum\n";
		print STRAT "$name LIE =1024 $fieldNum\n";
		print STRAT "$name LIE =256 $fieldNum\n";
		print STRAT "$name LIE =64 $fieldNum\n";
	} elsif ($type eq "int32_t" or $type eq "int") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =-2147483647 $fieldNum\n";
		print STRAT "$name LIE =2147483647 $fieldNum\n";
		#print STRAT "$name LIE =-131072 $fieldNum\n";
		#print STRAT "$name LIE =131072 $fieldNum\n";
		print STRAT "$name LIE =-4096 $fieldNum\n";
		print STRAT "$name LIE =4096 $fieldNum\n";
	} elsif ($type eq "uint32_t" or $type eq "unsigned" or $type eq "unsigned int") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =4294967295 $fieldNum\n";
		#print STRAT "$name LIE =134217728 $fieldNum\n";
		#print STRAT "$name LIE =4194304 $fieldNum\n";
		print STRAT "$name LIE =131072 $fieldNum\n";
		print STRAT "$name LIE =4096 $fieldNum\n";
		print STRAT "$name LIE =128 $fieldNum\n";
	} elsif ($type eq "int64_t") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =-9223372036854775807LL $fieldNum\n";
		#print STRAT "$name LIE =9223372036854775807LL $fieldNum\n";
		print STRAT "$name LIE =-17179869184 $fieldNum\n";
		#print STRAT "$name LIE =17179869184 $fieldNum\n";
		#print STRAT "$name LIE =-16777216 $fieldNum\n";
		print STRAT "$name LIE =16777216 $fieldNum\n";
	} elsif ($type eq "uint64_t") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		#print STRAT "$name LIE =4294967295 $fieldNum\n";
		print STRAT "$name LIE =134217728 $fieldNum\n";
		#print STRAT "$name LIE =4194304 $fieldNum\n";
		print STRAT "$name LIE =9223372036854775807LL $fieldNum\n";
		print STRAT "$name LIE =17179869184 $fieldNum\n";
		#print STRAT "$name LIE =16777216 $fieldNum\n";
	} elsif ($type eq "double") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =-1E+37 $fieldNum\n";
		print STRAT "$name LIE =1E+37 $fieldNum\n";
		print STRAT "$name LIE =1000000000 $fieldNum\n";
		print STRAT "$name LIE =-1000000000 $fieldNum\n";
		#print STRAT "$name LIE =1000000000000000000 $fieldNum\n";
		#print STRAT "$name LIE =-1000000000000000000 $fieldNum\n";
	} elsif ($type eq "float") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =-1E+37 $fieldNum\n";
		print STRAT "$name LIE =1E+37 $fieldNum\n";
		print STRAT "$name LIE =1000000000 $fieldNum\n";
		print STRAT "$name LIE =-1000000000 $fieldNum\n";
		#print STRAT "$name LIE =1000000000000000000 $fieldNum\n";
		#print STRAT "$name LIE =-1000000000000000000 $fieldNum\n";
	} elsif ($type eq "bool") {
		print STRAT "$name LIE r $fieldNum\n";
		print STRAT "$name LIE =0 $fieldNum\n";
		print STRAT "$name LIE =1 $fieldNum\n";
	}

}
