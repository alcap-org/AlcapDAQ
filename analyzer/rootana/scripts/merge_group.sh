#!/bin/bash

datasets="Al100 Al50b Si16P SiR23pct SiR21pct Al50awithNDet2 Al50awithoutNDet2"

if [ $# -ne 5 ]; then
    echo "Usage: ./merge_group <GROUPSIZE> <PRODUCTIONDB> <CALIBRATIONDB> <ODBDIRECTORY> <ROOTFILEDIRECTORY>"
    exit 0
fi
group_size=$1
proddb=$2
calibdb=$3
odbdir=$4
rootdir=$5
mergedb="merge.db"

# Prepare files
echo "File Time Runtime Pedestal Noise" > times.csv
rm -f $mergedb
sqlite3 $mergedb "CREATE TABLE PedestalAndNoise(file TEXT, channel TEXT, 'pedestal' REAL, 'noise' REAL)"
sqlite3 $mergedb "CREATE TABLE Merge(file TEXT, runs TEXT, time REAL, runtime REAL)"
for d in $datasets; do
    rm -f $d"_"*.root
    runs=$(sqlite3 $proddb "SELECT run FROM datasets WHERE dataset=='$d' ORDER BY run ASC")
    files=""; m=1; starts=""; stops=""; rs="";
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
	    files="$files $rootdir/out0$r.root"
	    rs="$rs $r"
	else
	    echo "Cannot find a file for run $r (ROOT: $ifile, ODB: $odbfile)"
	fi
	# If we have enough files, hadd their histograms and calculate time and weight
	if (($(echo $files | wc -w)>=group_size)); then
	    # Calculate average weighted time of run and the total runtime (weight)
	    tw=$(python -c "import numpy; ts=[sum(it)/2. for it in zip([$starts], [$stops])]; ws=numpy.subtract([$stops], [$starts]); print numpy.average(ts,weights=ws), sum(ws);")
	    rs=${rs:1}; f=$d"_"$m; t=$(echo $tw | cut -d' ' -f1); w=$(echo $tw | cut -d' ' -f2);
	    hadd -f -T $f.root $files
	    sqlite3 $mergedb "INSERT INTO Merge VALUES('$f', '$rs', $t, $w)"
	    # Make calib db entry
	    for det in $(sqlite3 $calibdb "SELECT DISTINCT channel FROM PedestalAndNoise"); do
		pn=$(sqlite3 $calibdb "SELECT AVG(pedestal),AVG(noise) FROM PedestalAndNoise WHERE channel=='$det' AND run IN (${rs// /,})")
		if [ -z ${pn/|/} ]; then continue; fi;
		p=$(echo $pn | cut -d"|" -f1); n=$(echo $pn | cut -d"|" -f2);
		sqlite3 $mergedb "INSERT INTO PedestalAndNoise VALUES('$f', '$det', $p, $n)"
	    done
	    files=""; starts=""; stops=""; rs=""
	    ((m++))
	fi
    done
    # Add up any that are left
    if [ -n "$files" ]; then
	tw=$(python -c "import numpy; ts=[sum(it)/2. for it in zip([$starts], [$stops])]; ws=numpy.subtract([$stops], [$starts]); print numpy.average(ts,weights=ws), sum(ws);")
	rs=${rs:1}; f=$d"_"$m; t=$(echo $tw | cut -d' ' -f1); w=$(echo $tw | cut -d' ' -f2);
	hadd -f -T $f.root $files
	sqlite3 $mergedb "INSERT INTO Merge VALUES('$f', '$rs', $t, $w)"
	for det in $(sqlite3 $calibdb "SELECT DISTINCT channel FROM PedestalAndNoise"); do
	    pn=$(sqlite3 $calibdb "SELECT AVG(pedestal),AVG(noise) FROM PedestalAndNoise WHERE channel=='$det' AND run IN (${rs// /,})")
	    if [ -z ${pn/|/} ]; then continue; fi;
	    p=$(echo $pn | cut -d"|" -f1); n=$(echo $pn | cut -d"|" -f2);
	    sqlite3 $mergedb "INSERT INTO PedestalAndNoise VALUES('$f', '$det', $p, $n)"
	done
    fi
    # Make one big one
    hadd -f -T $d.root $d"_"*.root
done
