#!/bin/bash

dir=$1
macro=$2
run=$3

echo $dir $macro $run

. process_common.sh

cd $dir
$cmd $macro.C+\($run\)
cd ..

