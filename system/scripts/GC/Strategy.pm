#!/usr/bin/env perl

package Strategy;

################################################################################
# Strategies with level
# 1 - coarse grained strategy: tested when the message type is discovered
# 2 - fine grained strategy: tested only when there is at least a small change
# in attacking the field in similar states
################################################################################

# strategies
my %strategyCount;
my %coarseStrategyList; # these get one more parameter
my %fineStrategyList; # these get one more parameter

my %msgTypeList;
my %FlenList;
my %FlenNumList; # field num
my %fieldsPerMsg;
my %msgFlenList;

sub parseMessage {
    my $maxField = 0;
    my %msgType;
    my @test;

    my $parsing = 0;
    my $fieldNum = 0;
    my $fieldOffset;
    my %fieldToType;
    my $needOffset = 0;
    my $field = "";
    my $type = "";
    my $index = "1";
    my $indexField = "";
    my %struct;
    my $structName;
    my $name;
    my $flen;
    open FILE, $GatlingConfig::formatFile or die "Can't open format file [$GatlingConfig::formatFile\n]$!";

    while(<FILE>) {
        s/;//;
        my @token = split();
#skip empty or commented out lines
        if ($#token < 0 or $token[0] =~ /^\/\//) {
            next;
    }
    $field = "";
    $type = "";
    $index = "1";
    $indexField = "";
    if ($parsing == 0) {
        if ($token[0] eq "struct") {
            $parsing = 1;
            $structName = $token[1];
        }elsif($token[0] eq "DEFINE"){
            #ignore
        }else {
            push(@msgName, $token[0]);
            $name = $token[0];
            $parsing = 2;
            $fieldNum = -1;
            %fieldToType = ();
            $fieldOffset = "msg";
            $needOffset = 0;
        }
    } elsif ($parsing == 1) {
#parsing a struct
        if ($token[0] eq "}") {
            $parsing = 0;
        } else {
            if ($#token == 1) {
                $type = $token[0];
            } else {
                $type = join(" ", @token[0,-2]);
            }
            $field = $token[-1];
            push(@{$struct{$structName}}, $type);
            push(@{$struct{$structName}}, $field); 
        }
    } elsif ($parsing == 2) {
#parsing a msg
        if ($token[0] eq "}") {
            $parsing = 0;
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
            $flen=-1;
            if($field =~ /:/){
                my @tmp=split(/:/,$field);
                $field=$tmp[0];
                $flen=$tmp[1];
            }
            push(@{$msgFlenList{$#msgName}},$flen);

            if ($field eq "type") {
                if ($#token >= 3) {
                    push(@msgType, $token[3]);
                    $msgTypeList{$name} = $token[3];
                } else {
                    print STDERR "$msgName[$#msgName] does not include type number!\n";
                    exit;
                }
            }

            if ($type =~ /^struct/) {
                my @vars = @{$struct{$token[1]}};
                for (my $j = 0; $j <= $#vars; $j += 2) {
                    $type = $vars[$j];
                    $field = $vars[$j+1];
                    $fieldToType{$field} = $type;
                    $fieldNum += 1;
                    push(@{$fieldsPerMsg{$#msgName}}, $type);
                    if ($fieldNum > $maxField) {
                        $maxField = $fieldNum;
                    }

                    my $tempIndex = 1;
                    my $baseField;
                    if ($field =~ /\[/) {
                        $baseField = substr($field, 0, index($field, "["));
                        my $length = index($field, "]") - index($field, "[") - 1;
                        $tempIndex = substr($field, index($field, "[")+1, $length);
                    }

                    if ($type eq "char") {
                    } else {
                        for (my $k = 0; $k < $tempIndex; $k++) {
                            if ($tempIndex > 1) {
                                $field = "$baseField". "[$k]";
                            }
                        }
                    }

                }
                $fieldOffset .= "+sizeof(struct $token[1])";
                if ($index ne "1") {
                    $fieldOffset .= "*$index";
                }

            } else {

                $fieldToType{$field} = $type;
                $fieldNum += 1;
                push(@{$fieldsPerMsg{$#msgName}}, $type);
                if ($fieldNum > $maxField) {
                    $maxField = $fieldNum;
                }

                $fieldOffset .= "+sizeof($type)";
                if ($index ne "1") {
                    $fieldOffset .= "*$index";
                }
            }
        }

    }

    }


    close(FILE);



    my $typenamestr = "int8_t";

    push(@{$coarseStrategyList{$typenamestr}}, "r");
    # Zero
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    # Extreme
    push(@{$coarseStrategyList{$typenamestr}}, "=-128");
    push(@{$coarseStrategyList{$typenamestr}}, "=128");
    # fine
    #push(@{$fineStrategyList{$typenamestr}}, "=-32");
    #push(@{$fineStrategyList{$typenamestr}}, "=32");
    push(@{$fineStrategyList{$typenamestr}}, "=16");
    push(@{$fineStrategyList{$typenamestr}}, "=-16");
    push(@{$fineStrategyList{$typenamestr}}, "+1");
    push(@{$fineStrategyList{$typenamestr}}, "+10");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr = "uint8_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=257");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "+1");
    #push(@{$fineStrategyList{$typenamestr}}, "=128");
    push(@{$fineStrategyList{$typenamestr}}, "=64");
    #push(@{$fineStrategyList{$typenamestr}}, "=32");
    push(@{$fineStrategyList{$typenamestr}}, "=16");
    push(@{$fineStrategyList{$typenamestr}}, "=8");
    push(@{$fineStrategyList{$typenamestr}}, "+10");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "int16_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-128");
    push(@{$coarseStrategyList{$typenamestr}}, "=128");
    #fine
    push(@{$fineStrategyList{$typenamestr}}, "=-32767");
    push(@{$fineStrategyList{$typenamestr}}, "=32767");
    push(@{$fineStrategyList{$typenamestr}}, "=-1024");
    push(@{$fineStrategyList{$typenamestr}}, "=1024");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};

    $typenamestr= "short";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-32767");
    push(@{$coarseStrategyList{$typenamestr}}, "=32767");
    #fine
    push(@{$fineStrategyList{$typenamestr}}, "=-1024");
    push(@{$fineStrategyList{$typenamestr}}, "=1024");
    push(@{$fineStrategyList{$typenamestr}}, "=-128");
    push(@{$fineStrategyList{$typenamestr}}, "=128");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "uint16_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=65535");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=16384");
    push(@{$fineStrategyList{$typenamestr}}, "=4096");
    push(@{$fineStrategyList{$typenamestr}}, "+1");
    push(@{$fineStrategyList{$typenamestr}}, "-1");
    push(@{$fineStrategyList{$typenamestr}}, "=1024");
    push(@{$fineStrategyList{$typenamestr}}, "=256");
    push(@{$fineStrategyList{$typenamestr}}, "=64");
    push(@{$fineStrategyList{$typenamestr}}, "+1000");
    push(@{$fineStrategyList{$typenamestr}}, "+10");
    push(@{$fineStrategyList{$typenamestr}}, "-1000");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "int32_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-2147483647");
    push(@{$coarseStrategyList{$typenamestr}}, "=2147483647");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=-131072");
    push(@{$fineStrategyList{$typenamestr}}, "=131072");
    push(@{$fineStrategyList{$typenamestr}}, "=-4096");
    push(@{$fineStrategyList{$typenamestr}}, "=4096");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};

    $typenamestr= "int";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-2147483647");
    push(@{$coarseStrategyList{$typenamestr}}, "=2147483647");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=-131072");
    push(@{$fineStrategyList{$typenamestr}}, "=131072");
    push(@{$fineStrategyList{$typenamestr}}, "=-4096");
    push(@{$fineStrategyList{$typenamestr}}, "=4096");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};

    $typenamestr= "uint32_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=4294967295");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=134217728");
    push(@{$fineStrategyList{$typenamestr}}, "=4194304");
    push(@{$fineStrategyList{$typenamestr}}, "=131072");
    push(@{$fineStrategyList{$typenamestr}}, "=4096");
    push(@{$fineStrategyList{$typenamestr}}, "=128");
    push(@{$fineStrategyList{$typenamestr}}, "+1");
    push(@{$fineStrategyList{$typenamestr}}, "+10");
    push(@{$fineStrategyList{$typenamestr}}, "+1000");
    push(@{$fineStrategyList{$typenamestr}}, "+10000");
    push(@{$fineStrategyList{$typenamestr}}, "-1");
    push(@{$fineStrategyList{$typenamestr}}, "-1000");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "unsigned";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=128");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=4294967295");
    push(@{$fineStrategyList{$typenamestr}}, "=134217728");
    push(@{$fineStrategyList{$typenamestr}}, "=4194304");
    push(@{$fineStrategyList{$typenamestr}}, "=131072");
    push(@{$fineStrategyList{$typenamestr}}, "=4096");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "unsigned int";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=4294967295");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=134217728");
    push(@{$fineStrategyList{$typenamestr}}, "=4194304");
    push(@{$fineStrategyList{$typenamestr}}, "=131072");
    push(@{$fineStrategyList{$typenamestr}}, "=4096");
    push(@{$fineStrategyList{$typenamestr}}, "=128");
    @test = $coarseScoarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "int64_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-16777216");
    push(@{$coarseStrategyList{$typenamestr}}, "=16777216");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=-9223372036854775807LL");
    push(@{$fineStrategyList{$typenamestr}}, "=9223372036854775807LL");
    push(@{$fineStrategyList{$typenamestr}}, "=-17179869184");
    push(@{$fineStrategyList{$typenamestr}}, "=17179869184");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "uint64_t";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=16777216");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=4294967295");
    push(@{$fineStrategyList{$typenamestr}}, "=134217728");
    push(@{$fineStrategyList{$typenamestr}}, "=4194304");
    push(@{$fineStrategyList{$typenamestr}}, "=9223372036854775807LL");
    push(@{$fineStrategyList{$typenamestr}}, "=17179869184");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "double";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-1E+37");
    push(@{$coarseStrategyList{$typenamestr}}, "=1E+37");
    # fine 
    push(@{$fineStrategyList{$typenamestr}}, "=1000000000");
    push(@{$fineStrategyList{$typenamestr}}, "=-1000000000");
    push(@{$fineStrategyList{$typenamestr}}, "=1000000000000000000");
    push(@{$fineStrategyList{$typenamestr}}, "=-1000000000000000000");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "float";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=-1E+37");
    push(@{$coarseStrategyList{$typenamestr}}, "=1E+37");
    # fine
    push(@{$fineStrategyList{$typenamestr}}, "=1000000000");
    push(@{$fineStrategyList{$typenamestr}}, "=-1000000000");
    push(@{$fineStrategyList{$typenamestr}}, "=1000000000000000000");
    push(@{$fineStrategyList{$typenamestr}}, "=-1000000000000000000");
    @test = $fineSoarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    @test = $fineStrategyList{$typenamestr};
    $fineStrategyCount{$typenamestr} = $#{$test[0]};


    $typenamestr= "bool";
    push(@{$coarseStrategyList{$typenamestr}}, "r");
    push(@{$coarseStrategyList{$typenamestr}}, "=0");
    push(@{$coarseStrategyList{$typenamestr}}, "=1");
    @test = $coarseStrategyList{$typenamestr};
    $coarseStrategyCount{$typenamestr} = $#{$test[0]};
    

    # CHAR TYPE: we do not lie about

    $coarseStrategyCount{"char"} = 0;
    $fineStrategyCount{"char"} = 0;

    # BIT FIELDS

    $typenamestr="1";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=1");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};


    $typenamestr="2";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=1");
    push(@{$FlenList{$typenamestr}}, "=3");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};


    $typenamestr="3";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=2");
    push(@{$FlenList{$typenamestr}}, "=3");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};
    push(@{$FlenList{$typenamestr}}, "=7");


    $typenamestr="4";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=2");
    push(@{$FlenList{$typenamestr}}, "=4");
    push(@{$FlenList{$typenamestr}}, "=8");
    push(@{$FlenList{$typenamestr}}, "=15");
    push(@{$FlenList{$typenamestr}}, "+1");
    push(@{$FlenList{$typenamestr}}, "-1");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};


    $typenamestr="5";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=2");
    push(@{$FlenList{$typenamestr}}, "=4");
    push(@{$FlenList{$typenamestr}}, "=8");
    push(@{$FlenList{$typenamestr}}, "=16");
    push(@{$FlenList{$typenamestr}}, "=9");
    push(@{$FlenList{$typenamestr}}, "=31");
    push(@{$FlenList{$typenamestr}}, "+1");
    push(@{$FlenList{$typenamestr}}, "-1");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};


    $typenamestr="6";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=2");
    push(@{$FlenList{$typenamestr}}, "=4");
    push(@{$FlenList{$typenamestr}}, "=16");
    push(@{$FlenList{$typenamestr}}, "=63");
    push(@{$FlenList{$typenamestr}}, "-1");
    push(@{$FlenList{$typenamestr}}, "+1");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};


    $typenamestr="7";
    push(@{$FlenList{$typenamestr}}, "=0");
    push(@{$FlenList{$typenamestr}}, "=3");
    push(@{$FlenList{$typenamestr}}, "=4");
    push(@{$FlenList{$typenamestr}}, "=8");
    push(@{$FlenList{$typenamestr}}, "=19");
    push(@{$FlenList{$typenamestr}}, "=64");
    push(@{$FlenList{$typenamestr}}, "=127");
    push(@{$FlenList{$typenamestr}}, "-1");
    push(@{$FlenList{$typenamestr}}, "+1");
    @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};

    return \%fieldsPerMsg;
}

sub getStrategyCount{
	return \%strategyCount;
}

sub getTypeStrategyList{
	return \%strategyList;
}

sub getCoarseStrategyList {
    return \%coarseStrategyList;
}

sub getFineStrategyList {
    return \%fineStrategyList;
}

sub getMsgNameRes{
  return \@msgName;
}

sub getMsgNameClount{
  return $#msgName;
}

sub getMsgFlenList{
	return \%msgFlenList;
}

sub getFlenList{
	return \%FlenList;
}

sub getFlenNumList{
	return \%FlenNumList;
}

sub getMsgTypeRef{
	return \%msgTypeList;
}
1;
