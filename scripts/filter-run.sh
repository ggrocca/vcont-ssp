#!/bin/bash

source filter-conf.sh
cp filter-conf.sh "$RES"/_filter-conf.sh.txt

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ $DO_RANGE_NORMAL = "yes" ]; then
    ( set -x; $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" >> "$RES"/"$RANGE"_"$NORMAL".txt; )
fi

if [ $DO_DEM_NORMAL = "yes" ]; then
    ( set -x; $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" >> "$RES"/"$DEM"_"$NORMAL".txt; )
fi

if [ $DO_DEM_FILTER_SLOW = "yes" ]; then
    ( set -x; $EXE $DEM_OPTS $FILTER_SLOW_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$FILTER_SLOW" -t "$RES"/"$DEM"_"$FILTER_SLOW" >> "$RES"/"$DEM"_"$FILTER_SLOW".txt; )
fi

if [ $DO_RANGE_FILTER_SLOW = "yes" ]; then
    ( set -x; $EXE $RANGE_OPTS $FILTER_SLOW_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$FILTER_SLOW" -t "$RES"/"$RANGE"_"$FILTER_SLOW" >> "$RES"/"$RANGE"_"$FILTER_SLOW".txt; )
fi

if [ $DO_DEM_FILTER_NORM = "yes" ]; then
    ( set -x; $EXE $DEM_OPTS $FILTER_NORM_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$FILTER_NORM" -t "$RES"/"$DEM"_"$FILTER_NORM" >> "$RES"/"$DEM"_"$FILTER_NORM".txt; )
fi

if [ $DO_RANGE_FILTER_NORM = "yes" ]; then
    ( set -x; $EXE $RANGE_OPTS $FILTER_NORM_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$FILTER_NORM" -t "$RES"/"$RANGE"_"$FILTER_NORM" >> "$RES"/"$RANGE"_"$FILTER_NORM".txt; )
fi

if [ $DO_DEM_FILTER_FAST = "yes" ]; then
    ( set -x; $EXE $DEM_OPTS $FILTER_FAST_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$FILTER_FAST" -t "$RES"/"$DEM"_"$FILTER_FAST" >> "$RES"/"$DEM"_"$FILTER_FAST".txt; )
fi

if [ $DO_RANGE_FILTER_FAST = "yes" ]; then
    ( set -x; $EXE $RANGE_OPTS $FILTER_FAST_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$FILTER_FAST" -t "$RES"/"$RANGE"_"$FILTER_FAST" >> "$RES"/"$RANGE"_"$FILTER_FAST".txt; )
fi



