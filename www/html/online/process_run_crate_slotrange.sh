#!/bin/bash

dir=$1
macro=$2
run=$3
crate=$4
slot1=$5
slot2=$6

echo $dir $macro $run $crate $slot1 $slot2

. process_common.sh

cd $dir
$cmd $macro.C+\($run,$crate,$slot1,$slot2\)
cd ..

