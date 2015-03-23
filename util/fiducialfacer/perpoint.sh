#!/bin/bash

tab="distancetable2.dat"

fiducials=`head $tab -n 13 | cut -f 3`
meshnum=`tail -n 1 $tab | cut -f 2`
#meshnum=$((meshnum+1))
# 100 : mn = x : ln   x=(100/mn)ln    
echo "number of meshe: $meshnum"
echo ""
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



echo "generate grouped plots..."

my_fontscale=1.7
my_dashlength=1.5

function plot_group {
    {
	name=$1
	shift
	comma=","
	plotline="plot "
	for var in "$@"; do
	    plotline="$plotline '$var.tmp.dat' using 'distance':'percent' w lp"
	    if [ $var != "${@: -1}" ]; then
		plotline=$plotline$comma
	    fi
	done
	echo "set terminal pdfcairo dl $my_dashlength size 20cm,20cm linewidth 4 rounded fontscale $my_fontscale"
	echo "set output '$name.pdf'"
#	echo "load 'settings.gp'"
	#echo "set title '$fp'"
	echo "set xrange [0:20]"
	echo $plotline
    } | gnuplot
}

plot_group group1_nose  fiducial_prn   fiducial_al_dx fiducial_al_sx fiducial_m      fiducial_sn
plot_group group2_eyes  fiducial_ex_dx fiducial_ex_sx fiducial_en_dx fiducial_en_sx
plot_group group3_mouth fiducial_sto1  fiducial_sto2  fiducial_ch_dx fiducial_ch_sx

echo "done."
echo ""
