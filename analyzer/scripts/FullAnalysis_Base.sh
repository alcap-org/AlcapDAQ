#! /bin/bash

Run=$1
RunDir="run_$Run"

# RawDataDir="$HOME/raw_rename_VM"
# ODBDataDir="$HOME/data/ckampa/AlCap_2015a/odb"
RawDataDir="/data/ckampa/AlCap_2015a/raw_rename"
ODBDataDir="/data/ckampa/AlCap_2015a/odb"
#DataDir="/data/ckampa/AlCap_2015a/output_full_analysis_first" # 08-20-24 adding pulse timing analysis (no ADC to TDC pulse matching)
DataDir="/data/ckampa/AlCap_2015a/output_full_analysis_first_CF" # 07-22-25 CF timing for WFD (everything except Sync pulses). Applied in TTScTCorrWFD and TDCWFDTCorr
mkdir -p $DataDir
#AlcapanaPath="/home/ckampa/coding/AlcapDAQ/analyzer/work/full_analysis_first"
AlcapanaPath="/home/ckampa/coding/AlcapDAQ/analyzer/work/full_analysis_first_CF"

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
