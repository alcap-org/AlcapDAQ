#! /bin/bash

#RunNumbers=( 64{27..51} 645{3..7} 64{59..61} 647{8..9} 64{63..75} 648{2..4}  648{6..8})
RunNumbers=( 660{2..8} 66{11..51} 665{3..6} 67{12..19} 67{23..37} 6740, 67{42..53 675{5..8} 6759 67{61..75} 67{77..82} 678{8..8} 679{0..3} 6{797..803} 680{5..8} 681{0..8} )


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
