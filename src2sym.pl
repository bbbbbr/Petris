#!/usr/bin/perl -w

use strict;
use warnings;
use File::Basename;

sub FindSourceLine;

my $MainSymbolFileName = "";
if ($ARGV[0]) {
    $MainSymbolFileName = $ARGV[0];
    open(MainSymbolFile, $MainSymbolFileName) or die("File $MainSymbolFileName not found");

    my ($SymbolFileName, $SymbolPath, $SymbolPathSuffix) = fileparse($MainSymbolFileName);

    while (my $FileLine = <MainSymbolFile>) {

        # First convert C source references to labels that include the source
        if ($FileLine =~ /(\d\d\:\w\w\w\w )(C\$)(\w*)\.c\$(\d*).*/) {
            # $1 : Symbol Address info
            # $2 : Label start ("C$")
            # $3 : C source file name *without* file extension
            # $4 : C source line number
            my $CLabelFileNameNoExt = $3;
            my $CLabelSourceLineNum = $4;

            my $CSourceLine = FindSourceLine($SymbolPath, $CLabelFileNameNoExt, $CLabelSourceLineNum);
            if ($CSourceLine) {
                # Output Reformatted address and truncated label
                print $1.$2.$3."\@".$CLabelSourceLineNum.":";
                # Append C source code line to output
                print $CSourceLine."\n";
            }

        } elsif ($FileLine =~ /(\d\d\:\w\w\w\w )A\$.*\n/) {
            # strip out ASM lines
            # print $FileLine." -> (ASM, REMOVE)\n";
        } else {
            # Pass the remainder on through
            print $FileLine;
        }

    }
    close(MainSymbolFile);
} else {
    print "Please specify main symbol file (located in same dir as individual .asm files)\n";
}


sub FindSourceLine {
    my ($Directory, $FileName, $CLineNum) = @_;
    # print "Checking: $Directory, $FileName, $CLineNum\n";

    if ($Directory and $FileName and $CLineNum) {
        my $SourceFileFullPath = $Directory.$FileName."\.asm";

        if (open(SourceFile, $SourceFileFullPath)) {

            while (my $FileLine = <SourceFile>) {

                # First convert C source references to labels that include the source
                if($FileLine =~ /\;src\/$FileName\.c\:$CLineNum\: (.*)/) {
                    # $1 : C source line

                    my $CSourceLine = $1;
                    $CSourceLine =~ s/\n//g; # remove newlines
                    # $CSourceLine =~ s/ /_/g; # convert spaces to underscores
                    $CSourceLine =~ s/ //g; # strip spaces
                    $CSourceLine =~ s/\;/\:/g; # convert semicolons to colons
                    # print "FOUND: $FileLine ---> $CSourceLine \n";
                    return $CSourceLine;
                }

            }
            close(SourceFile);
        } else {
            # print "file not found: $SourceFileFullPath\n";
        }

    } else {
        # print "Please specify main symbol file (located in same dir as individual .asm files)\n";
    }
}


