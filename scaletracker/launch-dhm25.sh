#!/bin/bash

/usr/bin/time -f "%E real,%U user,%S sys" \
	      -o ../../datasets/dhm25/aargau-lucerne_terr.time.txt \
	      ./scaletracker \
	      -i ../../datasets/dhm25/aargau-lucerne.asc \
	      -o ../../datasets/dhm25/aargau-lucerne_terr \
	      -a -n 17 -p 0.01 0 \
	      > ../../datasets/dhm25/aargau-lucerne_terr.out.txt \
	      2> ../../datasets/dhm25/aargau-lucerne_terr.err.txt

/usr/bin/time -f "%E real,%U user,%S sys" \
	      -o ../../datasets/dhm25/lucerne_terr.time.txt \
	      ./scaletracker \
	      -i ../../datasets/dhm25/lucerne.asc \
	      -o ../../datasets/dhm25/lucerne_terr \
	      -a -n 17 -p 0.01 0 \
	      > ../../datasets/dhm25/lucerne_terr.out.txt 2> \
	      ../../datasets/dhm25/lucerne_terr.err.txt

