#!/usr/bin/python
import merlin_utils as mu
import database_utils as dbu
import getopt
import time

prod = "alcapana"
ver = 6
nproc = 10
nprep = 5
wait = 2

## \brief
#  Keeps track of runs and where they are in the
#  process of staging, downloading, submitting,
#  and finishing. Moves them through this process.
class RunManager:
    def __init__(self):
        self.n_runs = 0
        self.n_downloaded = 0
        self.n_running = 0
        self.to_stage = []
        self.to_download = []
        self.to_submit = []
        self.to_finish = []

    def AddRun(self, run):
        self.n_runs = self.n_runs + 1
        self.to_stage.append(run)
        print "Claimed run:", run
        return len(self.to_stage + self.to_download + self.to_submit + self.to_finish)

    def DownloadOne(self):
        if len(self.to_stage) > 0 and len(self.to_download) > 0:
            print "Staging runs:", self.to_stage
            print "Downloading run:", self.to_download[0]
            print "...",
            if mu.stage_files_and_get_others(self.to_stage, self.to_download[0:1]) == 0:
                self.to_submit.append(self.to_download.pop(0))
                self.to_download = self.to_download + self.to_stage
                self.to_stage = []
                self.n_downloaded = self.n_downloaded + 1
                print "Success!"
            else:
                print "Failed. Will try again later."
        elif len(self.to_stage) > 0:
            print "Staging runs:", self.to_stage
            print "Downloading run:", self.to_stage[0]
            print "...",
            if mu.stage_files_and_get_others(self.to_stage, self.to_stage[0:1]) == 0:
                self.to_submit.append(self.to_stage.pop(0))
                self.to_download = self.to_stage
                self.to_stage = []
                self.n_downloaded = self.n_downloaded + 1
                print "Success!"
            else:
                print "Failed. Will try again later."
        elif len(self.to_download) > 0:
            print "Downloading run:", self.to_download[0]
            print "...",
            if mu.stage_files_and_get_others([], self.to_download[0:1]) == 0:
                self.to_submit.append(self.to_download.pop(0))
                self.n_downloaded = self.n_downloaded + 1
                print "Success!"
            else:
                print "Failed. Will try again later."
                
    ## \brief
    #  Submit a single job if there is
    #  a job to submit
    def SubmitOne(self, prog, ver):
        # We use a for loop in case there
        # are no runs to be submitted.
        for run in self.to_submit[0:1]:
            print "Submitting run:", run
            job = mu.submit_job(run, prog)
            self.to_finish.append(self.to_submit.pop(0))
            dbu.register_run_start(prog, ver, run)
            return [run, job]

    ## \brief
    #  When a run has been complete
    def Finish(self, run, prog, ver):
        self.n_runs = self.n_runs - 1
        self.n_downloaded = self.n_downloaded - 1
        mu.remove_midas_file(run)
        dbu.register_run_finish(prog, ver, run)
        srun = ("%05d" % run)
        dbu.register_file_location(prog, ver, run, mu.TREEdir + "/tree" + srun + ".root", "tree")
        dbu.register_file_location(prog, ver, run, mu.HISTdir + "/hist" + srun + ".root", "hist")
        dbu.register_file_location(prog, ver, run, mu.DUMPdir + "/dump" + srun + ".odb", "dump")
        print "Finished run:", run
        self.to_finish.remove(run)

runman = RunManager()

jobs = {}
while True:
    while runman.n_runs < nproc + nprep:
        run = dbu.claim_any_available_run(prod, ver)
        if not run:
            break
        runman.AddRun(run)

    if mu.fraction_of_quota_used() < 0.9 and runman.n_downloaded < nproc + nprep:
        runman.DownloadOne()

    while len(mu.submitted_jobs()) < nproc:
        sub = runman.SubmitOne(prod, ver)
        if not sub:
            break
        jobs[sub[0]] = sub[1]

    s_jobs = mu.submitted_jobs()
    to_delete = []
    for run, job in jobs.iteritems():
        if job not in s_jobs:
            to_delete.append(run)
            runman.Finish(run, prod, ver)
    for run in to_delete:
        del jobs[run]

    if runman.n_runs == 0:
        print "Nothing for run manager to do. Exiting."
        break

    time.sleep(wait)

if dbu.is_production_finished(prod, ver):
    print "Finishing production", dbu.production_table_name(prod, ver)
    dbu.finish_production(prod, ver)
