#!/bin/bash

source curv-conf.sh
cp curv-conf.sh "$RES"/_curv_conf.sh.txt

if [ ! -d "$RES" ] ; then
    mkdir -p "$RES"
fi

if [ $DO_CURV = "yes" ]; then
    ( set -x; $EXE $OPTS -r "$SOURCE_DIR"/"$CURV"."$SSP" -o  "$RES"/"$CURV"_"$READ" -t "$RES"/"$CURV"_"$READ" > "$RES"/"$CURV"_"$READ".txt; )
fi
