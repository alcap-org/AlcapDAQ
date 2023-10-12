#! /bin/bash

# Al2mm Y
# RunNumbers=( 6892 )
# Cs137 C -- None
# NdetU
# RunNumbers=( 7196 )
# NdetD
#RunNumbers=( 7197 )
# AmBe C
# NdetU 7194, 7195
# NdetD 7192, 7193
# RunNumbers=( 7192 7193 7194 7195 )
#RunNumbers=( 7192 )

# ALL CALIBRATIONS (NdetD and NdetU)
# key: (AmBeD, AmBeD, AmBeU, AmBeU, CsU, CsD, NaU, NaD, YU, YD, BiU, BiD, BKG)
RunNumbers=( 7192 7193 7194 7195 7196 7197 7198 7199 7200 7201 7202 7203 7204 )

# RawDataDir="$HOME/data/ckampa/AlCap_2015a/raw"
# RawDataDir="$HOME/data/ckampa/AlCap_2015a/raw_rename"
RawDataDir="$HOME/raw_rename_VM"
ODBDataDir="$HOME/data/ckampa/AlCap_2015a/odb"
# DataDir="$HOME/data/ckampa/AlCap_2015a/output_alcapana"
DataDir="$HOME/data/ckampa/AlCap_2015a/output_EnergyPlots" # 09-12-23 testing existing EnergyPlots module
# FIXME! This will not work when looking at data from other datasets.
# FileStub="$RawDataDir/raw.rhbob.AlCapR2015a.Al2mm_36MeV_Y."
# FileStub="$RawDataDir/raw.rhbob.AlCapR2015a.None_36MeV_C."
# FileStub="$RawDataDir/raw.rhbob.AlCapR2015a.AmBe_36MeV_C."
# FileStub="$RawDataDir/"
# FileStub="$RawDataDir/run0${Run}.mid"
# echo Input file symlink: ${FileStub}
# # variable InFile=$(readlink $FileStub)
# InFile="$(readlink $FileStub)"
# # InFile=`readlink $FileStub`
# echo Input file full path: ${InFile}
mkdir -p $DataDir
AlcapanaPath="$HOME/coding/AlcapDAQ/analyzer/work/BC501a_Calibration"

# odbedit -c "rm Analyzer/WireMap"
# odbedit -c "load $HOME/AlcapDAQ/analyzer/odb/Defaults.odb"

for Run in ${RunNumbers[@]}; do
    echo Now running $Run
    FileStub="$RawDataDir/run0${Run}.mid"
    echo Input file symlink: ${FileStub}
    # # variable InFile=$(readlink $FileStub)
    InFile=$(readlink $FileStub)
    # InFile=$(realpath $FileStub)
    # # InFile=`readlink $FileStub`
    echo Input file full path: ${InFile}
    cp $ODBDataDir/run0${Run}.odb currentRun.odb
    echo $Run
    yes n | odbedit -c " load currentRun.odb"
    # odbedit -c " load currentRun.odb"
    echo ""

    echo "Running alcapana..."
    echo "$AlcapanaPath/alcapana -i $InFile -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt"
    # $AlcapanaPath/alcapana -i ${FileStub}run0${Run}.mid -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    $AlcapanaPath/alcapana -i $InFile -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
    echo "" >> output.txt
done
