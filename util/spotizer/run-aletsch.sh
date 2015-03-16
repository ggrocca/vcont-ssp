#!/bin/bash

if [[ $1 == "all" || $1 == "normal" ]]; then
    ./launcher.sh aletsch query 100 100 3.0 0.1 y aletsch-classified 1.02 1.06
    ./launcher.sh aletsch_generic query 1000 1 3.0 0.1 y aletsch-classified 1.006 0
    ./launcher.sh aletsch_generic query 100 1 3.0 0.1 y aletsch-classified 1.06 0
fi

if [[ $1 == "all" || $1 == "fix_strength" ]]; then
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 0.0000000 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 1.1111111 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 2.3333333 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 5.4444444 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 8.8888888 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 11.490625 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 13.282430 15
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 20.222222 40
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 35.555555 40
fi

if [[ $1 == "all" || $1 == "fix_life" ]]; then
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 0.000000 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 0.111111 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 0.333333 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 0.555555 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 0.601377 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 0.888888 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 1.111111 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 1.584783 5
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 2.222222 10
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 3.333333 10
    ./launcher.sh aletsch query 1 100 3.0 0.1 y aletsch-classified 0 1.06 0 0 4.444444 10
fi

if  [[ $1 == "all" || $1 == "range" ]]; then
    ./launcher.sh aletsch query 100 100 3.0 0.1 y aletsch-classified 1.02 0 2 40
    # ./launcher.sh aletsch query 100 100 3.0 0.1 y aletsch-classified 1.02 0 2 35
    # ./launcher.sh aletsch query 100 100 3.0 0.1 y aletsch-classified 1.02 0 5 35
fi

if  [[ $1 == "all" || $1 == "paper" ]]; then
    ./launcher.sh aletsch_generic query 1000 1 3.0 0.1 y aletsch-classified 1.006 0
    ./launcher.sh aletsch query 100 1 3.0 0.1 y aletsch-classified 1.02 0 13.282430 15
    ./launcher.sh aletsch query 100 100 3.0 0.1 y aletsch-classified 1.02 0 2 40
    ./launcher.sh aletsch query 100 100 3.0 0.1 y aletsch-classified 1.02 1.06
fi
