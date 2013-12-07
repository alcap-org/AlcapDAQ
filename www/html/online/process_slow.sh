#!/bin/bash

dir=$1
macro=$2
rollback=$3

. process_common.sh

echo $dir $macro $run

cd $dir
$cmd $macro.C+\($rollback\)
cd ..

