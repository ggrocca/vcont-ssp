#!/bin/bash

function print_help {
    echo "Usage: $0" "[all | aargau-lucerne | lucerne | crop_named_peaks | aletsch_terr | aletsch_curv] [drop]" 
}

if [ $# -lt "1" ]; then
    print_help
    exit
fi

ALL=false
if [ $1 == "all" ]; then
    ALL=true
else
    DATASET=$1
fi

DROPOPT=""
if [ $# == "2" ]; then
    if [ $2 == "drop" ]; then
	DROPOPT="-D"
    fi
fi

if [ $ALL == "true" ]; then DATASET="aargau-lucerne"; fi
if [ $DATASET == "aargau-lucerne" ]; then
    echo "aargau-lucerne"
    set -x
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/dhm25/aargau-lucerne_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/dhm25/aargau-lucerne.asc \
    		  -o ../../datasets/dhm25/aargau-lucerne_terr \
    		  -a -n 17 -p 0.01 0 $DROPOPT \
    		  > ../../datasets/dhm25/aargau-lucerne_terr.out.txt \
    		  2> ../../datasets/dhm25/aargau-lucerne_terr.err.txt
    ../util/track2csv/track2csv -a ../../datasets/dhm25/aargau-lucerne.asc \
				-t ../../datasets/dhm25/aargau-lucerne_terr.trk \
				-o ../../datasets/dhm25/aargau-lucerne_terr.trk.csv \
    set +x
fi

if [ $ALL == "true" ]; then DATASET="lucerne"; fi
if [ $DATASET == "lucerne" ]; then
    echo "lucerne"
    set -x
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/dhm25/lucerne_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/dhm25/lucerne.asc \
    		  -o ../../datasets/dhm25/lucerne_terr \
    		  -a -n 17 -p 0.01 0 $DROPOPT \
    		  > ../../datasets/dhm25/lucerne_terr.out.txt 2> \
    		  ../../datasets/dhm25/lucerne_terr.err.txt
        ../util/track2csv/track2csv -a ../../datasets/dhm25/lucerne.asc \
				-t ../../datasets/dhm25/lucerne_terr.trk \
				-o ../../datasets/dhm25/lucerne_terr.trk.csv \
    set +x
fi

if [ $ALL == "true" ]; then DATASET="crop_named_peaks"; fi
if [ $DATASET == "crop_named_peaks" ]; then
    echo "crop_named_peaks"
    set -x
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/dhm25/crop_named_peaks_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/dhm25/aargau-lucerne.asc \
    		  -o ../../datasets/dhm25/crop_named_peaks_terr \
    		  -a -n 17 -p 0.01 0 -c 207 6 1573 1140 $DROPOPT \
    		  > ../../datasets/dhm25/crop_named_peaks_terr.out.txt 2> \
    		  ../../datasets/dhm25/crop_named_peaks_terr.err.txt
    ../util/track2csv/track2csv -a ../../datasets/dhm25/crop_named_peaks.asc \
				-t ../../datasets/dhm25/crop_named_peaks_terr.trk \
				-o ../../datasets/dhm25/crop_named_peaks_terr.trk.csv \
    #quick hack for util/spotizer/launcher.sh compatibility
    cp ../../datasets/dhm25/crop_named_peaks_terr_crop.asc \
       ../../datasets/dhm25/crop_named_peaks.asc
    set +x
fi

if [ $ALL == "true" ]; then DATASET="aletsch_terr"; fi
if [ $DATASET == "aletsch_terr" ]; then
    echo "aletsch"
    set -x
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/aletsch/aletsch_terr.time.txt \
    		  ./scaletracker \
    		  -i ../../datasets/aletsch/aletsch.asc \
    		  -o ../../datasets/aletsch/aletsch_terr \
    		  -a -n 15 -p 0.01 0 $DROPOPT \
    		  > ../../datasets/aletsch/aletsch_terr.out.txt \
    		  2> ../../datasets/aletsch/aletsch_terr.err.txt
    ../util/track2csv/track2csv -a ../../datasets/dhm25/aletsch.asc \
				-t ../../datasets/dhm25/aletsch_terr.trk \
				-o ../../datasets/dhm25/aletsch_terr.trk.csv \
    set +x
fi

if [ $ALL == "true" ]; then DATASET="aletsch_curv_8"; fi
if [ $DATASET == "aletsch_curv_8" ]; then
    set -x
    /usr/bin/time -f "%E real,%U user,%S sys" \
    		  -o ../../datasets/aletsch/aletsch_curv.time.txt \
		  ./scaletracker \
		  -d ../../datasets/aletsch/aletsch_curv_8.dem \
		  -o ../../datasets/aletsch/aletsch_curv_8 \
		  -n 15 -a \
		  > ../../datasets/aletsch/aletsch_curv_8.out.txt \
		  2> ../../datasets/aletsch/aletsch_curv_8.err.txt    
    set +x
fi
