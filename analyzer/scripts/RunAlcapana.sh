#! /bin/bash

#RunNumbers=( 2826 )
#RunNumbers=( 28{27..73} )
#RunNumbers=(09139)
#RunNumbers=(09282 09684 09686)
RunNumbers=(10204)

DataDir="$HOME/data"
#mkdir -p $DataDir
AlcapDAQPath="$HOME/AlcapDAQ"
AlcapanaPath="$AlcapDAQPath/analyzer/work/${USER}"
R15bRawPath="/data/R15b"

odbedit -c "rm Analyzer/WireMap"
odbedit -c "load $AlcapDAQPath/analyzer/odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do 
    echo Now running $Run
    cp $R15bRawPath/odb/run${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    cp $HOME/data/odb_wm/wm${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    echo ""
    
    $AlcapanaPath/alcapana -i $R15bRawPath/mid/run${Run}.mid -o $DataDir/hist/local/hist${Run}.root -T $DataDir/tree/local/tree${Run}.root 2>&1 |tee $DataDir/log/local/textOutput$Run > output.txt
    echo "" >> output.txt
done
