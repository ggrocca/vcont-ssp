#!/bin/bash

if [ $# -ne 4 ]; then
    echo "usage: ./jaara2csv dataset.asc outname SquareSizeStart SquareSizeEnd"
    exit 0
fi

dataset=$1
outname=$2
ss_start=$3
ss_end=$4

# jaara output for each squaresize
for (( i=$ss_end; i >= $ss_start; i-- )) do
    ./jaara.sh $dataset $i > jaara.out.$i.tmp
    cp jaara.out.$i.tmp jaara.clean.$i.tmp
done


# for each jaara output file, clean step by step
for (( i=$ss_end; i >= $ss_start; i-- )) do
    # for every line in each file
    while read line; do
	# check every other file next in queue
	for (( j=$(($i-1)); j >= $ss_start; j-- )) do
	    grep -v "$line" jaara.clean.$j.tmp > jaara.clean.$j.tmp.aux 
	    cp jaara.clean.$j.tmp.aux jaara.clean.$j.tmp
	    rm jaara.clean.$j.tmp.aux
	done
    done < jaara.clean.$i.tmp
done

# make csv header
echo "X,Y,Z,IMPORTANCE,CLASSIFICATION" > "$outname.csv"
	    
for (( i=$ss_end; i >= $ss_start; i-- )) do
    while read line; do
	echo `echo $line | cut -d " " -s -f 1`,`echo $line | cut -d " " -s -f 2`,`echo $line | cut -d " " -s -f 3`,$i,PEAK  >> "$outname.csv"
    done < jaara.clean.$i.tmp
done

rm jaara.out.*.tmp
rm jaara.clean.*.tmp

exit 0
