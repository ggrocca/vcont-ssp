#!/bin/bash

if [ $# -eq 1 ]; then
    DIR="$1"
else
    DIR=../../../datasets/dhm25
fi

N=name_pkt-classified.csv
H=hoehenkote-classified.csv
M=morph_kleinform-classified.csv

N100=NAMED_ALPINE_SUMMIT_100
N200=NAMED_MAIN_SUMMIT_200
N300=NAMED_SUMMIT_300
N400=NAMED_MAIN_HILL_400
N500=NAMED_HILL_500
N600=NAMED_ROCK_HEAD_600
N700=NAMED_PASS_700
N800=NAMED_ROAD_PASS_800

echo -e "\n\n  DATABASE: $N \n"

N100_ALL=`cat $DIR/$N | grep -c $N100`
N100_OTHER=`cat $DIR/$N | grep $N100 | grep -c OTHER`
N100_PEAK=`cat $DIR/$N | grep $N100 | grep -c PEAK`
N100_PIT=`cat $DIR/$N | grep $N100 | grep -c PIT`
N100_SADDLE=`cat $DIR/$N | grep $N100 | grep -c SADDLE`
echo -e "$N100 \n\t all=$N100_ALL \n\t other=$N100_OTHER \
     \n\t peak=$N100_PEAK \n\t pit=$N100_PIT \n\t saddle=$N100_SADDLE \n"

N200_ALL=`cat $DIR/$N | grep -c $N200`
N200_OTHER=`cat $DIR/$N | grep $N200 | grep -c OTHER`
N200_PEAK=`cat $DIR/$N | grep $N200 | grep -c PEAK`
N200_PIT=`cat $DIR/$N | grep $N200 | grep -c PIT`
N200_SADDLE=`cat $DIR/$N | grep $N200 | grep -c SADDLE`
echo -e "$N200 \n\t all=$N200_ALL \n\t other=$N200_OTHER \
     \n\t peak=$N200_PEAK \n\t pit=$N200_PIT \n\t saddle=$N200_SADDLE \n"

N300_ALL=`cat $DIR/$N | grep -c $N300`
N300_OTHER=`cat $DIR/$N | grep $N300 | grep -c OTHER`
N300_PEAK=`cat $DIR/$N | grep $N300 | grep -c PEAK`
N300_PIT=`cat $DIR/$N | grep $N300 | grep -c PIT`
N300_SADDLE=`cat $DIR/$N | grep $N300 | grep -c SADDLE`
echo -e "$N300 \n\t all=$N300_ALL \n\t other=$N300_OTHER \
     \n\t peak=$N300_PEAK \n\t pit=$N300_PIT \n\t saddle=$N300_SADDLE \n"

N400_ALL=`cat $DIR/$N | grep -c $N400`
N400_OTHER=`cat $DIR/$N | grep $N400 | grep -c OTHER`
N400_PEAK=`cat $DIR/$N | grep $N400 | grep -c PEAK`
N400_PIT=`cat $DIR/$N | grep $N400 | grep -c PIT`
N400_SADDLE=`cat $DIR/$N | grep $N400 | grep -c SADDLE`
echo -e "$N400 \n\t all=$N400_ALL \n\t other=$N400_OTHER \
     \n\t peak=$N400_PEAK \n\t pit=$N400_PIT \n\t saddle=$N400_SADDLE \n"

N500_ALL=`cat $DIR/$N | grep -c $N500`
N500_OTHER=`cat $DIR/$N | grep $N500 | grep -c OTHER`
N500_PEAK=`cat $DIR/$N | grep $N500 | grep -c PEAK`
N500_PIT=`cat $DIR/$N | grep $N500 | grep -c PIT`
N500_SADDLE=`cat $DIR/$N | grep $N500 | grep -c SADDLE`
echo -e "$N500 \n\t all=$N500_ALL \n\t other=$N500_OTHER \
     \n\t peak=$N500_PEAK \n\t pit=$N500_PIT \n\t saddle=$N500_SADDLE \n"

N600_ALL=`cat $DIR/$N | grep -c $N600`
N600_OTHER=`cat $DIR/$N | grep $N600 | grep -c OTHER`
N600_PEAK=`cat $DIR/$N | grep $N600 | grep -c PEAK`
N600_PIT=`cat $DIR/$N | grep $N600 | grep -c PIT`
N600_SADDLE=`cat $DIR/$N | grep $N600 | grep -c SADDLE`
echo -e "$N600 \n\t all=$N600_ALL \n\t other=$N600_OTHER \
     \n\t peak=$N600_PEAK \n\t pit=$N600_PIT \n\t saddle=$N600_SADDLE \n"

N700_ALL=`cat $DIR/$N | grep -c $N700`
N700_OTHER=`cat $DIR/$N | grep $N700 | grep -c OTHER`
N700_PEAK=`cat $DIR/$N | grep $N700 | grep -c PEAK`
N700_PIT=`cat $DIR/$N | grep $N700 | grep -c PIT`
N700_SADDLE=`cat $DIR/$N | grep $N700 | grep -c SADDLE`
echo -e "$N700 \n\t all=$N700_ALL \n\t other=$N700_OTHER \
     \n\t peak=$N700_PEAK \n\t pit=$N700_PIT \n\t saddle=$N700_SADDLE \n"

N800_ALL=`cat $DIR/$N | grep -c $N800`
N800_OTHER=`cat $DIR/$N | grep $N800 | grep -c OTHER`
N800_PEAK=`cat $DIR/$N | grep $N800 | grep -c PEAK`
N800_PIT=`cat $DIR/$N | grep $N800 | grep -c PIT`
N800_SADDLE=`cat $DIR/$N | grep $N800 | grep -c SADDLE`
echo -e "$N800 \n\t all=$N800_ALL \n\t other=$N800_OTHER \
     \n\t peak=$N800_PEAK \n\t pit=$N800_PIT \n\t saddle=$N800_SADDLE \n"


echo -e "\n\n  DATABASE: $H \n"

H100=HEIGHT_LFP1_100
H200=HEIGHT_LV95_200
H300=HEIGHT_AGNES_300,
H400=HEIGHT_NATURAL_400
H500=HEIGHT_HUMAN_500
H600=HEIGHT_LAKE_600

H100_ALL=`cat $DIR/$H | grep -c $H100`
H100_OTHER=`cat $DIR/$H | grep $H100 | grep -c OTHER`
H100_PEAK=`cat $DIR/$H | grep $H100 | grep -c PEAK`
H100_PIT=`cat $DIR/$H | grep $H100 | grep -c PIT`
H100_SADDLE=`cat $DIR/$H | grep $H100 | grep -c SADDLE`
echo -e "$H100 \n\t all=$H100_ALL \n\t other=$H100_OTHER \
     \n\t peak=$H100_PEAK \n\t pit=$H100_PIT \n\t saddle=$H100_SADDLE \n"

H200_ALL=`cat $DIR/$H | grep -c $H200`
H200_OTHER=`cat $DIR/$H | grep $H200 | grep -c OTHER`
H200_PEAK=`cat $DIR/$H | grep $H200 | grep -c PEAK`
H200_PIT=`cat $DIR/$H | grep $H200 | grep -c PIT`
H200_SADDLE=`cat $DIR/$H | grep $H200 | grep -c SADDLE`
echo -e "$H200 \n\t all=$H200_ALL \n\t other=$H200_OTHER \
     \n\t peak=$H200_PEAK \n\t pit=$H200_PIT \n\t saddle=$H200_SADDLE \n"

H300_ALL=`cat $DIR/$H | grep -c $H300`
H300_OTHER=`cat $DIR/$H | grep $H300 | grep -c OTHER`
H300_PEAK=`cat $DIR/$H | grep $H300 | grep -c PEAK`
H300_PIT=`cat $DIR/$H | grep $H300 | grep -c PIT`
H300_SADDLE=`cat $DIR/$H | grep $H300 | grep -c SADDLE`
echo -e "$H300 \n\t all=$H300_ALL \n\t other=$H300_OTHER \
     \n\t peak=$H300_PEAK \n\t pit=$H300_PIT \n\t saddle=$H300_SADDLE \n"

H400_ALL=`cat $DIR/$H | grep -c $H400`
H400_OTHER=`cat $DIR/$H | grep $H400 | grep -c OTHER`
H400_PEAK=`cat $DIR/$H | grep $H400 | grep -c PEAK`
H400_PIT=`cat $DIR/$H | grep $H400 | grep -c PIT`
H400_SADDLE=`cat $DIR/$H | grep $H400 | grep -c SADDLE`
echo -e "$H400 \n\t all=$H400_ALL \n\t other=$H400_OTHER \
     \n\t peak=$H400_PEAK \n\t pit=$H400_PIT \n\t saddle=$H400_SADDLE \n"

H500_ALL=`cat $DIR/$H | grep -c $H500`
H500_OTHER=`cat $DIR/$H | grep $H500 | grep -c OTHER`
H500_PEAK=`cat $DIR/$H | grep $H500 | grep -c PEAK`
H500_PIT=`cat $DIR/$H | grep $H500 | grep -c PIT`
H500_SADDLE=`cat $DIR/$H | grep $H500 | grep -c SADDLE`
echo -e "$H500 \n\t all=$H500_ALL \n\t other=$H500_OTHER \
     \n\t peak=$H500_PEAK \n\t pit=$H500_PIT \n\t saddle=$H500_SADDLE \n"

H600_ALL=`cat $DIR/$H | grep -c $H600`
H600_OTHER=`cat $DIR/$H | grep $H600 | grep -c OTHER`
H600_PEAK=`cat $DIR/$H | grep $H600 | grep -c PEAK`
H600_PIT=`cat $DIR/$H | grep $H600 | grep -c PIT`
H600_SADDLE=`cat $DIR/$H | grep $H600 | grep -c SADDLE`
echo -e "$H600 \n\t all=$H600_ALL \n\t other=$H600_OTHER \
     \n\t peak=$H600_PEAK \n\t pit=$H600_PIT \n\t saddle=$H600_SADDLE \n"


echo -e "\n\n  DATABASE: $M \n"

M100=MORPH_DOLINE_100
M200=MORPH_ERRATIC_200
M300=MORPH_BLOCK_300
M400=MORPH_SINK_400

M100_ALL=`cat $DIR/$M | grep -c $M100`
M100_OTHER=`cat $DIR/$M | grep $M100 | grep -c OTHER`
M100_PEAK=`cat $DIR/$M | grep $M100 | grep -c PEAK`
M100_PIT=`cat $DIR/$M | grep $M100 | grep -c PIT`
M100_SADDLE=`cat $DIR/$M | grep $M100 | grep -c SADDLE`
echo -e "$M100 \n\t all=$M100_ALL \n\t other=$M100_OTHER \
     \n\t peak=$M100_PEAK \n\t pit=$M100_PIT \n\t saddle=$M100_SADDLE \n"

M200_ALL=`cat $DIR/$M | grep -c $M200`
M200_OTHER=`cat $DIR/$M | grep $M200 | grep -c OTHER`
M200_PEAK=`cat $DIR/$M | grep $M200 | grep -c PEAK`
M200_PIT=`cat $DIR/$M | grep $M200 | grep -c PIT`
M200_SADDLE=`cat $DIR/$M | grep $M200 | grep -c SADDLE`
echo -e "$M200 \n\t all=$M200_ALL \n\t other=$M200_OTHER \
     \n\t peak=$M200_PEAK \n\t pit=$M200_PIT \n\t saddle=$M200_SADDLE \n"

M300_ALL=`cat $DIR/$M | grep -c $M300`
M300_OTHER=`cat $DIR/$M | grep $M300 | grep -c OTHER`
M300_PEAK=`cat $DIR/$M | grep $M300 | grep -c PEAK`
M300_PIT=`cat $DIR/$M | grep $M300 | grep -c PIT`
M300_SADDLE=`cat $DIR/$M | grep $M300 | grep -c SADDLE`
echo -e "$M300 \n\t all=$M300_ALL \n\t other=$M300_OTHER \
     \n\t peak=$M300_PEAK \n\t pit=$M300_PIT \n\t saddle=$M300_SADDLE \n"

M400_ALL=`cat $DIR/$M | grep -c $M400`
M400_OTHER=`cat $DIR/$M | grep $M400 | grep -c OTHER`
M400_PEAK=`cat $DIR/$M | grep $M400 | grep -c PEAK`
M400_PIT=`cat $DIR/$M | grep $M400 | grep -c PIT`
M400_SADDLE=`cat $DIR/$M | grep $M400 | grep -c SADDLE`
echo -e "$M400 \n\t all=$M400_ALL \n\t other=$M400_OTHER \
     \n\t peak=$M400_PEAK \n\t pit=$M400_PIT \n\t saddle=$M400_SADDLE \n"

