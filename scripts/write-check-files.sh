#!/bin/bash

source write-conf.sh

echo ""
echo "Checking tiff files"
echo ""

for i in $DATASETS ; do

    for (( j=0; j<$NUM_LEVELS; j++ )) ; do
	( set -x; $DIFFTIFF $RES/"$i"_"$NORMAL"-"$j".tif $RES/"$i"_"$READ"-"$j".tif )
    done

    for (( j=0; j<$NUM_LEVELS; j++ )) ; do
	( set -x; $DIFFTIFF $RES/"$i"_"$NORMAL"-MARKED-"$j".tif $RES/"$i"_"$READ"-MARKED-"$j".tif )
    done

done


echo ""
echo "Checking text files"
echo ""

for i in $DATASETS ; do
    for j in $FILETYPES ; do
	( set -x; $DIFF $RES/"$i"_"$NORMAL"."$j" $RES/"$i"_"$READ"."$j" )
    done
done

echo ""
echo "Checking track files"
echo ""

for i in $DATASETS ; do
    ( set -x; $DIFFTRAC $RES/"$i"_"$NORMAL".trk $RES/"$i"_"$READ".trk )
done

echo ""
