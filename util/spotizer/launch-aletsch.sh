#!/bin/bash

# GG make new parameter for name and dir

if [ $# -ne "6" ]; then
    echo Usage $0 MODE[basic] CURVSET CURVFACTOR DISTQUERY BORDERCUT PRUNE[y\|n]
    echo Usage $0 MODE[query] LIFE_STEPS STRENGTH_STEPS DISTQUERY BORDERCUT PRUNE[y\|n]
    exit 1
fi

MODE=$1
if [ "$MODE" == "basic" ]; then
    CURV=$2
    FACTOR=$3
elif [ "$MODE" == "query" ]; then
    LIFE=$2
    STRENGTH=$3
else
    echo Unexpected mode $MODE
    exit 1
fi
DIST=$4
BCUT=$5
IS_PRUNE=$6
if [ "$IS_PRUNE" == "y" ]; then
    PRUNE="-P"
elif [ "$IS_PRUNE" == "n" ]; then
    PRUNE=""
else
    echo Prune must be \"y\" or \"n\", not $IS_PRUNE
    exit 1
fi

#echo $CURV $FACTOR $DIST

BASEDIR="../../../datasets/swiss/"
NAME="aletsch"

if [ "$MODE" == "basic" ]; then
    RESDIR=$BASEDIR"stats-basic-c"$CURV"-f"$FACTOR"-d"$DIST"-b"$BCUT$PRUNE"/"
elif [ "$MODE" == "query" ]; then
    RESDIR=$BASEDIR"stats-query-l"$LIFE"-s"$STRENGTH"-d"$DIST"-b"$BCUT$PRUNE"/"
fi
    
if [ ! -d $BASEDIR ]; then
    echo basedir $BASEDIR does not exist
    exit 1
fi

if [ -d $RESDIR ]; then
    rm $RESDIR/*
fi

if [ ! -d $RESDIR ]; then
    mkdir $RESDIR
fi

if [ "$MODE" == "basic" ]; then

    echo ./spotizer -t $BASEDIR$NAME"_terr.trk" -c $BASEDIR$NAME"_curv_"$CURV".trk" \
	 -a $BASEDIR$NAME".asc" -s $BASEDIR"classified.csv" \
	 -F $FACTOR -D $DIST -C $BCUT $PRUNE \
	 -o $RESDIR"stats" \
	 ">" $RESDIR"stats.txt"

    ./spotizer -t $BASEDIR$NAME"_terr.trk" -c $BASEDIR$NAME"_curv_"$CURV".trk" \
    	       -a $BASEDIR$NAME".asc" -s $BASEDIR"classified.csv" \
    	       -F $FACTOR -D $DIST -C $BCUT $PRUNE \
    	       -o $RESDIR"stats" \
    	       > $RESDIR"stats.txt"

    gnuplot -e "dir='$RESDIR'" plot_basic.gp

elif [ "$MODE" == "query" ]; then

    echo ./spotizer -t $BASEDIR$NAME"_terr.trk" \
	 -a $BASEDIR$NAME".asc" -s $BASEDIR"classified.csv" \
	 -Q $LIFE $STRENGTH -D $DIST -C $BCUT $PRUNE \
	 -o $RESDIR"stats" \
	 ">" $RESDIR"stats.txt"

    ./spotizer -t $BASEDIR$NAME"_terr.trk" \
    	       -a $BASEDIR$NAME".asc" -s $BASEDIR"classified.csv" \
    	       -Q $LIFE $STRENGTH -D $DIST -C $BCUT $PRUNE \
    	       -o $RESDIR"stats" \
    	       > $RESDIR"stats.txt"

    gnuplot -e "dir='$RESDIR'" plot_query.gp

fi
