#!/bin/bash

RD=../../scalespace-results
EXE=./scaletracker

if [ ! -d $RD ]; then
    mkdir $RD
    mkdir $RD/mini/
fi

$EXE -n 3 -i ../../N45E007.hgt -m 14 -o  "$RD/mini/smooth1000.track" -s -p 0.0001 1 -t "$RD/mini/smooth1000n" -c 2500 2500 3500 3500 > $RD/mini/smooth1000.txt

$EXE -n 3 -i ../../N45E007.hgt -m 14 -o  "$RD/mini/normal1000.track" -p 0.0001 1 -t "$RD/mini/normal1000n" -c 2500 2500 3500 3500 > $RD/mini/normal1000.txt
