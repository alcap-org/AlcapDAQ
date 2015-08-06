#! /bin/bash

RunNumbers=( 33{55..63} )
#RunNumbers=( 28{27..73} )

DataDir="$HOME/data"
mkdir -p $DataDir
AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/${USER}"

odbedit -c "rm Analyzer/WireMap"
odbedit -c "load $HOME/AlcapDAQ/analyzer/odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp $DataDir/odb/run0${Run}.odb $AlcapanaPath/currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    wget ftp://archivftp.psi.ch/mu2e/run2013/run0${Run}.mid
    mv run0${Run}.mid $DataDir/raw/run0${Run}.mid

    $AlcapanaPath/alcapana -i $DataDir/raw/run0${Run}.mid -o $DataDir/hist/hist0${Run}.root -T $DataDir/tree/tree0${Run}.root 2>&1 |tee $DataDir/log/textOutput$Run > output.txt

    rm $DataDir/raw/run0${Run}.mid # to save space
    echo "" >> output.txt
done
