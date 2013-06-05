#!/bin/bash

source mini-conf.sh

if [ ! -d "$NEWRES" ] ; then
    mkdir -p "$NEWRES"
fi

if [ -n $NORMAL1000 ]; then
$EXE -n $NUM_LEVELS $NORMAL1000_OPTS -i ../../N45E007.hgt -o  "$NEWRES"/"$NORMAL1000" -t "$NEWRES"/"$NORMAL1000"n > "$NEWRES"/"$NORMAL1000".txt
fi

if [ -n $SMOOTH1000 ]; then
$EXE -n $NUM_LEVELS $SMOOTH1000_OPTS -i ../../N45E007.hgt -o "$NEWRES"/"$SMOOTH1000" -t "$NEWRES"/"$SMOOTH1000"n > "$NEWRES"/"$SMOOTH1000".txt
fi

#RD=../../scalespace-results
#EXE=./scaletracker

# if [ ! -d $RD ]; then
#     mkdir $RD
#     mkdir $RD/mini/
# fi

#$EXE -n 8 -i ../../N45E007.hgt -m 14 -o  "$RD/mini/nopert1000"                -t "$RD/mini/nopert1000n" -c 2500 2500 3500 3500 > $RD/mini/nopert1000.txt
#$EXE -n 3 -i ../../N45E007.hgt -m 14 -o  "$RD/mini/pd3600.track" -t "$RD/mini/pd3600n" > $RD/mini/pd3600.txt
