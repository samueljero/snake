#!/usr/bin/perl

package MsgParse;

my %strategyCount;
my %strategyList;
my %msgTypeList;
my %FlenList;
my %FlenNumList;
my %fieldsPerMsg;
#my @msgName;
#my @msgType;
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
  open FILE, $GatlingConfig::formatFile or die "Can't open format file $GatlingConfig::formatFile\n$!";

  while(<FILE>) {
    s/;//;
    my @token = split();
#skip empty or commented out lines
    if ($#token < 0 or $token[0] =~ /^\/\//) {
      next;
  }
#make sure first message is base message type
  if ($parsing == 0 and $#msgName == -1 and $token[0] ne "BaseMessage") {
    print STDERR "First message type is not BaseMessage!\n";
    exit;
  } 
  $field = "";
  $type = "";
  $index = "1";
  $indexField = "";
  if ($parsing == 0) {
    if ($token[0] eq "struct") {
      $parsing = 1;
      $structName = $token[1];

    } else {
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
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=-128");
  push(@{$strategyList{$typenamestr}}, "=128");
  #push(@{$strategyList{$typenamestr}}, "=-32");
  #push(@{$strategyList{$typenamestr}}, "=32");
  push(@{$strategyList{$typenamestr}}, "=16");
  push(@{$strategyList{$typenamestr}}, "=-16");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr = "uint8_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=257");
  #push(@{$strategyList{$typenamestr}}, "=128");
  push(@{$strategyList{$typenamestr}}, "=64");
  #push(@{$strategyList{$typenamestr}}, "=32");
  push(@{$strategyList{$typenamestr}}, "=16");
  push(@{$strategyList{$typenamestr}}, "=8");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "int16_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
#push(@{$strategyList{$typenamestr}}, "=-32767");
#push(@{$strategyList{$typenamestr}}, "=32767");
#push(@{$strategyList{$typenamestr}}, "=-1024");
#push(@{$strategyList{$typenamestr}}, "=1024");
  push(@{$strategyList{$typenamestr}}, "=-128");
  push(@{$strategyList{$typenamestr}}, "=128");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "short";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=-32767");
  push(@{$strategyList{$typenamestr}}, "=32767");
  #push(@{$strategyList{$typenamestr}}, "=-1024");
  #push(@{$strategyList{$typenamestr}}, "=1024");
  push(@{$strategyList{$typenamestr}}, "=-128");
  push(@{$strategyList{$typenamestr}}, "=128");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "uint16_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=65535");
  #push(@{$strategyList{$typenamestr}}, "=16384");
  push(@{$strategyList{$typenamestr}}, "=4096");
  #push(@{$strategyList{$typenamestr}}, "=1024");
  push(@{$strategyList{$typenamestr}}, "=256");
  push(@{$strategyList{$typenamestr}}, "=64");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "int32_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=-2147483647");
  push(@{$strategyList{$typenamestr}}, "=2147483647");
  #push(@{$strategyList{$typenamestr}}, "=-131072");
  #push(@{$strategyList{$typenamestr}}, "=131072");
  push(@{$strategyList{$typenamestr}}, "=-4096");
  push(@{$strategyList{$typenamestr}}, "=4096");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "int";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=-2147483647");
  push(@{$strategyList{$typenamestr}}, "=2147483647");
  #push(@{$strategyList{$typenamestr}}, "=-131072");
  #push(@{$strategyList{$typenamestr}}, "=131072");
  push(@{$strategyList{$typenamestr}}, "=-4096");
  push(@{$strategyList{$typenamestr}}, "=4096");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "uint32_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=4294967295");
#push(@{$strategyList{$typenamestr}}, "=134217728");
#push(@{$strategyList{$typenamestr}}, "=4194304");
#push(@{$strategyList{$typenamestr}}, "=131072");
  push(@{$strategyList{$typenamestr}}, "=4096");
  push(@{$strategyList{$typenamestr}}, "=128");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "unsigned";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
#push(@{$strategyList{$typenamestr}}, "=4294967295");
#push(@{$strategyList{$typenamestr}}, "=134217728");
#push(@{$strategyList{$typenamestr}}, "=4194304");
#push(@{$strategyList{$typenamestr}}, "=131072");
#push(@{$strategyList{$typenamestr}}, "=4096");
  push(@{$strategyList{$typenamestr}}, "=128");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "unsigned int";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
#push(@{$strategyList{$typenamestr}}, "=4294967295");
#push(@{$strategyList{$typenamestr}}, "=134217728");
#push(@{$strategyList{$typenamestr}}, "=4194304");
#push(@{$strategyList{$typenamestr}}, "=131072");
#push(@{$strategyList{$typenamestr}}, "=4096");
  push(@{$strategyList{$typenamestr}}, "=128");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "int64_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
#push(@{$strategyList{$typenamestr}}, "=-9223372036854775807LL");
#push(@{$strategyList{$typenamestr}}, "=9223372036854775807LL");
#push(@{$strategyList{$typenamestr}}, "=-17179869184");
#push(@{$strategyList{$typenamestr}}, "=17179869184");
  push(@{$strategyList{$typenamestr}}, "=-16777216");
  push(@{$strategyList{$typenamestr}}, "=16777216");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "uint64_t";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
#push(@{$strategyList{$typenamestr}}, "=4294967295");
#push(@{$strategyList{$typenamestr}}, "=134217728");
#push(@{$strategyList{$typenamestr}}, "=4194304");
#push(@{$strategyList{$typenamestr}}, "=9223372036854775807LL");
#push(@{$strategyList{$typenamestr}}, "=17179869184");
  push(@{$strategyList{$typenamestr}}, "=16777216");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "double";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=-1E+37");
#push(@{$strategyList{$typenamestr}}, "=1E+37");
#push(@{$strategyList{$typenamestr}}, "=1000000000");
#push(@{$strategyList{$typenamestr}}, "=-1000000000");
#push(@{$strategyList{$typenamestr}}, "=1000000000000000000");
  push(@{$strategyList{$typenamestr}}, "=-1000000000000000000");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "float";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=-1E+37");
  push(@{$strategyList{$typenamestr}}, "=1E+37");
  push(@{$strategyList{$typenamestr}}, "=1000000000");
  push(@{$strategyList{$typenamestr}}, "=-1000000000");
# push(@{$strategyList{$typenamestr}}, "=1000000000000000000");
  #push(@{$strategyList{$typenamestr}}, "=-1000000000000000000");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $typenamestr= "bool";
  push(@{$strategyList{$typenamestr}}, "r");
  push(@{$strategyList{$typenamestr}}, "=0");
  push(@{$strategyList{$typenamestr}}, "=1");
  @test = $strategyList{$typenamestr};
  $strategyCount{$typenamestr} = $#{$test[0]};
  $strategyCount{"char"} = 0; # we won't lie about char
  
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
  @test = $FlenList{$typenamestr};
    $FlenNumList{$typenamestr}=$#{$test[0]};
  $typenamestr="6";
  push(@{$FlenList{$typenamestr}}, "=0");
  push(@{$FlenList{$typenamestr}}, "=2");
  push(@{$FlenList{$typenamestr}}, "=4");
  push(@{$FlenList{$typenamestr}}, "=16");
  push(@{$FlenList{$typenamestr}}, "=63");
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

1;
