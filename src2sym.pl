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
#     @echo Reprocessing .NOI Debug data to include C Source
#     $(SRC2SYM) $(OBJDIR)/$(PROJECT_NAME).noi > $(OBJDIR)/$(PROJECT_NAME).sym
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

        # FROM: DEF C$player_hinting.c$119$1_0$194 0x5677
        #   TO: 01:5677 C$player_hinting@119:spr_x=(player_x*BRD_UNIT_SIZE)+BRD_PIECE_X_OFFSET:
        # First convert C source references to labels that include the source
        if ($FileLine =~ /DEF (C\$)(\w*)\.c\$(\d*)\$.*0x(\w*).*/) {
            # $1 : Label start ("C$")
            # $2 : C source file name *without* file extension
            # $3 : C source line number
            # $4 : Symbol Address info
            my $CLabelFileNameNoExt = $2;
            my $CLabelSourceLineNum = $3;

            my $CSourceLine = FindSourceLine($SymbolPath, $CLabelFileNameNoExt, $CLabelSourceLineNum);
            if ($CSourceLine) {
                # Output Reformatted address and truncated label
                print "00\:".$4." ".$1.$2."\@".$CLabelSourceLineNum.":";
                # Append C source code line to output
                print $CSourceLine."\n";
            }

        } elsif ($FileLine =~ /DEF (A\$.*)\n/) {
            #ex: DEF A$player_hinting$320 0x5677
            # strip out ASM lines
            # print $FileLine." -> (ASM, REMOVE)\n";
        } elsif ($FileLine =~ /DEF (.*) 0x(\w*).*/) {
            #ex: DEF _joypad 0x6E97
            # This was not used in old gbdk/sdcc version
            # $1 : symbol identifier
            # $2 : symbol address
            # reformat other symbol lines
            print "00\:".$2." ".$1."\n";
        } else {
            # strip out other unknown lines
            # print $FileLine." -> (OTHER, REMOVE)\n";
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


