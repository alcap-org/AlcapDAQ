#!/bin/bash

RUNS=$(seq $2 $3)

for RUN in $RUNS; do
    ln -s correct$1.dat correct$RUN.dat
done