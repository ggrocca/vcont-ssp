#!/bin/bash

source mini-conf.sh

echo ""
echo "Checking tiff files"
echo ""

for i in $DATASETS ; do

    if [ -f $NEWRES/"$i"n-base.tif ] ; then
	echo "difftiff: " $NEWRES/"$i"n-base.tif $OLDRES/"$i"n-base.tif
	$DIFFTIFF $NEWRES/"$i"n-base.tif $OLDRES/"$i"n-base.tif
    else
	echo "!! No file " $NEWRES/"$i"n-base.tif $OLDRES/"$i"n-base.tif
    fi

    for (( j=1; j<$NUM_LEVELS; j++ )) ; do
	if [ -f $NEWRES/"$i"n"$j".tif ] ; then
	    echo "difftiff: " $NEWRES/"$i"n"$j".tif $OLDRES/"$i"n"$j".tif
	    $DIFFTIFF $NEWRES/"$i"n"$j".tif $OLDRES/"$i"n"$j".tif
	else
	    echo "!! No file " $NEWRES/"$i"n"$j".tif $OLDRES/"$i"n"$j".tif
	fi
    done

    for (( j=0; j<$NUM_LEVELS; j++ )) ; do
	if [ -f $NEWRES/"$i"n"$j".tif ] ; then
	    echo "difftiff: " $NEWRES/"$i"n-MARKED-"$j".tif $OLDRES/"$i"n-MARKED-"$j".tif
	    $DIFFTIFF $NEWRES/"$i"n-MARKED-"$j".tif $OLDRES/"$i"n-MARKED-"$j".tif
	else
	    echo "!! No file " $NEWRES/"$i"n-MARKED-"$j".tif $OLDRES/"$i"n-MARKED-"$j".tif
	fi
    done

done


echo ""
echo "Checking text files"
echo ""

for i in $DATASETS ; do
    for j in $FILETYPES ; do
	echo $DIFF $NEWRES/$i.$j $OLDRES/$i.$j
	if [ -f $NEWRES/$i.$j ] ; then
	    $DIFF $NEWRES/$i.$j $OLDRES/$i.$j
	else
	    echo "!! No .$j file $NEWRES/$i.$j"
	fi
    done
done

echo ""
echo "Checking track files"
echo ""

for i in $DATASETS ; do
    echo $DIFFTRAC $NEWRES/$i.trk $OLDRES/$i.trk
    if [ -f $NEWRES/$i.trk ] ; then
	$DIFFTRAC $NEWRES/$i.trk $OLDRES/$i.trk
    else
	echo "!! No .trk file $NEWRES/$i.trk"
    fi
done
