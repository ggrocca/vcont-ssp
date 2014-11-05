#!/bin/bash

# enum ClassifiedType {OTHER=0, PEAK, PIT, SADDLE, HUMAN, ALL};
# enum SwisstopoType {
#     NAMED_ALPINE_SUMMIT_100, NAMED_MAIN_SUMMIT_200, NAMED_SUMMIT_300,
#     NAMED_MAIN_HILL_400, NAMED_HILL_500, NAMED_ROCK_HEAD_600,
#     NAMED_PASS_700, NAMED_ROAD_PASS_800,
#     HEIGHT_LFP1_100, HEIGHT_LV95_200, HEIGHT_AGNES_300,
#     HEIGHT_NATURAL_400, HEIGHT_HUMAN_500, HEIGHT_LAKE_600,
#     MORPH_DOLINE_100, MORPH_ERRATIC_200, MORPH_BLOCK_300, MORPH_SINK_400,
#     UNDEFINED
# };


if [ $# == 0 ]; then
    echo NAMED_ALPINE_SUMMIT_100
    echo NAMED_MAIN_SUMMIT_200
    echo NAMED_SUMMIT_300
    echo NAMED_MAIN_HILL_400
    echo NAMED_HILL_500
    echo NAMED_ROCK_HEAD_600
    echo NAMED_PASS_700
    echo NAMED_ROAD_PASS_800
    echo HEIGHT_LFP1_100
    echo HEIGHT_LV95_200
    echo HEIGHT_AGNES_300
    echo HEIGHT_NATURAL_400
    echo HEIGHT_HUMAN_500
    echo HEIGHT_LAKE_600
    echo MORPH_DOLINE_100
    echo MORPH_ERRATIC_200
    echo MORPH_BLOCK_300
    echo MORPH_SINK_400
    exit 0
fi

OUTDIR=../../../datasets/dhm25

cat $OUTDIR/morph_kleinform-classified.csv > $OUTDIR/temp.csv
cat $OUTDIR/name_pkt-classified.csv >> $OUTDIR/temp.csv
cat $OUTDIR/hoehenkote-classified.csv >> $OUTDIR/temp.csv

if [ $# -ge 2 ]; then

    echo output file $1
    
    if [ -f $OUTDIR/$1.csv ]; then
	rm $OUTDIR/$1.csv
    fi

    head -n 1 $OUTDIR/name_pkt-classified.csv > $OUTDIR/$1.csv

    for i in ${@:2} ; do
	echo mangling $i
	grep $i $OUTDIR/temp.csv >> $OUTDIR/$1.csv 
    done
fi

rm $OUTDIR/temp.csv
