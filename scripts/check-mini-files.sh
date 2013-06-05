#!/bin/bash

source mini-conf.sh

for i in $DATASETS ; do
    for j in $FILETYPES ; do
	echo "Checking $NEWRES/$i.$j"
	if [ -f $NEWRES/$i.$j ] ; then
	    diff $NEWRES/$i.$j $OLDRES/$i.$j
	else
	    echo "!! No .$j file $NEWRES/$i.$j"
	fi
    done
done
