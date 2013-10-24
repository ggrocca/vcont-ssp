#!/bin/bash

source integral-conf.sh

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

cp integral-conf.sh "$RES"/_integral_conf.sh.txt

if [ $DO_RANGE_NORMAL = "yes" ]; then
    ( set -x; $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" > "$RES"/"$RANGE"_"$NORMAL".txt; )
fi

if [ $DO_DEM_NORMAL = "yes" ]; then
    (set -x; $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" > "$RES"/"$DEM"_"$NORMAL".txt; )
fi
