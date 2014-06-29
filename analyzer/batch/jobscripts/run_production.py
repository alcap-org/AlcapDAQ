#!/usr/bin/python
import DBManager
import RunManager
import merlin_utils as mu

import optparse
import time


#parser = argparse.ArgumentParser(description="Manages production of AlCap run data among many users.")
#parser.add_argument("--production", type=str, choices=["alcapana","rootana"], help="Production type; either alcapana or rootana", required=True)
#parser.add_argument("--version", type=int, help="The version number to run production on. If not given, assume the most recent production.")
#parser.add_argument("--new", type=str, choices=["alcapana","rootana"], help="Start a new production of a given type. Either alcapana or rootana. Version argument ignored if this is given.", action="store_true")
#parser.add_argument("--pause", type=int, help="Time (in seconds) to wait before checking if there's more to be done.", default=10)
#parser.add_argument("--nproc", type=int, help="Maximum number of jobs to have on the grid at any time.", default=10)
#parser.add_argument("--nprep", type=int, help="Number of files to try and download so that they're ready to be submitted when the number of running jobs falls below NPROC.", default=5)
#args = parser.parse_args()

#prod = args.production
#ver = args.version
#nproc = args.nproc
#nprep = args.nprep
#wait = args.pause


#if args.new:
#    dbm = DBManager.DBManager(prod, ver)
#    dbm.StartProduction()
#    ver = ver + 1
#ver = None

prod = "alcapana"
ver = 4
nproc = 2
nprep = 0
wait = 2
runman = RunManager.RunManager(prod, ver)


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
