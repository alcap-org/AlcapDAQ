#! /bin/bash

#RunNumbers=( 3190 )
#RunNumbers=( 3{190..192} )
RunNumbers=( 28{26..73} )

DataDir="$HOME/data"
mkdir -p $DataDir
RootanaPath="$HOME/AlcapDAQ/analyzer/rootana"

for Run in ${RunNumbers[@]};do 
        echo Now running $Run

        $RootanaPath/rootana -i $DataDir/tree0${Run}.root -o $DataDir/out0${Run}_EvdE.root 2>&1 |tee $DataDir/rootanaTextOutput$Run > output.txt
	echo "" >> output.txt

done
