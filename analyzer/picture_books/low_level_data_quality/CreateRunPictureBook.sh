#!/bin/bash
# CreateRunPictureBook.sh
# Usage: ./CreateRunPictureBook.sh data_dir run_number

if [ $# -ne 2 ] ; then
	echo "Incorrect Usage!"
	echo "Correct usage: ./CreateRunPictureBook.sh data_dir run_number"
	exit;
fi

data_dir=$1
run_number=$2

mkdir data_quality_figs
echo "void RunPicBookMacros() { GenerateAllPlots(\"$data_dir\", $run_number); dataQualitySumm($run_number); }" > RunPicBookMacros.C

root -l -b -q LoadPicBookMacros.C
pdflatex Data_Quality_Run$run_number
pdflatex Data_Quality_Run$run_number