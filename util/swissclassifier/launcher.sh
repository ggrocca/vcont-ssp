#!/bin/bash

./swissclassifier -i ../../../datasets/dhm25/morph_kleinform.csv \
		  -o ../../../datasets/dhm25/morph_kleinform-classified.csv \
		  -a ../../../datasets/dhm25/aargau-lucerne.asc \
		  -t ../../../datasets/dhm25/aargau-lucerne_terr.trk \
		  -S 1.0 -c

./swissclassifier -i ../../../datasets/dhm25/name_pkt.csv \
		  -o ../../../datasets/dhm25/name_pkt-classified.csv \
		  -a ../../../datasets/dhm25/aargau-lucerne.asc \
		  -t ../../../datasets/dhm25/aargau-lucerne_terr.trk \
		  -S 1.0 -c


./swissclassifier -i ../../../datasets/dhm25/hoehenkote.csv \
		  -o ../../../datasets/dhm25/hoehenkote-classified.csv \
		  -a ../../../datasets/dhm25/aargau-lucerne.asc \
		  -t ../../../datasets/dhm25/aargau-lucerne_terr.trk \
		  -S 1.0 -c
