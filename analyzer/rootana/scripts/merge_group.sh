#!/bin/bash

datasets="Al100 Al50b Si16P SiR23pct SiR21pct"

if [ $# -ne 4 ]; then
    echo "Usage: ./merge_group <GROUPSIZE> <PRODUCTIONDB> <ODBDIRECTORY> <ROOTFILEDIRECTORY>"
    exit 0
fi
group_size=$1
db=$2
odbdir=$3
rootdir=$4

echo "File Time Runtime" > times.csv
for d in $datasets; do
    runs=$(sqlite3 $db "SELECT run FROM datasets WHERE dataset=='$d'")
    files=""; m=1; starts=""; stops=""
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
done
