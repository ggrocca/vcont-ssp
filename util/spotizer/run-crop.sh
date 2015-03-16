#!/bin/bash

if [[ $1 == "all" || $1 == "normal" ]]; then
    
    ./launcher.sh dhm25 crop_named_peaks 100 100 3.0 0.13 y natural 1.02 1.06
    ./launcher.sh dhm25 crop_named_peaks 100 100 3.0 0.13 y allpeaks 1.02 1.06
    ./launcher.sh dhm25 crop_named_peaks 100 100 3.0 0.13 y mainpeaks 1.02 1.06

    ./launcher.sh dhm25_generic crop_named_peaks 1000 1 3.0 0.13 y natural 1.006 0
    ./launcher.sh dhm25_generic crop_named_peaks 1000 1 3.0 0.13 y allpeaks 1.006 0
    ./launcher.sh dhm25_generic crop_named_peaks 1000 1 3.0 0.13 y mainpeaks 1.006 0

    ./launcher.sh dhm25_generic crop_named_peaks 100 1 3.0 0.13 y natural 1.06 0
    ./launcher.sh dhm25_generic crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.06 0
    ./launcher.sh dhm25_generic crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.06 0

fi

if [[ $1 == "all" || $1 == "fix_strength" ]]; then
    
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 0.000000 10
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 1.111111 10
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 2.222222 10
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 5.555555 10
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 7.687155 10
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 8.888888 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 12.22222 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 18.11111 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 30.00000 50

    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 0.0000000 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 10.000000 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 20.000000 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 35.000000 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 45.996038 50
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 55.000000 100
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 70.000000 100
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 90.000000 100

    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 0.0000000 80
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 15.000000 80
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 30.000000 80
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 60.000000 80
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 74.697446 80
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 90.000000 200
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 120.00000 200
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y mainpeaks 1.02 0 170.00000 200

fi

if [[ $1 == "all" || $1 == "fix_life" ]]; then

    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 0.000000 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 0.111111 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 0.333333 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 0.561106 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 0.752277 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 1.000000 10
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 3.000000 10
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y natural 0 1.06 0 0 7.000000 10

    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 0.000000 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 0.222222 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 0.444444 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 0.777777 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 0.955150 1
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 3.000000 20
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 8.000000 20
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y allpeaks 0 1.06 0 0 15.00000 20
    
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 0.000000 2
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 0.111111 2
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 0.266675 2
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 0.777777 2
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 1.398906 2
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 3.000000 20
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 8.000000 20
    ./launcher.sh dhm25 crop_named_peaks 1 100 3.0 0.13 y mainpeaks 0 1.06 0 0 15.00000 20

    
fi

if  [[ $1 == "all" || $1 == "range" ]]; then

    ./launcher.sh dhm25 crop_named_peaks 100 40 3.0 0.13 y natural 1.02 0 1 40
    ./launcher.sh dhm25 crop_named_peaks 100 80 3.0 0.13 y allpeaks 1.02 0 1 80
    ./launcher.sh dhm25 crop_named_peaks 100 100 3.0 0.13 y mainpeaks 1.02 0 1 120
    
    # ./launcher.sh dhm25 crop_named_peaks 100 10 3.0 0.13 y natural 1.02 0 2 10
    # ./launcher.sh dhm25 crop_named_peaks 100 40 3.0 0.13 y allpeaks 1.02 0 2 60
    # ./launcher.sh dhm25 crop_named_peaks 100 60 3.0 0.13 y mainpeaks 1.02 0 2 100
fi


if  [[ $1 == "all" || $1 == "paper" ]]; then
    ./launcher.sh dhm25_generic crop_named_peaks 1000 1 3.0 0.13 y natural 1.006 0
    ./launcher.sh dhm25_generic crop_named_peaks 1000 1 3.0 0.13 y allpeaks 1.006 0
    ./launcher.sh dhm25_generic crop_named_peaks 100 1 3.0 0.13 y natural 1.06 0
    ./launcher.sh dhm25_generic crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.06 0
    
    ./launcher.sh dhm25 crop_named_peaks 100 100 3.0 0.13 y natural 1.02 1.06
    ./launcher.sh dhm25 crop_named_peaks 100 100 3.0 0.13 y allpeaks 1.02 1.06
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y natural 1.02 0 7.687155 10
    ./launcher.sh dhm25 crop_named_peaks 100 1 3.0 0.13 y allpeaks 1.02 0 45.996038 50
    ./launcher.sh dhm25 crop_named_peaks 100 40 3.0 0.13 y natural 1.02 0 1 40
    ./launcher.sh dhm25 crop_named_peaks 100 80 3.0 0.13 y allpeaks 1.02 0 1 80
fi
