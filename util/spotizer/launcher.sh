#!/bin/bash

function resdir_setup()
{
    if [ -d $1 ]; then
	rm $1/*
    fi
    
    if [ ! -d $1 ]; then
	mkdir $1
    fi
}

if [ $# -ne "8" ]; then
    echo "Usage $0 DATASET[aletsch] MODE[basic] CURVSET CURVFACTOR DISTQUERY BORDERCUT PRUNE[y\|n] CSV[all|classified|refined]"
    echo "Usage $0 DATASET[aletsch] MODE[query] LIFE_STEPS STRENGTH_STEPS DISTQUERY BORDERCUT PRUNE[y|n] CSV[file]"
    echo "Usage $0 DATASET[dhm25|dhm25_generic] SET[aargau-lucerne|lucerne|crop_named_peaks] LIFE_STEPS STRENGTH_STEPS DISTQUERY BORDERCUT PRUNE[y|n] CSV[file]"
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
if [ $DATASET == "dhm25_generic" ]; then
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
    BASEDIR="../../../datasets/aletsch"
    NAME="aletsch"
fi
if [ $DATASET == "dhm25" ]; then
    BASEDIR="../../../datasets/dhm25"
    NAME=$SET
fi
if [ $DATASET == "dhm25_generic" ]; then
    BASEDIR="../../../datasets/dhm25"
    NAME=$SET
fi

if [ "$MODE" == "basic" ]; then
    RESDIR=$BASEDIR/$NAME"-stats-basic-"$CSVMODE"-c"$CURVSET"-f"$CURVFACTOR"-d"$DIST"-b"$BCUT$PRUNE
elif [ "$MODE" == "query" ]; then
    RESDIR=$BASEDIR/$NAME"-stats-query-"$CSVMODE"-l"$LIFE_STEPS"-s"$STRENGTH_STEPS"-d"$DIST"-b"$BCUT$PRUNE
fi
    
if [ ! -d $BASEDIR ]; then
    echo basedir $BASEDIR does not exist
    exit 1
fi

if [ "$MODE" == "basic" ]; then

    resdir_setup $RESDIR

    echo ./spotizer -t $BASEDIR/$NAME"_terr.trk" -c $BASEDIR/$NAME"_curv_"$CURVSET".trk" \
	 -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
	 -F $CURVFACTOR -D $DIST -C $BCUT $PRUNE \
	 -o $RESDIR/"stats" \
	 ">" $RESDIR/"stats.txt"
    ./spotizer -t $BASEDIR/$NAME"_terr.trk" -c $BASEDIR/$NAME"_curv_"$CURVSET".trk" \
    	       -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    	       -F $CURVFACTOR -D $DIST -C $BCUT $PRUNE \
    	       -o $RESDIR/"stats" \
    	       > $RESDIR/"stats.txt"
    echo gnuplot -e "dir='$RESDIR'" plot_basic.gp
    gnuplot -e "dir='$RESDIR'" plot_basic.gp

elif [ "$MODE" == "query" ]; then

    if [[ "$DATASET" != "dhm25_generic" ]]; then
	resdir_setup $RESDIR
	
	echo ./spotizer -t $BASEDIR/$NAME"_terr.trk" \
    	     -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    	     -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    	     -o $RESDIR/"stats" \
    	     ">" $RESDIR/"stats.txt"
	./spotizer -t $BASEDIR/$NAME"_terr.trk" \
    		   -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    		   -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    		   -o $RESDIR/"stats" \
    		   > $RESDIR/"stats.txt"
	echo gnuplot -e "dir='$RESDIR'" plot_query.gp
	gnuplot -e "dir='$RESDIR'" plot_query.gp
    fi

    if [[ "$SET" == "crop_named_peaks" && "$DATASET" == "dhm25_generic" ]]; then
	STRENGTH_STEPS=1
	
	DROPDIR=$RESDIR"_DROP"
	resdir_setup $DROPDIR
	echo ./spotizer -g $BASEDIR/$NAME"_drop.csv" \
    		   -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    		   -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    		   -o $DROPDIR/"stats" \
    		   ">" $DROPDIR/"stats.txt"	
	./spotizer -g $BASEDIR/$NAME"_drop.csv" \
    		   -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    		   -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    		   -o $DROPDIR/"stats" \
    		   > $DROPDIR/"stats.txt"
	echo gnuplot -e "dir='$DROPDIR'" plot_generic.gp
	gnuplot -e "dir='$DROPDIR'" plot_generic.gp
	
	JAARADIR=$RESDIR"_JAARA"
	resdir_setup $JAARADIR
	echo ./spotizer -g $BASEDIR/$NAME"_jaara.csv" \
    		   -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    		   -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    		   -o $JAARADIR/"stats" \
    		   ">" $JAARADIR/"stats.txt"
	./spotizer -g $BASEDIR/$NAME"_jaara.csv" \
    		   -a $BASEDIR/$NAME".asc" -s $BASEDIR/$CSV \
    		   -Q $LIFE_STEPS $STRENGTH_STEPS -D $DIST -C $BCUT $PRUNE \
    		   -o $JAARADIR/"stats" \
    		   > $JAARADIR/"stats.txt"
	echo gnuplot -e "dir='$JAARADIR'" plot_generic.gp
	gnuplot -e "dir='$JAARADIR'" plot_generic.gp

    fi    
fi
