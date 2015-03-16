#!/bin/bash

if [ $# -ne 2 ]; then
    echo "usage: ./drop2csv dataset.asc outname"
    exit 0
fi

dataset=$1
outname=$2

./drop.sh $dataset 0 > drop.1.tmp

# make csv header
echo "X,Y,Z,IMPORTANCE,CLASSIFICATION" > "$outname.csv"

# do not consider first line in file
grep -v "$(head -n 1 drop.1.tmp)" drop.1.tmp > drop.2.tmp

# for every other line in file
while read line; do
    echo `echo $line | cut -d " " -s -f 2`,`echo $line | cut -d " " -s -f 3`,0,`echo $line | cut -d " " -s -f 4`,PEAK >> "$outname.csv"
done < drop.2.tmp

rm drop.1.tmp
rm drop.2.tmp

exit 0
