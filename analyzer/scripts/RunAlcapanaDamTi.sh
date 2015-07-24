#! /bin/bash

#RunNumbers=( 65{25..56} 65{58..63} )
RunNumbers=( 686{0..2} 68{64..77} 69{05..37} 69{39..61} 69{64..93} )
#RunNumbers=( 70{18..22} 70{24..25} 70{27..29} 70{32..35} 70{37..44} 70{46..85} 70{87..94} 7{097..124} 71{26..40} 71{42..47} 71{49..66} ) 


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
