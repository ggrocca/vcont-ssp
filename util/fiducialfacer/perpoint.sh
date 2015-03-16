#!/bin/bash

tab="distancetable.dat"

fiducials=`head $tab -n 13 | cut -f 3`

for fp in $fiducials; do

    echo $fp
#    set -x
    {
	#echo ""
	grep $fp $tab | \
	    cut -f "2,1,5" | \
	    sort -g -k "3,3" | \
	    awk 'BEGIN {printf("histvalue name number distance\n")}  {printf("%d %s\n", NR, $0)}'
    } > $fp.tmp.dat
#    set +x
    
    gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm linewidth 4 rounded 
set output 'var-$fp.pdf'
set title '$fp'
#set xrange [0:40]
#set yrange [0:120]
plot '$fp.tmp.dat' \
     using "distance":"histvalue" \
     w lp
_EOF_

        gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm linewidth 4 rounded 
set output 'fix-$fp.pdf'
set title '$fp'
set xrange [0:40]
set yrange [0:120]
plot '$fp.tmp.dat' \
     using "distance":"histvalue" \
     w lp
_EOF_

    
done
