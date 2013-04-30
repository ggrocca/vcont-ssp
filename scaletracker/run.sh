#!/bin/bash

RD=../../smoother-results

#### OLD

# terrain 3601
#./smoother -n 7 -j 1 -i ../../N45E007.hgt -o "$RD/terrain3601.track" -p 0.0001 1 -t "$RD/terrain3601n" > $RD/terrain3601.txt
# terrain cropped
#./smoother -n 7 -j 1 -i ../../N45E007.hgt -o "$RD/terrain1200.track" -p 0.0001 1 -t "$RD/terrain1200n" -c 2400 2400 3600 3600 > $RD/terrain1200.txt
# lena gray level
#./smoother -n 7 -j 1 -i ../../lena512.bmp -o "$RD/lena512.track" -p 0.0001 1 -t "$RD/lena512n" > $RD/lena512.txt
# range image
#./smoother -n 7 -j 1 -i ../../range500.png -o "$RD/range500.track" -p 0.0001 1 -t "$RD/range500n" -b 10 > $RD/range500.txt
# range image no clip
#./smoother -n 7 -j 1 -i ../../range500.png -o "$RD/range-noclip-500.track" -p 0.0001 1 -t "$RD/range-noclip-500n" > $RD/range-noclip-500.txt


# range image
#./smoother -n 12 -i ../../range500.png -o "$RD/long/range500.track"    -s -p 0.0001 1 -t "$RD/long/range500n"    -b 10                  > $RD/long/range500.txt
#./smoother -n 9 -i ../../range500.png -o "$RD/range500.track"    -s -p 0.0001 1 -t "$RD/range500n"    -b 10                  > $RD/range500.txt

# lena gradient
#./smoother -n 9 -i ../../gradN.tif -o "$RD/gradN.track" -s -p 0.0001 1 -t "$RD/gradNn"                  > $RD/gradN.txt
#./smoother -n 9 -i ../../gradN.tif -o "$RD/clip-gradN.track" -s -p 0.0001 1 -t "$RD/clip-gradNn" -b 10    > $RD/clip-gradN.txt

# lena gray level
#./smoother -n 9 -i ../../lena512.bmp  -o "$RD/lena512.track"     -s -p 0.0001 1 -t "$RD/lena512n"                            > $RD/lena512.txt

# terrain cropped
#./smoother -n 15 -i ../../N45E007.hgt -o "$RD/long/terrain1200.track" -s -p 0.0001 1 -t "$RD/long/terrain1200n" -c 2400 2400 3600 3600 > $RD/long/terrain1200.txt
#./smoother -n 9 -i ../../N45E007.hgt -o "$RD/terrain1200.track" -s -p 0.0001 1 -t "$RD/terrain1200n" -c 2400 2400 3600 3600 > $RD/terrain1200.txt

# terrain 3601
#./smoother -n 9 -i ../../N45E007.hgt  -o "$RD/terrain3601.track" -s -p 0.0001 1 -t "$RD/terrain3601n"                        > $RD/terrain3601.txt

### ICIAP 2013?

# range image
#./smoother -n 12 -i ../../range500.png -o "$RD/long/range500.track"    -s -p 0.0001 1 -t "$RD/long/range500n"    -b 10                  > $RD/long/range500.txt
#./smoother -n 9 -i ../../range500.png -o "$RD/range500.track"    -s -p 0.0001 1 -t "$RD/range500n"    -b 10                  > $RD/range500.txt

# terrain cropped
#./smoother -n 15 -i ../../N45E007.hgt -o "$RD/long/terrain1200.track" -s -p 0.0001 1 -t "$RD/long/terrain1200n" -c 2400 2400 3600 3600 > $RD/long/terrain1200.txt
#./smoother -n 9 -i ../../N45E007.hgt -o "$RD/terrain1200.track" -s -p 0.0001 1 -t "$RD/terrain1200n" -c 2400 2400 3600 3600 > $RD/terrain1200.txt
#./smoother -n 12 -i ../../N45E007.hgt -o "$RD/medium/terrain1200.track" -s -p 0.0001 1 -t "$RD/medium/terrain1200n" -c 2400 2400 3600 3600 > $RD/medium/terrain1200.txt


### NEW (tests)

./smoother -n 12 -i ../../range500.png -o "$RD/presmooth/range500.track"    -s -p 0.0001 1 -t "$RD/presmooth/range500n"    -b 10                  > $RD/presmooth/range500.txt
./smoother -n 15 -i ../../N45E007.hgt -o  "$RD/presmooth/terrain1200.track" -s -p 0.0001 1 -t "$RD/presmooth/terrain1200n" -c 2400 2400 3600 3600 > $RD/presmooth/terrain1200.txt

./smoother -n 12 -i ../../range500.png -o "$RD/normal/range500.track"    -p 0.0001 1 -t "$RD/normal/range500n"    -b 10                  > $RD/normal/range500.txt
./smoother -n 15 -i ../../N45E007.hgt -o  "$RD/normal/terrain1200.track" -p 0.0001 1 -t "$RD/normal/terrain1200n" -c 2400 2400 3600 3600 > $RD/normal/terrain1200.txt
