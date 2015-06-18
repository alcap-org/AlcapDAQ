#! /bin/bash

RunNumbers=( 64{27..51} )
RunNumbers=( 645{3..7} )
RunNumbers=( 64{59..61} )
RunNumbers=( 64{63..75} )
RunNumbers=( 647{8..9} )
RunNumbers=( 648{2..4} )
RunNumbers=( 648{6..8} )

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
