#!/bin/bash

if [ $USER == "gg" ]; then
    paperdir=/home/gg/Repositories/papers/iciap2015
fi

cp group1_nose.pdf group2_eyes.pdf group3_mouth.pdf $paperdir/img/
