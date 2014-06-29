#!/usr/bin/python
import DBManager
import RunManager
import merlin_utils as mu

import getopt
import time

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
    print "                   The production type (alcapana or rootana)."
    print "Optional:"
    print "    -h, --help     Print this help then exit."
    print "    --usage        Print this help then exit."
    print "    --version=#    Version number to produce. Default is highest"
    print "                   production version number in production table."
    print "    --new          Start a new production, incrememted once from the"
    print "                   highest number present in productions table."
    print "    --pause=#      Wait # seconds between the event loop, which"
    print "                   checks for finished jobs and runs to download."
    print "                   (Default: 10)"
    print "    --nproc=#      Maximum number of processes to submit"
    print "                   to the grid simultaneously."
    print "                   (Default: 10)"
    print "    --nprep==#     Maximum number of MOIDAS files to have downloaded"
    print "                   in addition to the one running on the grid. This"
    print "                   means have, at most, NPROC + NPREP MIDAS files at"
    print "                   once."
    print "                   (Default: 5)"


################################################################################
# Argument parsing #############################################################
################################################################################
short_args = "h"
long_args = ["usage", "help", "production=", "version=", "new", "pause=", "nproc=", "nprep="]
opts, unrecognized = getopt.getopt(sys.argv[1:], short_args, long_args)

# Default arguments
production = None
version = None
new = False
wait = 10
nproc = 10
nprep = 5

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
    elif opt == "--pause":
        wait = int(val)
    elif opt == "--nproc":
        nproc = int(val)
    elif opt == "--nprep":
        nprep = int(val)


# Argument checking
if len(unrecognized) > 0:
    msg = "Unrecognized arguments: " + unrecognized
    raise ArgumentError(msg)
if new and version:
    msg = "Version argument given and new production requested."
    raise ArgumentError(msg)
if version and (version < 0 or version > 100):
    msg = "Version argument less than zero or geater than 100."
    raise ArgumentError(msg)
if production not in ["alcapana", "rootana"]:
    msg = "Unknown production type: " + str(production)
    raise ArgumentError(msg)
# Remove this when rootana implemented
if production == "rootana":
    msg = "Rootana not implemented yet."
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
################################################################################


if new:
    dbm = DBManager.DBManager(production, version)
    version = dbm.StartNewProduction()
    print "Starting new production, version " + str(version) + "..."
elif not version:
    dbm = DBManager.DBManager(production, version)
    version = dbm.GetRecentProductionVersionNumber()
    print "Most recent production version " + str(version) + "..."


runman = RunManager.RunManager(production, version)
jobs = {}
try:
    while True:
        # If we don't have enough space, just finish what we have
        if mu.fraction_of_quota_used() < 0.9:
            runman.ClaimRuns(nproc + nprep)
            runman.DownloadOne()

        while len(mu.submitted_jobs()) < nproc:
            sub = runman.SubmitOne()
            if not sub:
                break
            jobs[sub[0]] = sub[1]

        s_jobs = mu.submitted_jobs()
        to_delete = []
        for run, job in jobs.iteritems():
            if job not in s_jobs:
                to_delete.append(run)
                runman.Finish(run)
        for run in to_delete:
            del jobs[run]

        if len(runman.to_finish + runman.to_submit) == 0:
            print "Nothing for run manager to do. Exiting."
            break

        time.sleep(wait)


    if runman.dbm.IsProductionFinished():
        print "Finishing production", runman.dbm.production_table
        runman.dbm.FinishProduction()


except KeyboardInterrupt:
    runman.Abort()
    mu.abort_jobs(jobs.values())
