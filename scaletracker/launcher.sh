#!/bin/bash

ALL=false
if [ $# == "0" ]; then
    ALL=true
else
    DATASET=$1
fi

if [ $ALL == "true" ]; then DATASET="aargau-lucerne"; fi
if [ $DATASET == "aargau-lucerne" ]; then
    echo "aargau-lucerne"
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/dhm25/aargau-lucerne_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/dhm25/aargau-lucerne.asc \
    		  -o ../../datasets/dhm25/aargau-lucerne_terr \
    		  -a -n 17 -p 0.01 0 \
    		  > ../../datasets/dhm25/aargau-lucerne_terr.out.txt \
    		  2> ../../datasets/dhm25/aargau-lucerne_terr.err.txt
fi

if [ $ALL == "true" ]; then DATASET="lucerne"; fi
if [ $DATASET == "lucerne" ]; then
    echo "lucerne"
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/dhm25/lucerne_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/dhm25/lucerne.asc \
    		  -o ../../datasets/dhm25/lucerne_terr \
    		  -a -n 17 -p 0.01 0 \
    		  > ../../datasets/dhm25/lucerne_terr.out.txt 2> \
    		  ../../datasets/dhm25/lucerne_terr.err.txt
fi

if [ $ALL == "true" ]; then DATASET="crop_named_peaks"; fi
if [ $DATASET == "crop_named_peaks" ]; then
    echo "crop_named_peaks"
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/dhm25/crop_named_peaks_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/dhm25/aargau-lucerne.asc \
    		  -o ../../datasets/dhm25/crop_named_peaks_terr \
    		  -a -n 17 -p 0.01 0 -c 207 6 1573 1140\
    		  > ../../datasets/dhm25/crop_named_peaks_terr.out.txt 2> \
    		  ../../datasets/dhm25/crop_named_peaks_terr.err.txt
fi

if [ $ALL == "true" ]; then DATASET="aletsch_terr"; fi
if [ $DATASET == "aletsch_terr" ]; then
    echo "aletsch"
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/aletsch/aletsch_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/aletsch/aletsch.asc \
    		  -o ../../datasets/aletsch/aletsch_terr \
    		  -a -n 15 -p 0.01 0 \
    		  > ../../datasets/aletsch/aletsch_terr.out.txt \
    		  2> ../../datasets/aletsch/aletsch_terr.err.txt
fi

if [ $ALL == "true" ]; then DATASET="aletsch_curv_8"; fi
if [ $DATASET == "aletsch_curv_8" ]; then
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/aletsch/aletsch_curv.time.txt \
		  ./scaletracker \
		  -d ../../datasets/aletsch/aletsch_curv_8.dem \
		  -o ../../datasets/aletsch/aletsch_curv_8 \
		  -n 15 -a \
		  > ../../datasets/aletsch/aletsch_curv_8.out.txt \
		  2> ../../datasets/aletsch/aletsch_curv_8.err.txt    
fi
