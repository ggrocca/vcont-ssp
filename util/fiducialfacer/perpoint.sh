#!/bin/bash

tab="distancetable2.dat"

fiducials=`head $tab -n 13 | cut -f 3`
meshnum=`tail -n 1 $tab | cut -f 2`
#meshnum=$((meshnum+1))
# 100 : mn = x : ln   x=(100/mn)ln    
echo $meshnum
#meshnum=108

echo "check that all meshes have all fiducial points..."
for (( m=1; m<=$meshnum; m++ )); do
    lzm="$(printf "%03d" $m)"
    #echo $lzm
    fidnum=`grep Mesh_$lzm $tab | wc -l`
    if [ $fidnum -ne "13" ]; then
	echo "$lzm has $fidnum fiducial instead of 13"
    fi
done
echo "done."
echo ""
echo "generate temporary tables and plots..."
for fp in $fiducials; do

    echo $fp
#    set -x
    {
	#echo ""
	grep $fp $tab | \
	    cut -f "2,1,5" | \
	    sort -g -k "3,3" | \
	    awk -v "m=$meshnum" 'BEGIN {printf("histvalue percent name number distance\n")}  {printf("%d %lf %s\n", NR, (100.0/m)*NR, $0)}'
    } > $fp.tmp.dat
#    set +x
    
    gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm linewidth 4 rounded 
set output 'var-$fp.pdf'
set title '$fp'
#set xrange [0:40]
#set yrange [0:120]
plot '$fp.tmp.dat' \
     using "distance":"percent" \
     w lp
_EOF_

        gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm linewidth 4 rounded 
set output 'fix-$fp.pdf'
set title '$fp'
set xrange [0:40]
#set yrange [0:120]
plot '$fp.tmp.dat' \
     using "distance":"percent" \
     w lp
_EOF_

done
echo "done."
echo ""
