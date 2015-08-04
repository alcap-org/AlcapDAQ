#! /bin/bash

RunNumbers=( 3360 )
#RunNumbers=( 28{27..73} )

DataDir="$HOME/data"
mkdir -p $DataDir
AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/${USER}"

odbedit -c "rm Analyzer/WireMap"
odbedit -c "load $HOME/AlcapDAQ/analyzer/odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp $DataDir/odb/run0${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    $AlcapanaPath/alcapana -i $DataDir/raw/run0${Run}.mid -o $DataDir/hist/hist0${Run}.root -T $DataDir/tree/tree0${Run}.root 2>&1 |tee $DataDir/log/textOutput$Run > output.txt
    echo "" >> output.txt
done
