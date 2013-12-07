#!/bin/bash

dir=$1
macro=$2
run=$3
SN1=$4
SN2=$5

. process_common.sh

echo $dir $macro $run $SN1 $SN2



cd $dir
$cmd $macro.C+\($run,$SN1,$SN2\)
cd ..

