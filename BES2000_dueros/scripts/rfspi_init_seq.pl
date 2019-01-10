#!/bin/perl

use strict;

my $prefix;
my $reg;
my $val;
my $postfix;

if (scalar(@ARGV) == 0) {
    print "Usage:\n\t$0 <rfspi_seq_file>\n";
    exit 0;
}

open FH, $ARGV[0] || die "Failed to open $ARGV[0]";

while (<FH>) {
    if (/rfspi_wvalue|rfspi_rvalue/) {
        if (/^(.*(?:rfspi_wvalue|rfspi_rvalue)) *\( *8'(\w+) *, *([^ ]+) *\) *;(.*)$/) {
            $prefix = $1;
            $reg = $2;
            $val = $3;
            $postfix =$4;

            $reg =~ s/^h|H/0x/;
            $reg =~ s/^b|B/0b/;
            $reg =~ s/^(?:d|D)0*//;
            $reg =~ s/^o|O//;
            $reg = oct($reg) if ($reg =~ /^0/);
            $reg = sprintf("0x%02x", $reg);

            if ($val =~ /^16'(.*)$/) {
                $val = $1;
                $val =~ s/_//g;
                $val =~ s/^h|H/0x/;
                $val =~ s/^b|B/0b/;
                $val =~ s/^(?:d|D)0*//;
                $val =~ s/^o|O//;
                $val = oct($val) if ($val =~ /^0/);
                $val = sprintf("0x%04x", $val);
            }

            print "$prefix($reg, $val);$postfix\n";
	    next;
        }
    }

    print $_;
}

close FH;

