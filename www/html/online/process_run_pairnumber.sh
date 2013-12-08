#!/bin/bash

dir=$1
macro=$2
run=$3
pair_nr=$4

echo $dir $macro $run $pair_nr

. process_common.sh

cd $dir
$cmd $macro.C+\($run,$pair_nr\)
cd ..

