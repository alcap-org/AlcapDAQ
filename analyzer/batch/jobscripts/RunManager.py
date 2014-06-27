import merlin_utils as mu
import DBManager

## \brief
#  Keeps track of runs and where they are in the
#  process of staging, downloading, submitting,
#  and finishing. Moves them through this process.
#
#  \details
#  The productiond database is checked and available
#  runs are claimed and added to the manager. A run
#  is considered "managed" if it has been claimed
#  and has not finished processing yet. A managed run
#  is first claimed from the database and then added to
#  the list of files to be staged on the remote tape
#  archiv. The run is promoted to the download queue
#  from there and, once downloaded, promoted to the
#  grid submit queue. Once the job has been submitted
#  to the queue for processing, the status (using the
#  output of qstat) is watched until it disappears from
#  the list of running jobs. The run is considered done
#  processing at that point (even if there was an error)
#  is is moved to the finish queue, where the database
#  updated to reflect this. The run is them removed from
#  the run manager, making room for more runs.
class RunManager:
    ## \brief
    #  Needs to be declared with a production type and version number.
    #
    #  \details
    #  A RunManager is attached to a specific production run.
    #  Specifically, a production type and version number.
    #  All of the database checks and updates are done from
    #  here on out on the tables prod_vver (example:
    #  alcapana_v4).
    #
    #  \param[in] prog A string indicating the production type
    #  ("alcapana" or "rootana").
    #  \param[in] ver An integer representing the version number.
    def __init__(self, prod, ver):
        self.prod = prod
        self.ver = ver
        self.dbm = DBManager.DBManager(prod, ver)
        self.n_runs = 0
        self.n_downloaded = 0
        self.n_running = 0
        self.to_stage = []
        self.to_download = []
        self.to_submit = []
        self.to_finish = []

    ## \brief
    #  Claims some number of runs from the database
    #  up intil some maximum number.
    #
    #  \param[in] max_num_runs Maximum number of runs to
    #  manage at a time
    def ClaimRuns(self, max_num_runs):
        while self.n_runs < max_num_runs:
            run = dbm.ClaimAnyAvailableRun()
            if not run:
                return
            self.n_runs = self.n_runs + 1
            self.to_stage.append(run)
            print "Claimed run:", run
        
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
    #  Submit a single job if there is a job to submit, and register
    #  this with the database.
    #
    #  \return A list with two objects; the first is the run number of the
    #  submitted job (int), the second is an SGEJob for following the
    #  job on the grid.
    def SubmitOne(self):
        # We use a for loop in case there
        # are no runs to be submitted.
        for run in self.to_submit[0:1]:
            print "Submitting run:", run
            job = mu.submit_job(run, self.prod)
            self.to_finish.append(self.to_submit.pop(0))
            dbm.RegisterRunStart(run)
            return [run, job]

    ## \brief
    #  When a run has been complete, remove its MIDAS file and register
    #  it with the database.
    #
    #  \param[in] run The run number that has completed on the grid (int)
    def Finish(self, run):
        self.n_runs = self.n_runs - 1
        self.n_downloaded = self.n_downloaded - 1
        mu.remove_midas_file(run)
        dbm.RegisterRunFinish(run)
        srun = "%05d" % run
        dbm.registerFileLocation(run, mu.TREEdir + "/tree" + srun + ".root", "tree")
        dbm.registerFileLocation(run, mu.HISTdir + "/hist" + srun + ".root", "hist")
        dbm.RegisterFileLocation(run, mu.DUMPdir + "/dump" + srun + ".odb", "dump")
        print "Finished run:", run
        self.to_finish.remove(run)
