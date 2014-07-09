import merlin_utils as mu
import DBManager
import SGEJob
from AlCapExceptions import *

import os

_ALCAPANA = "alcapana"
_ROOTANA  = "rootana"
_PROGRAMS = [_ALCAPANA, _ROOTANA]

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
        if prod not in _PROGRAMS:
            raise UnknownProductionError(prod)
        self.prod = prod
        self.ver = ver
        self.dbm = DBManager.DBManager(prod, ver)
        self.n_runs = 0
        self.n_downloaded = 0
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
            run = self.dbm.ClaimAnyAvailableRun()
            if not run:
                return
            self.n_runs = self.n_runs + 1
            if self.prod == _ALCAPANA:
                tree = mu.TREEdir + "/tree%05d.root" % run
                hist = mu.HISTdir + "/hist%05d.root" % run
                for path in [tree, hist]:
                    if os.path.lexists(path):
                        if os.path.islink(path):
                            os.remove(path)
                        else:
                            msg = "Old file exists: " + str(path)
                            raise AlCapError(msg)
                self.to_stage.append(run)
            elif self.prod == _ROOTANA:
                out = mu.OUTdir + "/out%05d.root" % run
                if os.path.lexists(out):
                    if os.path.islink(out):
                        os.remove(out)
                    else:
                        msg = "Old file exists: " + str(out)
                        raise AlCapError(msg)
                self.to_submit.append(run)
            print "Claimed run:", run
        return

    ## \brief
    #  Stage any runs not yet staged and download a single run.
    #
    #  \details
    #  First, any runs in the queue to be staged are staged from
    #  the arhive's tapes. Then, if a run is in the queue to be
    #  downloaded, a download is requested. If not, one of the runs
    #  who just had a staging request made for it is downloaded
    #  (this will take some time as the MIDAS run file has not been
    #  staged prior).
    def DownloadOne(self):
        if self.prod == _ROOTANA:
            return
        if len(self.to_download) > 0:
            staged = True
            dl = self.to_download[0]
        elif len(self.to_stage) > 0:
            staged = False
            dl = self.to_stage[0]
        else:
            return
        if self.to_stage:
            print "Staging runs:", self.to_stage
        print "Downloading run:", dl
        print "...",
        if mu.stage_files_and_get_others([dl] + self.to_stage, [dl]) == 0:
            if staged:
                del self.to_download[self.to_download.index(dl)]
            else:
                del self.to_stage[self.to_stage.index(dl)]
            self.to_submit.append(dl)
            self.to_download = self.to_download + self.to_stage
            self.to_stage = []
            self.n_downloaded = self.n_downloaded + 1
            print "Success!"
        else:
            print "Failed. Will try again later."
        return

    ## \brief
    #  Submit a single job if there is a job to submit, and register
    #  this with the database.
    #
    #  \return A list with two objects; the first is the run number of the
    #  submitted job (int), the second is an SGEJob for following the
    #  job on the grid. None is returned if there is nothing to submit.
    def SubmitOne(self):
        # We use a for loop in case there
        # are no runs to be submitted.
        for run in self.to_submit[0:1]:
            print "Submitting run:", run
            infile = None
            if self.prod == _ROOTANA:
                infile = self.dbm.GetRootanaInputFile(run)
            job = mu.submit_job(run, self.prod, infile)
            self.to_finish.append(self.to_submit.pop(0))
            self.dbm.RegisterRunStart(run)
            return [run, job]
        return

    ## \brief
    #  When a run has been complete, register and put files where they belong.
    #
    #  \details
    #  When a run is finished, the location of its outputs are registered with
    #  the database. These are the tree and hist files for alcapana, and the
    #  out file for rootana. Additionally, the modules file for each is loaded
    #  completely into a text field in the database, not just the location.
    #  The output files are also linked to in the main directories. That is,
    #  each output file lives in a data/filetype/version folder, and links
    #  are provided in data/filetype.
    #
    #  \param[in] run The run number that has completed on the grid (int)
    def Finish(self, run):
        self.n_runs = self.n_runs - 1
        self.n_downloaded = self.n_downloaded - 1
        self.dbm.RegisterRunFinish(run)
        if self.prod == _ALCAPANA:
            mu.cleanup(run)
            tree = mu.TREEdir + "/v%d/tree%05d.root" % (self.ver, run)
            hist = mu.HISTdir + "/v%d/hist%05d.root" % (self.ver, run)
            dump = mu.DUMPdir + "/v%d/dump%05d.odb" % (self.ver, run)
            olog = mu.LOGdir  + "/v%d/alcapana.run%05d.out" % (self.ver, run)
            elog = mu.LOGdir  + "/v%d/alcapana.run%05d.err" % (self.ver, run)
            new_paths = {"tree":tree, "hist":hist, "odb":dump, "olog":olog, "elog":elog}
            self.dbm.RegisterFile(run, mu.DAQdir + "/analyzer/work/production/MODULES", "modules")
        elif self.prod == _ROOTANA:
            out = mu.OUTdir + "/v%d/out%05d.root" % (self.ver, run)
            olog = mu.LOGdir + "/v%d/rootana.run%05d.out" % (self.ver, run)
            elog = mu.LOGdir + "/v%d/rootana.run%05d.err" % (self.ver, run)
            new_paths = {"out":out, "olog":olog, "elog":elog}
            self.dbm.RegisterFile(run, mu.DAQdir + "/analyzer/rootana/production.cfg", "modules")
        old_paths = dict((ftype, path.replace("/v%d" % self.ver, "")) for ftype, path in new_paths.iteritems())
        for ftype in old_paths.keys():
            new_dir = os.path.dirname(new_paths[ftype])
            if not os.path.exists(new_dir):
                os.makedirs(new_dir)
            os.rename(old_paths[ftype], new_paths[ftype])
            os.symlink(new_paths[ftype], old_paths[ftype])
            self.dbm.RegisterFile(run, new_paths[ftype], ftype)
        print "Finished run:", run
        self.to_finish.remove(run)
        return

    ## \brief
    #  Unclaim a list of runs in the database and delete all files associated with unfinished production;
    #  if no run provided, abort all runs.
    #
    #  \param[in] runs List of run numbers to abort. If not present, abort all runs.
    def Abort(self, runs=None):
        if runs:
            self.n_runs = self.n_runs - len(runs)
            for run in runs:
                if run in self.to_stage:
                    self.to_stage.remove(run)
                elif run in self.to_download:
                    self.to_download.remove(run)
                elif run in self.to_submit:
                    self.to_submit.remove(run)
                    self.n_downloaded = self.n_downloaded - 1
                elif run in self.to_finish:
                    self.to_finish.remove(run)
                    self.n_downloaded = self.n_downloaded - 1
                else:
                    msg = "Asked to abort run that is not managed."
                    raise AlCapError(msg)

        else:
            runs = self.to_stage + self.to_download + self.to_submit + self.to_finish
            self.n_runs = 0
            self.n_downloaded = 0
            self.to_stage = self.to_download = self.to_submit = self.to_finish = []
        print "Aborting runs:", runs
        for run in runs:
            self.dbm.RollbackRun(run)
            mu.remove_all_files(run, self.prod)
        return

    ## \brief
    #  The %RunManager is considered busy if it is managing any runs.
    def Busy(self):
        return self.n_runs != 0
