#!/bin/bash

usage() {
    echo "usage: batch_alcapana.sh [-h | --help] [--usage] [options] [runs...]"
}

help() {
    usage
    echo "At least one run must be specified by either a range (-r) of runs"
    echo "(inclusive) or in a space seperated list. Multiple ranges and"
    echo "runs can be specified."
    echo
    echo "Options:"
    echo "-n    The maximum number of jobs to submit to the queue at a time."
    echo "-r    Space separated range of runs to process. Multiple ranges can be specified,"
    echo "      they must all be preceded by their own -r."
    echo "-t    Amount of time in seconds between checks to see if more jobs can be submittied."
    echo "      Default is 30, there is no real motivation for this other than the concern"
    echo "      of using up Merlin login node resources."
    echo "-p    FTP password to download raw data."
    echo
    echo "Example:"
    echo "\$ ./batch_alcapana.sh -r 1 6 9 -n 5 125 -r 367 369"
    echo "This will process runs 1-6, 9, 125, and 367-369, and up to 5"
    echo "at a time."
}

runcanon() {
    if [ $1 -lt 10 ]; then
	echo 0000$1
    elif [ $1 -lt 100 ]; then
	echo 000$1
    elif [ $1 -lt 1000 ]; then
	echo 00$1
    elif [ $1 -lt 10000 ]; then
	echo 0$1
    fi
}

runfilecanon() {
    x=""
    echo "run$(runcanon $1).mid"
}

flagcanon() {
    echo "alcapana.run$(runcanon $1).flag"
}

flagadd() {
    x=""
    while [ $# -gt 0 ]; do
	x="$x $1"
	shift
    done
    echo "$x"
}

flagrm() {
    rx="$1"
    shift
    x=""
    y=""
    while [ $# -gt 0 ]; do
	x="$x $1"
	shift
    done
    for ix in $x; do
	if [ ! $ix = $rx ]; then
	    y="$y $ix"
	fi
    done
    echo $y
}

flagdone() {
    if [ -f "$1/$(flagcanon $2)" ]; then
	echo ""
    else
	echo "done"
    fi
}

flagcount() {
    n=0
    x=$(ls $1/alcapana.*.flag 2> /dev/null)
    for ix in $x; do
	((n += 1))
    done
    echo $n
}

RUNS=""
NJOBS=1
DT=30

FTPUSER="mucap"
FTPPSWD=""
FTPSRVR="ftp://archivftp.psi.ch"
FTPDIR="mu2e/run2013"

# Get command line arguments
while [ $# -gt 0 ]; do
    if [ $1 = "-h" -o $1 = "--help" ]; then
	help
	exit 0
    elif [ $1 = "--usage" ]; then
	usage
	exit 0
    elif [ $1 = "-r" ]; then
	if [ ! $# -ge 3 ]; then
	    usage
	    echo "ERROR: Range option (-r) requires two space separate arguments!"
	    exit 1
	fi
	RUNS="$RUNS $(seq $2 $3)"
	shift 3
    elif [ $1 = "-n" ]; then
	NJOBS=$2
	shift 2
    elif [ $1 = "-t" ]; then
	DT="$2"
	shift 2
    elif [ $1 = "-p" ]; then
	FTPPSWD="$2"
	shift 2
    else
	RUNS="$RUNS $((10#$1))"
	shift
    fi
done

# Get FTP password if not passed at command line
if [ -z "$FTPPSWD" ]; then
    read -p "FTP Password: " FTPPSWD
fi

# Check there are no duplicates and that these are all numbers.
for IRUN in $RUNS; do
    if [ $IRUN -eq 0 ]; then
	echo "ERROR: Are all arguments nonzero numbers?"
	exit 2;
    fi
    NRUN=0
    for JRUN in $RUNS; do
	((JRUN = JRUN))
	if [ $IRUN = $JRUN ]; then
	    ((NRUN += 1))
	fi
    done
    if [ $NRUN -ne 1 ]; then
	echo "ERROR: Run $IRUN is repeated ($NRUN)!"
	exit 3
    fi
done

# Check the DAQdir is set
if [ -z "$DAQdir" ]; then
    echo "ERROR: Environment variable DAQdir must be set."
    exit 4
fi
export DAQdir

# Setup some environment variables
LOGDIR="$DAQdir/analyzer/batch/log"
FLGDIR="$DAQdir/analyzer/batch/tmp/flag"
DATADIR="$HOME/data"
RAWDIR="$DATADIR/raw"
TREEDIR="$DATADIR/tree"
HISTDIR="$DATADIR/hist"

mkdir -p $LOGDIR
mkdir -p $FLGDIR
mkdir -p $RAWDIR

# Check other instances of this aren't running
if [ $(flagcount $FLGDIR) -ne 0 ]; then
    echo "ERROR: Seem to be some flags in directory ($FLGDIR). Are there other instances of this script running?"
    echo "Remove to continue."
    exit 5
fi

# Submit jobs
FLAGS=""
CMD="$DAQdir/analyzer/batch/scripts/batch_alcapana.sge"
for IRUN in $RUNS; do

    OLOG="$LOGDIR/alcapana.run$(runcanon $IRUN).out"
    ELOG="$LOGDIR/alcapana.run$(runcanon $IRUN).err"

    while [ $(flagcount $FLGDIR) -ge $NJOBS ]; do
	sleep $DT
    done
    for IFLAG in $FLAGS; do
	if [ "$(flagdone $FLGDIR $IFLAG)" ]; then
	    echo "Finished run $IFLAG!"
	    FLAGS="$(flagrm $IFLAG $FLAGS)"
	    rm -f "$RAWDIR/$(runfilecanon $IFLAG)"
	fi
    done

    if [ -f "$TREEDIR/tree$(runcanon $IRUN).root" -o -f "$HISTDIR/hist$(runcanon $IRUN).root" ]; then
	echo "WARNING: It seems a tree or hist file for run $IRUN exists!"
	continue
    fi
    
    rm -f $OLOG
    rm -f $ELOG

    echo "Downloading run $IRUN..."
    wget -c --user=$FTPUSER --password=$FTPPSWD $FTPSRVR/$FTPDIR/$(runfilecanon $IRUN)
    if [ -f "$(runfilecanon $IRUN)" ]; then
	mv "$(runfilecanon $IRUN)" "$RAWDIR/"
    else
	continue
    fi

    touch $FLGDIR/$(flagcanon $IRUN)
    FLAGS=$(flagadd $FLAGS $IRUN)
    qsub -v DAQdir -e $ELOG -o $OLOG $CMD $IRUN

done

while [ $(flagcount $FLGDIR) -gt 0 ]; do
    for IFLAG in $FLAGS; do
	if [ "$(flagdone $FLGDIR $IFLAG)" ]; then
	    echo "Finished run $IFLAG!"
	    FLAGS="$(flagrm $IFLAG $FLAGS)"
	    rm -f "$RAWDIR/$(runfilecanon $IFLAG)"
	fi
    done
    sleep $DT
done

rm -f $RAWDIR/run*.mid
