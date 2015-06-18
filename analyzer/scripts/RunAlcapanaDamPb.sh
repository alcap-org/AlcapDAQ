#! /bin/bash

RunNumbers=( 56{62..69} )
RunNumbers=( 54{33..48} )


AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/damien"
DataDir="$AlcapanaPath/Alhists"
mkdir -p $DataDir

odbedit -c "rm Analyzer/WireMap"
odbedit -c "load $HOME/AlcapDAQ/analyzer/odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp /archive/data/alcap/R15a/mid/run0${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    $AlcapanaPath/alcapana -i /archive/data/alcap/R15a/mid/run0${Run}.mid -o $DataDir/hist0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    echo "" >> output.txt
done   
