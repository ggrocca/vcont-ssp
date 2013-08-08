EXE=../scaletracker/scaletracker
RES=../../scalespace-results/filter

SOURCE_DIR="../../datasets"
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

NUM_LEVELS=3

RANGE="range500"
RANGE_OPTS=" -n $NUM_LEVELS -b 10 -m 255 "

DEM="dem300"
DEM_OPTS=" -n $NUM_LEVELS -c 2700 3000 3300 3600 -m 14 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0 -s "
DO_DEM_NORMAL="yes"
DO_RANGE_NORMAL="yes"

FILTER_SLOW="filter_slow"
FILTER_SLOW_OPTS=" -p 0.0001 0 -f 0 -s "
DO_DEM_FILTER_SLOW="yes"
DO_RANGE_FILTER_SLOW="yes"

FILTER_NORM="filter_norm"
FILTER_NORM_OPTS=" -p 0.0001 0 -f 0 -s "
DO_DEM_FILTER_NORM="yes"
DO_RANGE_FILTER_NORM="yes"

FILTER_FAST="filter_fast"
FILTER_FAST_OPTS=" -p 0.0001 0 -f 0 -s "
DO_DEM_FILTER_FAST="yes"
DO_RANGE_FILTER_FAST="yes"


DATASETS="$RANGE $DEM"

CRT=crt
TRK=trk
SSP=ssp
FILETYPES="$CRT $TRK $SSP"

DIFFTIFF="../analyzers/difftiff/difftiff -v 0"
DIFFTRAC="../analyzers/difftrack/difftrack -v 0"
DIFF="diff -q"
