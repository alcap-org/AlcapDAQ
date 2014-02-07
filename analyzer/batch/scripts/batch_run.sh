#!/bin/bash

# Get run list
if [ $# -eq 1 ]; then
    RUNS=$1
elif [ $# -eq 2 ]; then
    RUNS=({$1..$2})
else
    echo "ERROR: Should only be 2 arguments!"
    exit 1;
fi

# Setup environment
if [ -z "$DAQdir" ]; then
    echo "ERROR: DAQdir not set!"
    exit 2
fi
. $DAQdir/thisdaq.sh
TMP="$HOME/tmp/batch/alcapana/run$RUN"
MIDAS_DIR="$TMP/ODB"

#############################################
# Export the environment variables we'll need
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
ODB_DEFAULT="$DAQdir/analyzer/odb/Default.odb"

CMD="$DAQdir/analyzer/batch/scripts/batch_run.sge"

#################################
# Check that the data files exist
if [ ! -f $ODB ]; then
    echo "ERROR: ODB file not found!"
    exit 3
elif [ ! -f $RAW ]; then
    echo "ERROR: Raw data not found!"
    exit 4
fi

###################################
# Make directories that don't exist
mkdir -p $TMP
mkdir -p $HISTDATADIR
mkdir -p $TREEDATADIR
mkdir -p $LOGDIR

for RUN in ${RUNS[@]}; do
    ODB="$ODBDATADIR/run$RUN.odb"
    RAW="$RAWDATADIR/run$RUN.mid"
    HIST="$HISTDATADIR/hist$RUN.mid"
    TREE="TREEDATADIR/tree$RUN.mid"
    OLOG="$LOGDIR/alcapana.run$RUN.out"
    ELOG="$LOGDIR/alcapana.run$RUN.err"
    
    yes n | odbedit -c "load $ODB_DEFAULT"
    yes n | odbedit -c "load $ODB"

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

    echo "$RUN $ODB $RAW $HIST $HIST $TREE $OLOG $ELOG"
#    qsub -N alcapana$RUN -v MIDAS_DIR -v PATH -v LD_LIBRARY_PATH $CMD -- 
done
