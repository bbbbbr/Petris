#!/bin/bash

# find all the data allocation rows in the linker symbol output
# dump them into a csv
# load the csv in libre office calc

rm linker_summary.csv
#grep -R "flags" Release/* | grep ".sym" | perl -pe 's/(.*)\.sym\:\s*\d+\s_([0-9A-Z_]*)\s*size\s*([0-9A-F]*)\s*flags.*/$1,$2,=HEX2DEC("$3")/' >> linker_summary.csv
echo "File,Seg,Size" > linker_summary.csv;
grep -R "flags" ReleaseColor/* | grep ".sym" | perl -pe 's/(.*)\.sym\:\s*\d+\s_([0-9A-Z_]*)\s*size\s*([0-9A-F]*)\s*flags.*/ /; print $1 . "," . $2 . "," . hex($3) . ","' >> linker_summary.csv
datamash --headers --field-separator=, --no-strict --ignore-case --narm  --sort --group 2 sum 3 < linker_summary.csv
#loffice -calc linker_summary.csv
