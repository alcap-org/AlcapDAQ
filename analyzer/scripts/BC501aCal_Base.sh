#! /bin/bash

Run=$1
RunDir="run_$Run"

RawDataDir="$HOME/raw_rename_VM"
ODBDataDir="$HOME/data/ckampa/AlCap_2015a/odb"
# DataDir="$HOME/data/ckampa/AlCap_2015a/output_EnergyPlots_trun" # 09-12-23 testing existing EnergyPlots module
DataDir="$HOME/data/ckampa/AlCap_2015a/output_EnergyPlots" # 10-06-23 testing keeping more of the pulse in full integral (EnergyPlots module)
mkdir -p $DataDir
AlcapanaPath="$HOME/coding/AlcapDAQ/analyzer/work/BC501a_Calibration"

echo Now running $Run
# mkdir and chng dir -- to allow running many at once
mkdir -p $RunDir
cd $RunDir
FileStub="$RawDataDir/run0${Run}.mid"
echo Input file symlink: ${FileStub}
InFile=$(readlink $FileStub)
echo Input file full path: ${InFile}
cp $ODBDataDir/run0${Run}.odb currentRun.odb
echo $Run
yes n | odbedit -c " load currentRun.odb"
# odbedit -c " load currentRun.odb"
echo ""

echo "Running alcapana..."
echo "$AlcapanaPath/alcapana -i $InFile -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt"
$AlcapanaPath/alcapana -i $InFile -o $DataDir/hist0${Run}.root -T $DataDir/tree0${Run}.root 2>&1 |tee $DataDir/textOutput$Run > output.txt
echo "" >> output.txt
# back out and remove the temp dir
cd ..
rm -rf $RunDir
