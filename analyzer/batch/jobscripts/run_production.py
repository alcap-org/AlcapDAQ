#!/usr/bin/python
import merlin_utils as mu
import DBManager
import RunManager
import getopt
import time

prod = "alcapana"
ver = 4
nproc = 10
nprep = 5
wait = 2


runman = RunManager.RunManager(prod, ver)
dbman = DBManager.DBManager(prod, ver)

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

        if runman.n_runs == 0:
            print "Nothing for run manager to do. Exiting."
            break

        time.sleep(wait)


    if dbman.IsProductionFinished():
        print "Finishing production", dbman.production_table
        dbman.FinishProduction()


except KeyboardInterrupt:
    runman.Abort()
    mu.abort_jobs(jobs.values())
