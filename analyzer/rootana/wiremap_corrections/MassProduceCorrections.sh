#!/bin/bash
# Use to make the same correction to multiple files

RunNumbers=( 3{188..220} )
TextToAdd="CaUH\tADCSlopeCalib\t0.1222\nCaUH\tADCOffsetCalib\t0.5"

for Run in ${RunNumbers[@]}; do

    if [ ! -e correct${Run}.dat ]
    then
	echo "Bank name  |  Field      | New value" > correct${Run}.dat
	echo "--------------------------------" >> correct${Run}.dat
    fi
    echo -e ${TextToAdd} >> correct${Run}.dat

done