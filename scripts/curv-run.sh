#!/bin/bash

source write-conf.sh
cp write-conf.sh "$RES"/_write_conf.sh.txt

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ $DO_RANGE_NORMAL = "yes" ]; then
    ( set -x; $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" > "$RES"/"$RANGE"_"$NORMAL".txt; )
fi

if [ $DO_DEM_NORMAL = "yes" ]; then
    (set -x; $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" > "$RES"/"$DEM"_"$NORMAL".txt; )
fi

if [ $DO_RANGE_READ = "yes" ]; then
    ( set -x; $EXE $READ_RANGE_OPTS -r "$RES"/"$RANGE"_"$NORMAL"."$SSP" -o  "$RES"/"$RANGE"_"$READ" -t "$RES"/"$RANGE"_"$READ" > "$RES"/"$RANGE"_"$READ".txt; )
fi

if [ $DO_DEM_READ = "yes" ]; then
    ( set -x; $EXE $READ_DEM_OPTS -r "$RES"/"$DEM"_"$NORMAL"."$SSP" -o  "$RES"/"$DEM"_"$READ" -t "$RES"/"$DEM"_"$READ" > "$RES"/"$DEM"_"$READ".txt; )
fi
