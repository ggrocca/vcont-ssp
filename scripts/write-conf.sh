EXE=../scaletracker/scaletracker


RES=../../scalespace-results/write

SOURCE_DIR="../.."
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

RANGE="range500"
RANGE_OPTS=" -n 6 -b 10 -m 255 "

DEM="dem1200"
DEM_OPTS=" -n 4 -c 2400 2400 3600 3600 -m 14 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0 "

READ="read"
#READ_OPTS=""

DO_RANGE_NORMAL="yes"
DO_RANGE_READ="yes"
DO_DEM_NORMAL="yes"
DO_DEM_READ="yes"


# DATASETS="$NORMAL1000 $READ1000"

CRT=crt
TRK=trk
SSP=ssp
FILETYPES="$CRT $TRK"
