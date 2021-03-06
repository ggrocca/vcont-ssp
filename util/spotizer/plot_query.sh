#!/bin/bash

source libplot.sh

dir=$1
peaks=$2
# maxlife=$2
# maxstrength=$3

# maxlife=`grep terrain_life_max $RESDIR/"stats.txt" | cut -d ' '  -f 2`
# echo "maxlife = " $maxlife
# maxstrength=`grep terrain_strength_max $RESDIR/"stats.txt" | cut -d ' '  -f 2`
# echo "maxstrength = " $maxstrength


#for category in peaks pits saddles all; do

if [[ "$PEAKS" == "-M" ]]; then
    categories="peaks all saddles pits"
else
    categories="peaks"
fi

for category in $categories ; do

    for graph in ROC PR ; do

	if [[ $graph == "PR" ]]; then
	    xx=tpr
	    yy=ppv
	fi
	if [[ $graph == "ROC" ]]; then
	    xx=fpr
	    yy=tpr
	fi
	
	plot_scattered $dir stats-$category-performance.dat \
		       terrain-$category-$graph.pdf \
    		       $xx $yy "Life+Drop"

	for zed in life strength ; do

	    if [[ $zed == "life" ]]; then
		colnum=3
		#max=$maxlife
	    fi
	    if [[ $zed == "strength" ]]; then
		colnum=4
		#max=$maxstrength
	    fi

	    max=`grep terrain_"$category"_"$zed"_max "$dir"/"stats.txt" | cut -d ' '  -f 2`
	    echo "max = "$max

	    plot_scattered_color $dir stats-$category-performance.dat \
				 terrain-$category-$graph-$zed.pdf \
    				 $xx $yy $zed $colnum $graph-$category $max

	    zoom=$(awk -v m=$max 'BEGIN { print (m / 4) }')
	    plot_scattered_color $dir stats-$category-performance.dat \
				 terrain-$category-$graph-$zed-zoom.pdf \
    				 $xx $yy $zed $colnum $graph-$category $zoom
	    zoommore=$(awk -v m=$max 'BEGIN { print (m / 16) }')
	    plot_scattered_color $dir stats-$category-performance.dat \
				 terrain-$category-$graph-$zed-zoommore.pdf \
    				 $xx $yy $zed $colnum $graph-$category $zoommore
	done
    done

    for column in markedness informedness mcc duplicates maxcandidates f1 f2 f05 ppv tpr fpr fnr tnr ; do
    heat_map $dir stats-$category-performance.dat terrain-$category-$column.pdf \
	     life strength $column
    done

    realmax=`grep terrain_peaks_life_max "$dir"/"stats.txt" | cut -d ' '  -f 2`
    simple_plot $dir stats-peaks-performance.dat terrain-peaks-f05-simple.pdf \
		life f05 "Life+Drop" 0 $realmax 0 1
#		life f05 f05 \* \* 0 1


    
done
