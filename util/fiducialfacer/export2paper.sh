#!/bin/bash

if [ $USER == "gg" ]; then
    paperdir=/home/gg/Repositories/papers/iciap2015
fi

cp group*.pdf $paperdir/img/
