#!/usr/bin/perl -w

# == Source-ish debugging in BGB ==
#
# Merges C Source into the symbol labels.
# This isn't the "Real Thing" and looks messy in BGB,
# but does work and can give a little more clarity when
# stepping in the debugger (or figuring out where to set
# your breakpoint without having to consult the .sym + .asm output).

# The labels get truncated if they're too long.
# Using the Tab key you can easily turn the symbol view on/off.
#
# These are the flags added :
# SDCC: --debug
# LCC: -Wl-j
#
# Then the main symbol file has the "A$..." lines stripped,
# and the "C$..." (C source) lines are used to pull in the
# source from the .asm files generated during compile time.
#
# Example use in a makefile:
# ifdef CDEBUG
#     @echo Reprocessing Symbols to include C Source
#     mv $(OBJDIR)/$(PROJECT_NAME).sym $(OBJDIR)/$(PROJECT_NAME)_raw.sym
#     $(SRC2SYM) $(OBJDIR)/$(PROJECT_NAME)_raw.sym > $(OBJDIR)/$(PROJECT_NAME).sym
# endif



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
            # print "Source file scan: File not found: $SourceFileFullPath\n";
        }

    } else {
        # print "Source file scan: Missing one of the following: Directory, Filename or C Line Number\n";
    }
}


