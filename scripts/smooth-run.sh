#!/bin/bash

source smooth-conf.sh

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ -n $DO_RANGE_NORMAL ]; then
echo $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL"
$EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" > "$RES"/"$RANGE"_"$NORMAL".txt
fi

if [ -n $DO_RANGE_SMOOTH ]; then
echo $EXE $RANGE_OPTS $SMOOTH_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$SMOOTH" -t "$RES"/"$RANGE"_"$SMOOTH"
$EXE $RANGE_OPTS $SMOOTH_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$SMOOTH" -t "$RES"/"$RANGE"_"$SMOOTH" > "$RES"/"$RANGE"_"$SMOOTH".txt
fi
if [ -n $DO_DEM_NORMAL ]; then
echo $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL"
$EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" > "$RES"/"$DEM"_"$NORMAL".txt
fi

if [ -n $DO_DEM_SMOOTH ]; then
echo $EXE $DEM_OPTS $SMOOTH_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$SMOOTH" -t "$RES"/"$DEM"_"$SMOOTH"
$EXE $DEM_OPTS $SMOOTH_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$SMOOTH" -t "$RES"/"$DEM"_"$SMOOTH" > "$RES"/"$DEM"_"$SMOOTH".txt
fi
