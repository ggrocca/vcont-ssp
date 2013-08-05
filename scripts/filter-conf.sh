EXE=../scaletracker/scaletracker


RES=../../scalespace-results/filter

SOURCE_DIR="../../datasets"
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

RANGE="range500"
RANGE_OPTS=" -n 3 -b 10 -m 255 "

DEM="dem300"
DEM_OPTS=" -n 3 -c 2700 3000 3300 3600 -m 14 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0 -s "

FILTER="filter"
FILTER_OPTS=" -p 0.0001 0 -f -s "

DO_RANGE_NORMAL="yes"
DO_RANGE_FILTER="yes"
DO_DEM_NORMAL="yes"
DO_DEM_FILTER="yes"

# CRT=crt
# TRK=trk
# FILETYPES="$CRT $TRK"
