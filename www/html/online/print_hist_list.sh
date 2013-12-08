#!/bin/bash

run_nr=$1

. process_common.sh

cd common
$cmd print_hist_list.C+\($run_nr\) > /dev/null
cd ..

cat tmp/hist_list.dat


