#!/bin/bash

source mini-conf.sh

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
