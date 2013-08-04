#!/bin/bash

source filter-conf.sh

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ $DO_RANGE_NORMAL = "yes" ]; then
echo $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL"
echo $EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" > "$RES"/"$RANGE"_"$NORMAL".txt
$EXE $RANGE_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$NORMAL" -t "$RES"/"$RANGE"_"$NORMAL" >> "$RES"/"$RANGE"_"$NORMAL".txt
fi

if [ $DO_DEM_NORMAL = "yes" ]; then
echo $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL"
echo $EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" > "$RES"/"$DEM"_"$NORMAL".txt
$EXE $DEM_OPTS $NORMAL_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$NORMAL" -t "$RES"/"$DEM"_"$NORMAL" >> "$RES"/"$DEM"_"$NORMAL".txt
fi

if [ $DO_DEM_FILTER = "yes" ]; then
echo $EXE $DEM_OPTS $FILTER_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$FILTER" -t "$RES"/"$DEM"_"$FILTER"
echo $EXE $DEM_OPTS $FILTER_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$FILTER" -t "$RES"/"$DEM"_"$FILTER" > "$RES"/"$DEM"_"$FILTER".txt
$EXE $DEM_OPTS $FILTER_OPTS -i $SOURCE_DIR/$SOURCE_DEM -o  "$RES"/"$DEM"_"$FILTER" -t "$RES"/"$DEM"_"$FILTER" >> "$RES"/"$DEM"_"$FILTER".txt
fi

if [ $DO_RANGE_FILTER = "yes" ]; then
echo $EXE $RANGE_OPTS $FILTER_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$FILTER" -t "$RES"/"$RANGE"_"$FILTER"
echo $EXE $RANGE_OPTS $FILTER_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$FILTER" -t "$RES"/"$RANGE"_"$FILTER" > "$RES"/"$RANGE"_"$FILTER".txt
$EXE $RANGE_OPTS $FILTER_OPTS -i $SOURCE_DIR/$SOURCE_RANGE -o  "$RES"/"$RANGE"_"$FILTER" -t "$RES"/"$RANGE"_"$FILTER" >> "$RES"/"$RANGE"_"$FILTER".txt
fi


