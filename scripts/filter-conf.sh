EXE=../scaletracker/scaletracker


RES=../../scalespace-results/filter

SOURCE_DIR="../.."
SOURCE_RANGE="range500.png"
SOURCE_DEM="N45E007.hgt"

RANGE="range500"
RANGE_OPTS=" -n 12 -b 10 "

DEM="dem1200"
DEM_OPTS=" -n 15 -c 2400 2400 3600 3600 "

NORMAL="normal"
NORMAL_OPTS=" -p 0.0001 0"

FILTER="filter"
FILTER_OPTS=" -p 0.0001 0 -f "

DO_RANGE_NORMAL="yes"
DO_RANGE_FILTER="yes"
DO_DEM_NORMAL="yes"
DO_DEM_FILTER="yes"

# CRT=crt
# TRK=trk
# FILETYPES="$CRT $TRK"
