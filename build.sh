#!/bin/bash

TARGET=""

if (( $# == 0 )); then
    TARGET=""
elif (( $# > 1 )); then
    print_usage;
    exit 1
elif [ $1 == all ]; then
    TARGET=all    
elif [ $1 == debug ]; then
    TARGET=debug
elif [ $1 == clean ]; then
    TARGET=clean
# elif [ $1 == debug ]; then
#     TARGET=test
fi

STARTDIR=`pwd`

DIRS="analyzers/difftiff analyzers/difftrack util/mesher scaletracker trackviewer"

FAILSTAGE=-1;

COUNTER=0;
for d in $DIRS; do

    echo; echo; echo;
    echo "cd $d; make $TARGET"
    echo;
    cd $d; make $TARGET

   if (( $? != 0 && FAILSTAGE == -1 )); then
	FAILSTAGE=$COUNTER
   fi

    COUNTER=$((COUNTER + 1))
    cd $STARTDIR;
done;

if (( $FAILSTAGE != -1 )); then
    
    fd=""
    COUNTER=0;
    for d in $DIRS; do
	if (( $FAILSTAGE == $COUNTER )); then
	    fd=$d
	fi
	COUNTER=$((COUNTER + 1))
    done

    echo; echo; echo;
    echo Make error
    echo stage $FAILSTAGE, dir $fd.
    echo
fi

