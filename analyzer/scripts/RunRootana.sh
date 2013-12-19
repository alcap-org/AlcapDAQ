#! /bin/bash

#RunNumbers=( 3190 )
RunNumbers=( 3{190..192} )

DataDir="$HOME/data"
mkdir -p $DataDir
RootanaPath="$HOME/AlcapDAQ/analyzer/rootana"

for Run in ${RunNumbers[@]};do 
        echo Now running $Run

        $RootanaPath/rootana -i $DataDir/tree0${Run}_test.root -o $DataDir/out0${Run}.root 2>&1 |tee $DataDir/rootanaTextOutput$Run > output.txt
	echo "" >> output.txt

done
