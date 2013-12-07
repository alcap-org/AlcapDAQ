#!/bin/bash

dir=$1
macro=$2
run_1=$3
run_2=$4
rollback=$5

. process_common.sh

echo $dir $macro $run_1 $run_2 $rollback

cd $dir
$cmd $macro.C+\($run_1,$run_2,$rollback\)
cd ..

