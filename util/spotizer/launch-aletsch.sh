#!/bin/bash

if [ $# -ne "8" ]; then
    echo "Usage $0 DATASET[aletsch] MODE[basic] CURVSET CURVFACTOR DISTQUERY BORDERCUT PRUNE[y\|n] CSV[all|classified|refined]"
    echo "Usage $0 DATASET[aletsch] MODE[query] LIFE_STEPS STRENGTH_STEPS DISTQUERY BORDERCUT PRUNE[y|n] CSV[all|classified|refined]"
    echo "Usage $0 DATASET[dhm25] SET[aargau-lucerne|lucerne] LIFE_STEPS STRENGTH_STEPS DISTQUERY BORDERCUT PRUNE[y|n] CSV[???]"
    exit 1
fi

DATASET=$1

if [ $DATASET == "aletsch" ]; then
    MODE=$2
fi
if [ $DATASET == "dhm25" ]; then
    MODE="query"
    SET=$2
fi

if [ "$MODE" == "basic" ]; then
    CURVSET=$3
    CURVFACTOR=$4
elif [ "$MODE" == "query" ]; then
    LIFE_STEPS=$3
    STRENGTH_STEPS=$4
else
    echo Unexpected mode $MODE
    exit 1
fi
DIST=$5
BCUT=$6
IS_PRUNE=$7
if [ "$IS_PRUNE" == "y" ]; then
    PRUNE="-P"
elif [ "$IS_PRUNE" == "n" ]; then
    PRUNE=""
else
    echo Prune must be \"y\" or \"n\", not $IS_PRUNE
    exit 1
fi

CSVMODE=$8
CSV=$CSVMODE.csv
# if [ $DATASET == "aletsch" ]; then
#     if [ "$CSVMODE" == "all" ]; then
#     	CSV="aletsch.csv"
#     elif [ "$CSVMODE" == "classified" ]; then
#     	CSV="classified.csv"
#     elif [ "$CSVMODE" == "refined" ]; then
#     	CSV="refined.csv"
#     else
#     	echo Prune must be \"all\", \"classified\" or \"refined\", not $CSVMODE
#     	exit 1
#     fi
# fi
# if [ $DATASET == "dhm25" ]; then
#     # GG-TBD select CSV
#     CSV="blah.csv"
# fi

#echo $CURVSET $CURVFACTOR $DIST

if [ $DATASET == "aletsch" ]; then
    BASEDIR="../../../datasets/aletsch/"
    NAME="aletsch"
fi
if [ $DATASET == "dhm25" ]; then
    BASEDIR="../../../datasets/dhm25/"
    NAME=$SET
fi

if [ "$MODE" == "basic" ]; then
    RESDIR=$BASEDIR$NAME"-stats-basic-"$CSVMODE"-c"$CURVSET"-f"$CURVFACTOR"-d"$DIST"-b"$BCUT$PRUNE"/"
elif [ "$MODE" == "query" ]; then
    RESDIR=$BASEDIR$NAME"-stats-query-"$CSVMODE"-l"$LIFE_STEPS"-s"$STRENGTH_STEPS"-d"$DIST"-b"$BCUT$PRUNE"/"
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

    echo ./spotizer -t $BASEDIR$NAME"_terr.trk" -c $BASEDIR$NAME"_curv_"$CURVSET".trk" \
	 -a $BASEDIR$NAME".asc" -s $BASEDIR$CSV \
	 -F $CURVFACTOR -D $DIST -C $BCUT $PRUNE \
	 -o $RESDIR"stats" \
	 ">" $RESDIR"stats.txt"

    ./spotizer -t $BASEDIR$NAME"_terr.trk" -c $BASEDIR$NAME"_curv_"$CURVSET".trk" \
    	       -a $BASEDIR$NAME".asc" -s $BASEDIR$CSV \
    	       -F $CURVFACTOR -D $DIST -C $BCUT $PRUNE \
    	       -o $RESDIR"stats" \
    	       > $RESDIR"stats.txt"

    gnuplot -e "dir='$RESDIR'" plot_basic.gp

elif [ "$MODE" == "query" ]; then

    echo ./spotizer -t $BASEDIR$NAME"_terr.trk" \
	 -a $BASEDIR$NAME".asc" -s $BASEDIR$CSV \
	 -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
	 -o $RESDIR"stats" \
	 ">" $RESDIR"stats.txt"

    ./spotizer -t $BASEDIR$NAME"_terr.trk" \
    	       -a $BASEDIR$NAME".asc" -s $BASEDIR$CSV \
    	       -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    	       -o $RESDIR"stats" \
    	       > $RESDIR"stats.txt"

    gnuplot -e "dir='$RESDIR'" plot_query.gp

fi
