#! /bin/bash

Runs=( 10{387..421} )

for run in $@;do
  echo "UPDATE alcapana_v3  SET status=\"F\", user=\"edmonds\", tree=\"/home/edmonds/data/tree/v3/tree$run.root\" where run=$run;"
done
