#! /bin/bash

RunNumbers=( 65{25..56} 65{58..63} )


AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/damien"
DataDir="$AlcapanaPath/Tihists"
mkdir -p $DataDir

odbedit -c "rm Analyzer/WireMap"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp /archive/alcap/R15a/mid/run0${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    $AlcapanaPath/alcapana -i /archive/alcap/R15a/mid/run0${Run}.mid -o $DataDir/hist0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    echo "" >> output.txt
done   
