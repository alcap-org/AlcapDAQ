import SGEJob
from AlCapExceptions import *

import os
import shutil
import subprocess
import ftplib
import netrc
import datetime

if "DAQdir" not in os.environ.keys():
    raise AlCapError("DAQdir not set")

_ALCAPANA = "alcapana"
_ROOTANA  = "rootana"
_PROGRAMS = [_ALCAPANA, _ROOTANA]

DAQdir = os.environ["DAQdir"]
DATAdir = os.environ["HOME"] + "/data"
RAWdir = DATAdir + "/raw"
TREEdir = DATAdir + "/tree"
HISTdir = DATAdir + "/hist"
ODBdir = DATAdir + "/odb"
CORRdir = DATAdir + "/corr"
DUMPdir = DATAdir + "/dump"
LOGdir = DATAdir + "/log"
OUTdir = DATAdir + "/out"
TMPdir = DAQdir + "/analyzer/batch/tmp/odb"

if not os.path.exists(DATAdir):
    os.makedirs(DATAdir)
if not os.path.exists(RAWdir):
    os.makedirs(RAWdir)
if not os.path.exists(TREEdir):
    os.makedirs(TREEdir)
if not os.path.exists(HISTdir):
    os.makedirs(HISTdir)
if not os.path.exists(DUMPdir):
    os.makedirs(DUMPdir)
if not os.path.exists(LOGdir):
    os.makedirs(LOGdir)
if not os.path.exists(OUTdir):
    os.makedirs(OUTdir)
if not os.path.exists(TMPdir):
    os.makedirs(TMPdir)

if not os.path.exists(ODBdir):
    raise AlCapError("ODB dir (" + ODBdir + ") does not exist!")

ftpurl = "archivftp.psi.ch"
ftpdir = "mu2e/run2013"
ftpinfo = netrc.netrc(os.environ["HOME"] + "/.netrc").authenticators(ftpurl)
if not ftpinfo:
    raise AlCapError("Could not find FTP info in netrc file!")


## \brief
#  Determines the fractional usage of allowed disk space on
#  Merlin using the /usr/lpp/mmfs/bin/mmlsquota command.
#
#  \return The fraction of space you've used (between 0 and 1 hopefully).
def fraction_of_quota_used():
    cmd = "/usr/lpp/mmfs/bin/mmlsquota"
    arg = "merliny"
    proc = subprocess.Popen([cmd, arg], stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    # The third and fourth tokens (numbers/words) of the third line
    # of the output of the quota command contain the information in
    # units of KB.
    quota_line = 2
    use_loc = 2
    quota_loc = 3
    [use, quota] = out.split("\n")[quota_line].split()[use_loc:quota_loc+1]
    
    return float(use)/float(quota)

## \brief
#  Determine what jobs the user has submitted to the queue.
#
#  \details
#  This is accomplished by parsing the output of the qstat command.
#
#  \return A list of SGEJobs representing the running jobs.
def submitted_jobs():
    cmd = "qstat"
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    # The first 2 lines of the qstat output are header lines.
    # On each subsequent line, the job id is the first token,
    # the program name the third, and the current job status
    # the fifth.
    out_lines = out.split("\n")
    header_size    = 2
    id_loc        = 0
    status_loc    = 4
    prog_name_loc = 2
    date_loc      = 5
    time_loc      = 6
    jobs = []
    for line in out_lines[header_size:-1]:
        tokens = line.split()
        jobs.append(SGEJob.SGEJob(int(tokens[id_loc]), tokens[status_loc],
                                  tokens[prog_name_loc], tokens[date_loc],
                                  tokens[time_loc]))

    return jobs

## \brief
#  Submit either an alcapana or rootana production job
#  on a run.
#
#  \param[in] run The run number (integer).
#  \param[in] prog The name of the program to run; either alcapana or
#  rootana.
#  \param[in] infile The absolute path of the input file for rootana. Ignored
#  for alcapana.
#  \return An SGEJob representing the submitted job.
def submit_job(run, prog, infile):
    if prog not in _PROGRAMS:
        raise UnknownProductionError(prog)
    
    log = LOGdir + "/" + prog + ".run" + ("%05d" % run) + "."
    olog = log + "out"
    elog = log + "err"

    cmd = ["qsub", "-v", "DAQdir=" + DAQdir, "-e", elog, "-o", olog, "-N", prog,
           DAQdir + "/analyzer/batch/scripts/batch_" + prog + ".sge"]
    if prog == _ALCAPANA:
        cmd.append(str(run))
    elif prog == _ROOTANA:
        cmd.append(infile)
        cmd.append(OUTdir + "/out%05d.root" % run)
        cmd.append(DAQdir + "/analyzer/rootana/production.cfg")
    con = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = con.communicate()

    # The third word of the print out after submission is the job ID.
    job_id = int(out.split()[2])

    # Assume the initial job status is waiting in the queue (qw).
    job = SGEJob.SGEJob(job_id, "qw", prog)

    return job


def stage_files(runs):
    fnames = ["run%05d.mid" % run for run in runs]
    ftp = ftplib.FTP(ftpurl)
    try:
        ftp.login(ftpinfo[0], ftpinfo[2])
    except ftplib.error_perm:
        print "Job submission error: Problem connecting to FTP server!"
        return 1
    ftp.cwd(ftpdir)
    for fname in fnames:
        ftp.sendcmd("STAGE " + fname)
    ftp.close()
    return 0

def stage_file(run):
    return stage_files([run])

def get_files(runs):
    target_dir = RAWdir + "/"
    fnames = ["run%05d.mid" % run for run in runs]
    ftp = ftplib.FTP(ftpurl)
    try:
        ftp.login(ftpinfo[0], ftpinfo[2])
    except ftplib.error_perm:
        print "Job submission error: Problem connecting to FTP server!"
        return 1
    ftp.cwd(ftpdir)
    for fname in fnames:
        ftp.retrbinary("RETR " + fname, open(target_dir + fname, "wb").write)
    ftp.close()
    return 0

def get_file(run):
    return get_files([run])

## \brief
#  Stage some files then download others from the tape archive.
#
#  \details
#  Staging files to be loaded from the archive's tapes onto their
#  hardrives for later can help speed up downloading the files, as
#  one is being downloaded another can be staged. This is in contrast
#  to downloading without staging, where the majority of the wait
#  during a download was waiting for this implicit staging
#  to happen.
#
#  \param[in] stages A list of run numbers to request have their run files staged.
#  \param[in] gets A list of run numbers to request download of their runfiles.
#  \return 0 on success, 1 if there was a problem connecting to the FTP server.
def stage_files_and_get_others(stages, gets, screenman=None):
    target_dir = RAWdir + "/"
    stagenames = ["run%05d.mid" % run for run in stages]
    getnames = ["run%05d.mid" % run for run in gets]

    if not os.path.exists(target_dir):
        os.makedirs(target_dir)

    ftp = ftplib.FTP(ftpurl)
    try:
        ftp.login(ftpinfo[0], ftpinfo[2])
    except ftplib.error_perm:
        msg = "Job submission error: Problem connecting to FTP server!"
        if screenman:
            screenman.Message(msg)
        else:
            print msg
        return 1
    ftp.cwd(ftpdir)
    for run in stagenames:
        ftp.sendcmd("STAGE " + run)
    for run in getnames:
        with open(target_dir + run, "wb") as dlfile:
            def download_noprogress(block):
                dlfile.write(block)
            def download_progress(block):
                screenman.UpdateProgress(len(block))
                dlfile.write(block)
            if not screenman:
                download = download_noprogress
            elif screenman:
                screenman.StartProgress(int(run[3:8]), ftp.size(run))
                download = download_progress
            ftp.retrbinary("RETR " + run, download)
            if screenman:
                screenman.FinishProgress()
    ftp.close()
    return 0

## \brief
#  When space is short, this deleted older files.
#
#  \todo Make this
def make_space():
    pass

## \brief
#  Delete temporary files needed during alcapana production;
#
#  \details
#  The MIDAS files and ODB SHared Memory (*.SHM) files are not needed
#  afterwards.
#
#  \param[in] run Run number of run the temporary files are associated with.
def cleanup(run):
    fname = RAWdir + "/run%05d.mid" % run
    if os.path.exists(fname):
        os.remove(fname)
    dname = TMPdir + "/run%05d" % run
    if os.path.exists(fname):
        shutil.rmtree(dname)
    return

## \brief
#  Remove all files associated with a run during some production.
#
#  \param[in] run The run number for which all associated files
#  created during production should be removed.
#  \param[in] prod The production type, either "rootana" or "alcapana".
def remove_all_files(run, prod):
    if prod == _ALCAPANA:
        cleanup(run)
        files = [TREEdir + "/tree%05d.root" % run,
                 HISTdir + "/hist%05d.root" % run,
                 DUMPdir + "/dump%05d.odb" % run]
    elif prod == _ROOTANA:
        files = [OUTdir + "/out%05d.root" % run]
    else:
        raise UnknownProductionError(prod)
    files.append(LOGdir + "/" + prod + ".run%05d.out" % run)
    files.append(LOGdir + "/" + prod + ".run%05d.err" % run)
    for f in files:
        if os.path.exists(f):
            os.remove(f)
    return

## \brief
#  Request list of jobs in the queue be stopped.
#
#  \param[in] jobs A list of SGEJobs to be stopped.
def abort_jobs(jobs, screenman=None):
    cmd = "qdel"
    for job in jobs:
        msg = "Cancelling job " + str(job.job_id)
        if screenman:
            screenman.Message(msg)
        else:
            print msg
        arg = str(job.job_id)
        subprocess.Popen([cmd, arg], stdout=subprocess.PIPE)
    return
