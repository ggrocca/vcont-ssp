EXE=../scaletracker/scaletracker
RES=../../scalespace-results/integral

SOURCE_DIR="../../datasets"
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

NUM_LEVELS=10
RANGE="range500"
RANGE_OPTS=" -n $NUM_LEVELS -b 10 -m 255 "

DEM="dem1200"
DEM_OPTS=" -n $NUM_LEVELS -m 14 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0 -l "

DO_RANGE_NORMAL="yes"
DO_DEM_NORMAL="yes"

DATASETS="$RANGE $DEM"
