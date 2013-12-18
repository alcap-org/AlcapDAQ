#! /bin/bash

#RunNumbers=( 3012 )
RunNumbers=( 3012 300{6..8} 299{7..9} 300{0..2} 299{4,5} 29{59..92} )

DataDir="$HOME/data"
AlcapanaPath="$HOME/AlcapDAQ/analyzer/work/krikler"

for Run in ${RunNumbers[@]};do 
        echo Now running $Run
        cp /net/abner/data/RunPSI2013/mid/run0${Run}.odb currentRun.odb
        odbedit -c " load currentRun.odb"
        $AlcapanaPath/alcapana -i /net/abner/data/RunPSI2013/mid/run0${Run}.mid -o $DataDir/hist${Run}.root -T $DataDir/tree${Run}.root 2>&1 |tee $DataDir/textOutput$Run >output.txt
        echo ""> output.txt
done
