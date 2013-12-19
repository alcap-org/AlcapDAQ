#! /bin/bash

#RunNumbers=( 3190 )
RunNumbers=( 3190 )

DataDir="$HOME/data"
mkdir -p $DataDir
AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/${USER}"

odbedit -c "rm Analyzer/WireMap"
odbedit -c "load ../odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp /net/abner/data/RunPSI2013/mid/run0${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    $AlcapanaPath/alcapana -i /net/abner/data/RunPSI2013/mid/run0${Run}.mid -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    echo "" >> output.txt
done
