#!/bin/bash

# Get run list
if [ $# -eq 1 ]; then
    RUNS=$1
elif [ $# -eq 2 ]; then
    RUNS=$(seq $1 $2)
else
    echo "ERROR: Should only be 1 or 2 arguments!"
    exit 1;
fi

# Setup environment
if [ -z "$DAQdir" ]; then
    echo "ERROR: DAQdir not set!"
    exit 2
fi
. $DAQdir/thisdaq.sh

#############################################
# Export the environment variables we'll need
export DAQdir
export MIDAS_DIR
export PATH
export LD_LIBRARY_PATH

# Variables we'll be using
DATADIR="$HOME/data"
ODBDATADIR="$DATADIR/odb"
RAWDATADIR="$DATADIR/raw"
HISTDATADIR="$DATADIR/hist"
TREEDATADIR="$DATADIR/tree"
LOGDIR="$DAQdir/analyzer/batch/log"

# Default ODBs to load
ODBDIR="$DAQdir/analyzer/odb"
ODB_DEFAULTS="master.odb New_Analyzer_WireMap_10-Dec-2013.odb Analyzer_WireMap_TimeShift.odb AddADCCalib.odb Defaults.odb"

CMD="$DAQdir/analyzer/batch/scripts/batch_run.sge"

#################################
# Check that the raw data exists
if [ ! -f $RAW ]; then
    echo "ERROR: Raw data not found!"
    exit 3
fi

###################################
# Make directories that don't exist
mkdir -p $HISTDATADIR
mkdir -p $TREEDATADIR
mkdir -p $LOGDIR

for RUN in $RUNS; do
    # Canonicalize run numbers
    if [ $RUN -lt 10 ]; then
	RUN=0000$RUN
    elif [ $RUN -lt 100 ]; then
	RUN=000$RUN
    elif [ $RUN -lt 1000 ]; then
	RUN=00$RUN
    elif [ $RUN -lt 10000 ]; then
	RUN=0$RUN
    fi

    # Declare outputs
    ODB="$ODBDATADIR/run$RUN.odb"
    RAW="$RAWDATADIR/run$RUN.mid"
    HIST="$HISTDATADIR/hist$RUN.root"
    TREE="$TREEDATADIR/tree$RUN.root"
    OLOG="$LOGDIR/alcapana.run$RUN.out"
    ELOG="$LOGDIR/alcapana.run$RUN.err"
    
    # Remove previous logs
    rm -f $OLOG
    rm -f $ELOG

    # Prepare temporary ODB
    TMP="$HOME/tmp/alcapana/run$RUN"
    MIDAS_DIR="$TMP/ODB"
    mkdir -p $TMP
    mkdir -p $MIDAS_DIR

    for I_ODB in $ODB_DEFAULTS; do
	I_ODB=$ODBDIR/$I_ODB
	if [ -f $I_ODB ]; then
	    yes n | odbedit -c "load $I_ODB"
	    echo
	else
	    echo "WARNING: $I_ODB not found..."
	fi
    done
    if [ -f $ODB ]; then
	yes n | odbedit -c "load $ODB"
	echo
    else
	echo "WARNING: ODB file not found for run $RUN, loading from runfile..."
	odbedit -c "set '/Analyzer/ODB Load' y"
    fi

    echo $RUN $CMD $RAW $HIST $TREE
    qsub -N alcapana$RUN -v DAQdir,MIDAS_DIR,PATH,LD_LIBRARY_PATH -e $ELOG -o $OLOG $CMD $RAW $HIST $TREE

done
