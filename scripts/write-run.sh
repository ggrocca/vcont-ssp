#!/bin/bash

source write-conf.sh

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ -n $DO_RANGE_NORMAL ]; then
#echo $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL"
( set -x; $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" > "$RES"/"$RANGE"_"$NORMAL".txt; )
fi

if [ -n $DO_DEM_NORMAL ]; then
#echo $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL"
(set -x; $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" > "$RES"/"$DEM"_"$NORMAL".txt; )
fi

if [ -n $DO_RANGE_READ ]; then
#echo $EXE -r "$RES"/"$RANGE"_"$NORMAL"."$SSP" -o  "$RES"/"$RANGE"_"$READ" -t "$RES"/"$RANGE"_"$READ"
( set -x; $EXE $READ_RANGE_OPTS -r "$RES"/"$RANGE"_"$NORMAL"."$SSP" -o  "$RES"/"$RANGE"_"$READ" -t "$RES"/"$RANGE"_"$READ" > "$RES"/"$RANGE"_"$READ".txt; )
fi

if [ -n $DO_DEM_READ ]; then
#echo $EXE -r "$RES"/"$DEM"_"$NORMAL"."$SSP" -o  "$RES"/"$DEM"_"$READ" -t "$RES"/"$DEM"_"$READ"
( set -x; $EXE $READ_DEM_OPTS -r "$RES"/"$DEM"_"$NORMAL"."$SSP" -o  "$RES"/"$DEM"_"$READ" -t "$RES"/"$DEM"_"$READ" > "$RES"/"$DEM"_"$READ".txt; )
fi
