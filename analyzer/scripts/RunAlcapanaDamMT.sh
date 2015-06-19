#! /bin/bash

RunNumbers=( 656{4..9} 657{1..4} 65{76..83} 65{85..91} 659{4..5} 6{597..601} )


AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/damien"
DataDir="$AlcapanaPath/MThists"
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
