#!/bin/bash

datasets="Al100 Al50b Si16P SiR23pct SiR21pct"

if [ $# -ne 5 ]; then
    echo "Usage: ./merge_group <GROUPSIZE> <PRODUCTIONDB> <CALIBRATIONDB> <ODBDIRECTORY> <ROOTFILEDIRECTORY>"
    exit 0
fi
group_size=$1
proddb=$2
calibdb=$3
odbdir=$4
rootdir=$5

echo "File Time Runtime Pedestal Noise" > times.csv
for d in $datasets; do
    rm -f $d"_"*.root
    runs=$(sqlite3 $proddb "SELECT run FROM datasets WHERE dataset=='$d' ORDER BY run ASC")
    files=""; m=1; starts=""; stops=""; peds=""; noises="";
    for r in $runs; do
	ifile="$rootdir/out0$r.root"
	odbfile="$odbdir/run0$r.odb"
	# Start agregating files
	if [ -f "$ifile" -a -f "$odbfile" ]; then
	    yes n | odbedit -c "load $odbfile" > /dev/null
	    start=$(odbedit -c "ls -v '/Runinfo/Start time binary'")
	    stop=$(odbedit -c "ls -v '/Runinfo/Stop time binary'")
	    starts="$starts $start,"
	    stops="$stops $stop,"
	    files="$files out0$r.root"
	else
	    echo "Cannot find a file for run $r (ROOT: $ifile, ODB: $odbfile)"
	fi
	# If we have enough files, hadd their histograms and calculate time and weight
	if (($(echo $files | wc -w)>=group_size)); then
	    f=$d"_"$m
	    hadd -f -T $f.root $files
	    t=$(python -c "import numpy; ts=[sum(it)/2. for it in zip([$starts], [$stops])]; ws=numpy.subtract([$stops], [$starts]); print numpy.average(ts,weights=ws), sum(ws);")
	    echo "$f $t" >> times.csv
	    files=""; starts=""; stops="";
	    ((m++))
	fi
    done
    # Add up any that are left
    if [ -n "$files" ]; then
	f=$d"_"$m
	hadd -f -T $f.root $files
	t=$(python -c "import numpy; ts=[sum(it)/2. for it in zip([$starts], [$stops])]; ws=numpy.subtract([$stops], [$starts]); print numpy.average(ts,weights=ws), sum(ws);")
	echo "$f $t" >> times.csv
    fi
    # Make one big one
    hadd -f -T $d.root $d"_"*.root
done
