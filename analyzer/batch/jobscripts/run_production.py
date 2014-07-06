#!/usr/bin/python
import DBManager
import RunManager
import merlin_utils as mu

import getopt
import time
import sys

_ALCAPANA = "alcapana"
_ROOTANA  = "rootana"
_PROGRAMS = [_ALCAPANA, _ROOTANA]

def usage():
    print "Usage: ./run_production --production=PROD_TYPE [optional arguments...]"
    print "--------------------------------------------------------------------------------"
    print "This program checks the production database located at"
    print "$HOME/data/production.db and takes care of downloading, submitting,"
    print "and cleanup, and updates the database accordingly so multiple people can process"
    print "at the same time."
    print "--------------------------------------------------------------------------------"
    print "Required:"
    print "    --production=PROD_TYPE"
    print "                    The production type (alcapana or rootana)."
    print "Optional:"
    print "    -h, --help      Print this help then exit."
    print "    --usage         Print this help then exit."
    print "    --version=#     Version number to produce. Default is highest production"
    print "                    version number in production table. If issued with"
    print "                    --new and --production=rootana, this flag tells us which"
    print "                    alcapana version to produce from. If issued with --new and"
    print "                    --production=alcapana, an exception is thrown."
    print "    --new=TAG       Start a new production, incrememted once from the highest"
    print "                    number present in productions table. The software tag in"
    print "                    GitHub must be provided for bookkeeping."
    print "    --pause=#       Wait # seconds between the event loop, which checks for"
    print "                    finished jobs and runs to download."
    print "                    (Default: 5)"
    print "    --nproc=#       Maximum number of processes to submit to the grid"
    print "                    simultaneously."
    print "                    (Default: 10)"
    print "    --nprep==#      Maximum number of MOIDAS files to have downloaded in"
    print "                    addition to the one running on the grid. This means have,"
    print "                    at most, NPROC + NPREP MIDAS files at once."
    print "                    (Default: 5)"
    print "    --spacelim=#    The maximum percentage of available space to use up"
    print "                    (as returned from the mmlsquota command)."
    print "                    (Default: 90)"


################################################################################
# Argument parsing #############################################################
################################################################################
short_args = "h"
long_args = ["usage", "help", "production=", "version=", "new=", "pause=", "nproc=", "nprep=", "spacelim="]
opts, unrecognized = getopt.getopt(sys.argv[1:], short_args, long_args)

# Default arguments
production = None
version = None
new = False
tag = None
wait = 5
nproc = 10
nprep = 5
space_limit = 0.9

# Parse arguments
for opt, val in opts:
    if opt == "h" or opt == "--help" or opt == "--usage":
        usage()
        exit(0)
    elif opt == "--production":
        production = val
    elif opt == "--version":
        version = int(val)
    elif opt == "--new":
        new = True
        tag = str(val)
    elif opt == "--pause":
        wait = int(val)
    elif opt == "--nproc":
        nproc = int(val)
    elif opt == "--nprep":
        nprep = int(val)
    elif opt == "--spacelim":
        space_limit = float(val)


# Argument checking
if len(unrecognized) > 0:
    msg = "Unrecognized arguments: " + unrecognized
    raise ArgumentError(msg)
if len(opts) == 0:
    usage()
    exit(0)
if new and version and production == _ALCAPANA:
    msg = "Version argument given and new alcapana production requested."
    raise ArgumentError(msg)
if new and not version and production == _ROOTANA:
    msg = "Version argument not given and new rootana production requested."
    raise ArgumentError(msg)
if version and (version < 0 or version > 100):
    msg = "Version argument less than zero or geater than 100."
    raise ArgumentError(msg)
if production not in _PROGRAMS:
    msg = "Unknown production type: " + str(production)
    raise ArgumentError(msg)
if wait <= 0 or wait > 60:
    msg = "Requested pause less than or equal to 0 or greater than a minute."
    raise ArgumentError(msg)
if nproc < 1 or nproc > 100:
    msg = "Number of processes shouldn't be less than 1 and maybe shouldn't be more than 100."
    raise ArgumentError(msg)
if nprep < 1 or nprep > 100:
    msg = "Number of preparations shouldn't be less than 1 and maybe shouldn't be more than 100."
    raise ArgumentError(msg)
if space_limit <= 0. or space_limit > 0.95:
    msg = "Space limit cannot be less than or equal to 0 and maybe shouldn't be greater than 95%."
    raise ArgumentError(msg)

# Nothing to prepare for rootana
if production == _ROOTANA:
    nprep = 0
################################################################################



dbman = DBManager.DBManager(production, version)
if new:
    version = dbman.StartNewProduction(tag, "gold", version)
    print "INFO: Starting new production, version " + str(version)
elif not version:
    version = dbman.GetRecentProductionVersionNumber()
    print "INFO: Most recent production version " + str(version)
runman = RunManager.RunManager(production, version)
jobs = {}
try:
    no_disk_space_error_printed = False
    runman.ClaimRuns(nproc + nprep)
    while runman.Busy():
        # Claim runs and download one if we have space. If we don't,
        # print a warning and abort the runs we haven't downloaded yet.
        # If space opens up later, we will continue.
        if mu.fraction_of_quota_used() < space_limit:
            if no_disk_space_error_printed:
                print "INFO: Disk space has become available."
                no_disk_space_error_printed = False
            runman.ClaimRuns(nproc + nprep)
            runman.DownloadOne()
        elif not no_disk_space_error_printed:
            if production == _ALCAPANA:
                print "INFO: There is not enough space. Downloaded runs will continue being processed,"
                print "      however no further runs will be fetched and all undownloaded claimed runs"
                print "      will be aborted."
                print "      This state will be reverted if space opens up as files are deleted"
                print "      during the processing process as they are done processing."
                no_disk_space_error_printed = True
                runman.Abort(runman.to_stage + runman.to_download)
            elif production == _ROOTANA:
                print "ERROR: There is not enough space to do production. Please make space."
                mu.abort_jobs(jobs.values())
                runman.Abort()
                break

        # Submit a job if we can and add the job to the job list
        # to keep track of.
        while len(mu.submitted_jobs()) < nproc:
            sub = runman.SubmitOne()
            if not sub:
                break
            jobs[sub[0]] = sub[1]

        # Update list of run/jobs according to queue status
        # Cancel a job if qstat says it is in error state
        # ("E" is in the status field).
        ## \todo Output the error.
        #  This doesn't happen often though.
        s_jobs = mu.submitted_jobs()
        to_delete = []
        to_abort = {}
        for run, job in jobs.iteritems():
            if job not in s_jobs:
                to_delete.append(run)
                runman.Finish(run)
            else:
                jobs[run] = s_jobs[s_jobs.index(job)]
                if jobs[run].HasError():
                    to_abort[run] = jobs[run]
        if len(to_abort.keys()) > 0:
            print "INFO: There were errors with some runs, so the following runs will be aborted:"
            print to_abort.keys()
            mu.abort_jobs(to_abort.values())
            runman.Abort(to_abort.keys())
        for run in to_delete + to_abort.keys():
            del jobs[run]

        time.sleep(wait)

except KeyboardInterrupt:
    print "User cancelling jobs..."
    mu.abort_jobs(jobs.values())
    runman.Abort()

except:
    print "ERROR: There was an uncaught exception. Aborting..."
    mu.abort_jobs(jobs.values())
    runman.Abort()
    raise



if dbman.IsProductionFinished():
    print "INFO: Finishing production", dbman.production_table
    dbman.FinishProduction()
