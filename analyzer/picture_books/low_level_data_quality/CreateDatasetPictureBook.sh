#!/bin/bash
# CreateDatasetPictureBook.sh
# Usage: ./CreateDatasetPictureBook.sh data_dir first_run n_runs

MERLIN_USER=edmonds_a

if [ $# -ne 3 ] ; then
	echo "Incorrect Usage!"
	echo "Correct usage: ./CreateDatasetPictureBook.sh data_dir first_run n_runs"
	exit;
fi


data_dir=$1
first_run=$2
n_runs=$3

dataset=""
if [ $first_run -eq 2091 ] ; then
    dataset="SiR"
elif [ $first_run -eq 2808 ] ; then
    dataset="Al100"
elif [ $first_run -eq 3101 ] ; then
    dataset="Al50a_without-NDet2"
elif [ $first_run -eq 3442 ] ; then
    dataset="Al50a_with-NDet2"
elif [ $first_run -eq 3474 ] ; then
    dataset="Si16P"
elif [ $first_run -eq 3563 ] ; then
    dataset="Al50b"
elif [ $first_run -eq 3763 ] ; then
    dataset="SiR2-3pct"
elif [ $first_run -eq 3771 ] ; then
    dataset="SiR2-1pct"
else
    echo "Invalid first_run for a dataset"
    exit
fi

# If we're on Merlin then generate the figures
if [ "$HOSTNAME" = "merlinl01" ] ; then
    mkdir -p data_quality_figs
    rm data_quality_figs/*.png
    echo "void RunPicBookMacros() { GenerateTrendPlots(\"$data_dir\", $first_run, $n_runs); latexWrapper($first_run, false); }" > RunPicBookMacros.C

    root -l -b -q LoadPicBookMacros.C

# Merlin doesn't have pdflatex so what we need to do is go over there, re-run this script and then come back
else
    ssh -XY $MERLIN_USER@merlinl01.psi.ch "cd AlcapDAQ && . thisdaq.sh && cd analyzer/picture_books/low_level_data_quality && ./CreateDatasetPictureBook.sh ~/data/ $first_run $n_runs"
    scp -r $MERLIN_USER@merlinl01.psi.ch:~/AlcapDAQ/analyzer/picture_books/low_level_data_quality/data_quality_figs .
    scp -r $MERLIN_USER@merlinl01.psi.ch:~/AlcapDAQ/analyzer/picture_books/low_level_data_quality/*.tex .
    
    echo "First passthrough with pdflatex..."
    pdflatex Data_Quality_Run$first_run > output.txt
    echo "Second passthrough with pdflatex..."
    pdflatex Data_Quality_Run$first_run > output.txt
    echo "Done"

    mv Data_Quality_Run$first_run.pdf Data_Quality_Dataset-$dataset.pdf
    mv data_quality_figs/ data_quality_figs_$dataset/
    tar -czf data_quality_figs_$dataset.tar.gz data_quality_figs_$dataset/
fi