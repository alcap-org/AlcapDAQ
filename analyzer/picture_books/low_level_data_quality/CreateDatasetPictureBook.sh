#!/bin/bash
# CreateDatasetPictureBook.sh
# Usage: ./CreateDatasetPictureBook.sh data_dir first_run n_runs

if [ $# -ne 3 ] ; then
	echo "Incorrect Usage!"
	echo "Correct usage: ./CreateDatasetPictureBook.sh first_run n_runs"
	exit;
fi

data_dir=$1
first_run=$2
n_runs=$3

dataset=""
if [ $first_run -eq 2091 ] ; then
    dataset="SiR"
elif [$first_run -eq 2808 ] ; then
    dataset="Al100"
elif [ $first_run -eq 3101 ] ; then
    dataset="Al50a"
elif [ $first_run -eq 3474 ] ; then
    dataset="Si16P"
elif [ $first_run -eq 3563 ] ; then
    dataset="Al50b"
elif [ $first_run -eq 3763 ] ; then
    dataset-"SiR2-3pct"
elif [ $first_run -eq 3771 ] ; then
    dataset-"SiR2-1pct"
else
    echo "Invalid first_run for a dataset"
    exit
fi


mkdir -p data_quality_figs
rm data_quality_figs/*.png
echo "void RunPicBookMacros() { GenerateTrendPlots(\"$data_dir\", $first_run, $n_runs); dataQualitySumm($first_run); }" > RunPicBookMacros.C

root -l -b -q LoadPicBookMacros.C
pdflatex Data_Quality_Run$first_run > output.txt
pdflatex Data_Quality_Run$first_run > output.txt

mv Data_Quality_Run$first_run.pdf Data_Quality_Dataset-$dataset.pdf