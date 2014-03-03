#!/bin/bash
# check_missing_runs.sh
# -- checks for any runs that are missing between two values
# -- assumes that your tree files are in ~/data/tree
# USAGE: . ./check_missing_runs.sh FIRST_RUN LAST_RUN

if [ $# -ne 2 ] ; then
  echo "Incorrect! Usage: ./check_missing_runs.sh FIRST_RUN LAST_RUN"
  exit 1
fi

FIRST_RUN=$1
LAST_RUN=$2

echo "Checking for missing runs in ~/data/tree"

for i in `seq -f "%05g" $FIRST_RUN $LAST_RUN`
do
    FILENAME=~/data/tree/tree$i.root
    if [ ! -f $FILENAME ] ; then
	echo $FILENAME "is missing"
    fi
done