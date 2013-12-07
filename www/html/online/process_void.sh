#!/bin/bash


dir=$1
macro=$2

. process_common.sh

echo $dir $macro

cd $dir
$cmd $macro.C+\(\)
cd ..

