#!/bin/bash

source libplot.sh

dir=$1
set=$2
# max=$2
max=`grep generic_life_max "$dir"/"stats.txt" | cut -d ' '  -f 2`
echo "max =" $max
realmax=$max

plot_scattered $dir stats-peaks-performance.dat \
	       terrain-peaks-PR.pdf \
	       tpr ppv $set

plot_scattered_color $dir stats-peaks-performance.dat \
		     terrain-peaks-PR-life.pdf \
		     tpr ppv life 3 PR-peak $max

zoom=$(awk -v m=$max 'BEGIN { print (m / 4) }')
plot_scattered_color $dir stats-peaks-performance.dat \
		     terrain-peaks-PR-life-zoom.pdf \
		     tpr ppv life 3 PR-peak $zoom
zoommore=$(awk -v m=$max 'BEGIN { print (m / 16) }')
plot_scattered_color $dir stats-peaks-performance.dat \
		     terrain-peaks-PR-life-zoommore.pdf \
		     tpr ppv life 3 PR-peak $zoommore

for column in markedness informedness mcc ; do
    simple_plot $dir stats-peaks-performance.dat terrain-peaks-$column.pdf \
		life $column $column 0 $realmax -1 1
    #life $column $column \* \* -1 1
done

for column in duplicates maxcandidates ; do
    simple_plot $dir stats-peaks-performance.dat terrain-peaks-$column.pdf \
		life $column $column 0 $realmax \* \*
    #life $column $column \* \* \* \*
done

for column in f1 f2 f05 ppv tpr fpr fnr tnr ; do
    simple_plot $dir stats-peaks-performance.dat terrain-peaks-$column.pdf \
		life $column $set 0 $realmax 0 1
    # life $column $column \* \* 0 1
done
