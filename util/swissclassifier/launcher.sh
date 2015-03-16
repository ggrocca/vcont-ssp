#!/bin/bash

ALL=false
if [ $# == "0" ]; then
    ALL=true
else
    DATASET=$1
fi

strength_threshold=1.0

if [ $ALL == "true" ]; then DATASET="dhm25"; fi
if [ $DATASET == "dhm25" ]; then
    OUTDIR=../../../datasets/dhm25.classification
    ./swissclassifier -i $OUTDIR/morph_kleinform.csv \
		      -o $OUTDIR/morph_kleinform-classified.csv \
		      -a $OUTDIR/aargau-lucerne.asc \
		      -t $OUTDIR/aargau-lucerne_terr.trk \
		      -S $strength_threshold -c -R \
                      > $OUTDIR/morph_kleinform-classified.csv.out.txt
    
    ./swissclassifier -i $OUTDIR/name_pkt.csv \
		      -o $OUTDIR/name_pkt-classified.csv \
		      -a $OUTDIR/aargau-lucerne.asc \
		      -t $OUTDIR/aargau-lucerne_terr.trk \
		      -S $strength_threshold -c -R \
                      > $OUTDIR/name_pkt-classified.csv.out.txt


    ./swissclassifier -i $OUTDIR/hoehenkote.csv \
		      -o $OUTDIR/hoehenkote-classified.csv \
		      -a $OUTDIR/aargau-lucerne.asc \
		      -t $OUTDIR/aargau-lucerne_terr.trk \
		      -S $strength_threshold -c -R \
                      > $OUTDIR/hoehenkote-classified.csv.out.txt


    ./swissmangler.sh natural \
		      NAMED_ALPINE_SUMMIT_100 NAMED_MAIN_SUMMIT_200 \
		      NAMED_SUMMIT_300 NAMED_PASS_700 \
		      MORPH_SINK_400 MORPH_DOLINE_100 \
		      NAMED_MAIN_HILL_400 NAMED_HILL_500 HEIGHT_NATURAL_400
    
    ./swissmangler.sh allpeaks \
		      NAMED_ALPINE_SUMMIT_100 NAMED_MAIN_SUMMIT_200 \
		      NAMED_SUMMIT_300 NAMED_PASS_700 MORPH_SINK_400 \
		      MORPH_DOLINE_100 NAMED_MAIN_HILL_400 NAMED_HILL_500

    ./swissmangler.sh mainpeaks \
		      NAMED_ALPINE_SUMMIT_100 NAMED_MAIN_SUMMIT_200 \
		      NAMED_SUMMIT_300 NAMED_PASS_700 \
		      MORPH_SINK_400 MORPH_DOLINE_100
fi


if [ $ALL == "true" ]; then DATASET="aletsch"; fi
if [ $DATASET == "aletsch" ]; then
    ./swissclassifier -i ../../../datasets/aletsch/aletsch.csv \
		      -o ../../../datasets/aletsch/aletsch-classified.csv \
		      -a ../../../datasets/aletsch/aletsch.asc \
		      -t ../../../datasets/aletsch/aletsch_terr.trk \
		      -S $strength_threshold -c -r \
                      > ../../../datasets/aletsch/aletsch-classified.csv.out.txt    
fi
