#!/bin/bash

ALL=false
if [ $# == "0" ]; then
    ALL=true
else
    DATASET=$1
fi

if [ $ALL == "true" ]; then DATASET="dhm25"; fi
if [ $DATASET == "dhm25" ]; then
    ./swissclassifier -i ../../../datasets/dhm25/morph_kleinform.csv \
		      -o ../../../datasets/dhm25/morph_kleinform-classified.csv \
		      -a ../../../datasets/dhm25/aargau-lucerne.asc \
		      -t ../../../datasets/dhm25/aargau-lucerne_terr.trk \
		      -S 1.0 -c
    
    ./swissclassifier -i ../../../datasets/dhm25/name_pkt.csv \
		      -o ../../../datasets/dhm25/name_pkt-classified.csv \
		      -a ../../../datasets/dhm25/aargau-lucerne.asc \
		      -t ../../../datasets/dhm25/aargau-lucerne_terr.trk \
		      -S 1.0 -c


    ./swissclassifier -i ../../../datasets/dhm25/hoehenkote.csv \
		      -o ../../../datasets/dhm25/hoehenkote-classified.csv \
		      -a ../../../datasets/dhm25/aargau-lucerne.asc \
		      -t ../../../datasets/dhm25/aargau-lucerne_terr.trk \
		      -S 1.0 -c

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
		      -S 1.0 -c
    
fi
