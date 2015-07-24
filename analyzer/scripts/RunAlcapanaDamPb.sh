#! /bin/bash

#RunNumbers=( 56{62..69} )
#RunNumbers=( 54{33..48} )
RunNumbers=( 66{58..74} 66{76..91} 6{692..711} )

AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/damien"
DataDir="$AlcapanaPath/Pbhists_620"
mkdir -p $DataDir

odbedit -c "rm Analyzer/WireMap"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp /home/damien/data/R15a/run0${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    $AlcapanaPath/alcapana -i /home/damien/data/R15a/run0${Run}.mid -o $DataDir/hist0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    echo "" >> output.txt
done   
