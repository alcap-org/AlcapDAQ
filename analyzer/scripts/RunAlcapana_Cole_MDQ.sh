#! /bin/bash

# Al2mm Y
# RunNumbers=( 6892 )
# Cs137 C -- None
# NdetU
# RunNumbers=( 7196 )
# NdetD
# RunNumbers=( 7197 )
# AmBe C
# NdetU 7194, 7195
# NdetD 7192, 7193
RunNumbers=( 7192 7193 7194 7195 )

RawDataDir="$HOME/data/ckampa/AlCap_2015a/raw"
ODBDataDir="$HOME/data/ckampa/AlCap_2015a/odb"
DataDir="$HOME/data/ckampa/AlCap_2015a/output_alcapana"
# FIXME! This will not work when looking at data from other datasets.
# FileStub="$RawDataDir/raw.rhbob.AlCapR2015a.Al2mm_36MeV_Y."
# FileStub="$RawDataDir/raw.rhbob.AlCapR2015a.None_36MeV_C."
FileStub="$RawDataDir/raw.rhbob.AlCapR2015a.AmBe_36MeV_C."
mkdir -p $DataDir
AlcapanaPath="$HOME/coding/AlcapDAQ/analyzer/work/data_quality"

# odbedit -c "rm Analyzer/WireMap"
# odbedit -c "load $HOME/AlcapDAQ/analyzer/odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do
    echo Now running $Run
    cp $ODBDataDir/run0${Run}.odb currentRun.odb
    yes n | odbedit -c " load currentRun.odb"
    # odbedit -c " load currentRun.odb"
    echo ""

    $AlcapanaPath/alcapana -i ${FileStub}run0${Run}.mid -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    echo "" >> output.txt
done
