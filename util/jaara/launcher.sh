#!/bin/bash

function print_help {
    echo "$0" "[aargau-lucerne | crop_named_peaks | aletsch]"
}

if [ $# != 1 ]; then
    print_help
    exit 0
fi

if [ $1 == "aargau-lucerne" ]; then 
    ./drop2csv.sh ../scalespace/datasets/dhm25/aargau-lucerne.asc ../scalespace/datasets/dhm25/aargau-lucerne_drop
    ./jaara2csv.sh ../scalespace/datasets/dhm25/aargau-lucerne.asc ../scalespace/datasets/dhm25/aargau-lucerne_jaara 1 350
    exit
fi

if [ $1 == "crop_named_peaks" ]; then 
    ./drop2csv.sh ../scalespace/datasets/dhm25/crop_named_peaks.asc ../scalespace/datasets/dhm25/crop_named_peaks_drop
    ./jaara2csv.sh ../scalespace/datasets/dhm25/crop_named_peaks.asc ../scalespace/datasets/dhm25/crop_named_peaks_jaara 1 350
    exit
fi

if [ $1 == "aletsch" ]; then 
    ./drop2csv.sh ../scalespace/datasets/aletsch/aletsch.asc ../scalespace/datasets/aletsch/aletsch_drop
    ./jaara2csv.sh ../scalespace/datasets/aletsch/aletsch.asc ../scalespace/datasets/aletsch/aletsch_jaara 1 350
    exit
fi

print_help
