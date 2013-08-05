EXE=../scaletracker/scaletracker
RES=../../scalespace-results/write

SOURCE_DIR="../../datasets"
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

NUM_LEVELS=3
RANGE="range500"
RANGE_OPTS=" -n $NUM_LEVELS -b 10 -m 255 "

DEM="dem1200"
DEM_OPTS=" -n $NUM_LEVELS -c 2400 2400 3600 3600 -m 14 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0 "

READ="read"
READ_DEM_OPTS=" -m 14 "
READ_RANGE_OPTS=" -m 255 "

DO_RANGE_NORMAL="yes"
DO_RANGE_READ="yes"
DO_DEM_NORMAL="yes"
DO_DEM_READ="yes"


DATASETS="$RANGE $DEM"

CRT=crt
TRK=trk
SSP=ssp
FILETYPES="$CRT $TRK $SSP"

DIFFTIFF="../analyzers/difftiff/difftiff -v 0"
DIFFTRAC="../analyzers/difftrack/difftrack -v 0"
DIFF="diff -q"
