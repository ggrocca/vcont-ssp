#!/bin/bash

source smooth-conf.sh
cp smooth-conf.sh "$RES"/_smooth-conf.sh.txt

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ $DO_RANGE_NORMAL = "yes" ]; then
( set -x; $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" > "$RES"/"$RANGE"_"$NORMAL".out.txt 2> "$RES"/"$RANGE"_"$NORMAL".err.txt; )
fi

if [ $DO_RANGE_SMOOTH = "yes" ]; then
( set -x; $EXE $RANGE_OPTS $SMOOTH_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$SMOOTH" -t "$RES"/"$RANGE"_"$SMOOTH" > "$RES"/"$RANGE"_"$SMOOTH".out.txt 2> "$RES"/"$RANGE"_"$SMOOTH".err.txt; )
fi

if [ $DO_DEM_NORMAL = "yes" ]; then
( set -x; $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" > "$RES"/"$DEM"_"$NORMAL".out.txt 2> "$RES"/"$DEM"_"$NORMAL".err.txt; )
fi

if [ $DO_DEM_SMOOTH = "yes" ]; then
( set -x; $EXE $DEM_OPTS $SMOOTH_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$SMOOTH" -t "$RES"/"$DEM"_"$SMOOTH" > "$RES"/"$DEM"_"$SMOOTH".out.txt 2> "$RES"/"$DEM"_"$SMOOTH".err.txt; )
fi
