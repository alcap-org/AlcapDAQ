#!/bin/bash

dir=$1
macro=$2
run=$3
ledbox=$4

echo $dir $macro $run $ledbox

. process_common.sh

cd $dir
$cmd $macro.C+\($run,$ledbox\)
cd ..

