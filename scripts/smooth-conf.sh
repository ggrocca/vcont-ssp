EXE=../scaletracker/scaletracker


RES=../../scalespace-results/smooth

SOURCE_DIR="../.."
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

RANGE="range500"
RANGE_OPTS=" -n 12 -b 10 -m 255 "

DEM="dem1200"
DEM_OPTS=" -n 15 -c 2400 2400 3600 3600 -m 14 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0 "

SMOOTH="smooth"
SMOOTH_OPTS=" -p 0.0001 0 -s "

DO_RANGE_NORMAL="yes"
DO_RANGE_SMOOTH="yes"
DO_DEM_NORMAL="yes"
DO_DEM_SMOOTH="yes"


# DATASETS="$NORMAL1000 $SMOOTH1000"

CRT=crt
TRK=trk
FILETYPES="$CRT $TRK"
