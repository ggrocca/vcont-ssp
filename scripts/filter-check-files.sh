#!/bin/bash

source filter-conf.sh

for (( k=0; k<2; k++ )) ; do

    if [ $k -eq 0 ]; then
	echo ""; echo ">>>> CHECK NORM <<<<"; echo ""
	FILTER_CHECK=$FILTER_NORM
    fi

    if [ $k -eq 1 ]; then
	if [ $DO_CHECK_FAST = "no" ]; then
	    exit 0
	fi

	echo ""; echo ">>>> CHECK FAST <<<<"; echo ""
	FILTER_CHECK=$FILTER_FAST
    fi


    echo ""
    echo "Checking tiff files"
    echo ""

    for i in $DATASETS ; do

	for (( j=0; j<$NUM_LEVELS; j++ )) ; do
	    ( set -x; $DIFFTIFF $RES/"$i"_"$FILTER_SLOW"-"$j".tif $RES/"$i"_"$FILTER_CHECK"-"$j".tif )
	done

	for (( j=0; j<$NUM_LEVELS; j++ )) ; do
	    ( set -x; $DIFFTIFF $RES/"$i"_"$FILTER_SLOW"-MARKED-"$j".tif $RES/"$i"_"$FILTER_CHECK"-MARKED-"$j".tif )
	done

    done


    echo ""
    echo "Checking text files"
    echo ""

    for i in $DATASETS ; do
	for j in $FILETYPES ; do
	    ( set -x; $DIFF $RES/"$i"_"$FILTER_SLOW"."$j" $RES/"$i"_"$FILTER_CHECK"."$j" )
	done
    done

    echo ""
    echo "Checking track files"
    echo ""

    for i in $DATASETS ; do
	( set -x; $DIFFTRAC $RES/"$i"_"$FILTER_SLOW".trk $RES/"$i"_"$FILTER_CHECK".trk )
    done

    echo ""

done
