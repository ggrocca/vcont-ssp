#!/bin/bash

RD=../../scalespace-results
EXE=./scaletracker

if [ ! -d $RD ]; then
    mkdir $RD
    mkdir $RD/presmooth/
    mkdir $RD/normal/
fi

### NEW (tests)

echo $EXE -n 12 -i ../../range500.png -o "$RD/presmooth/range500.track"    -s -p 0.0001 1 -t "$RD/presmooth/range500n"    -b 10                  
echo $EXE -n 15 -i ../../N45E007.hgt -o  "$RD/presmooth/terrain1200.track" -s -p 0.0001 1 -t "$RD/presmooth/terrain1200n" -c 2400 2400 3600 3600

echo $EXE -n 12 -i ../../range500.png -o "$RD/normal/range500.track"    -p 0.0001 1 -t "$RD/normal/range500n"    -b 10                  
echo $EXE -n 15 -i ../../N45E007.hgt -o  "$RD/normal/terrain1200.track" -p 0.0001 1 -t "$RD/normal/terrain1200n" -c 2400 2400 3600 3600 

# $EXE -n 12 -i ../../range500.png -o "$RD/presmooth/range500.track"    -s -p 0.0001 1 -t "$RD/presmooth/range500n"    -b 10                  > $RD/presmooth/range500.txt
# $EXE -n 15 -i ../../N45E007.hgt -o  "$RD/presmooth/terrain1200.track" -s -p 0.0001 1 -t "$RD/presmooth/terrain1200n" -c 2400 2400 3600 3600 > $RD/presmooth/terrain1200.txt

# $EXE -n 12 -i ../../range500.png -o "$RD/normal/range500.track"    -p 0.0001 1 -t "$RD/normal/range500n"    -b 10                  > $RD/normal/range500.txt
# $EXE -n 15 -i ../../N45E007.hgt -o  "$RD/normal/terrain1200.track" -p 0.0001 1 -t "$RD/normal/terrain1200n" -c 2400 2400 3600 3600 > $RD/normal/terrain1200.txt
