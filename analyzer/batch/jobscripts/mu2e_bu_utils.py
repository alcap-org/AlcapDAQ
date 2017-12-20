import SGEJob
import ScreenManager
from AlCapExceptions import *

import os
import shutil
import subprocess
import ftplib
import netrc
import datetime
import time

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
CFGdir = DAQdir + "/analyzer/rootana/configurations"

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

#ftpurl = "archivftp.psi.ch"
#ftpdir = "mu2e/R15b"
#ftpinfo = netrc.netrc(os.environ["HOME"] + "/.netrc").authenticators(ftpurl)
#if not ftpinfo:
#    raise AlCapError("Could not find FTP info in netrc file!")


## \brief
#  Determines the fractional usage of allowed disk space
#
#  \return The fraction of space you've used (between 0 and 1 hopefully).
def fraction_of_quota_used():
    cmd = ['du', '-s', '-BM', str(DATAdir)]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    # The first token has the memory used in megabytes
    use_loc = 0 
    use = out.split()[use_loc]
    use = use.split('M')[0]

    cmd = ['df', '-h', '-BM', '/home']
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    # The second token of the second line gives the total size of /home
    # divide by 5 to split evenly between analyzers with room to spare
    quota_line = 1
    quota_loc = 1
    quota = out.split("\n")[quota_line].split()[quota_loc]
    quota = quota.split('M')[0]
    
    return float(use)/(float(quota)/5)

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
        # Ignore g4sim
        if tokens[prog_name_loc] == "g4sim":
            continue
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
def submit_job(run, prog, infile, mods=None, calib=False, calibdatabase=""):
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
        cmd.append(os.path.abspath(mods))
        if calib:
            cmd.append("-c")
        if calibdatabase:
            cmd.append("-d " + calibdatabase)
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
#  All the files are on /data/R15bRaw already, no need to download from tape
#  so just create a soft link
#
#  \details
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

    for run in getnames:
        cmd = ['ln', '-s', '/data/R15bRaw/mid/'+run, target_dir+run]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        [out, err] = proc.communicate()
        
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
#    files.append(LOGdir + "/" + prod + ".run%05d.out" % run)
#    files.append(LOGdir + "/" + prod + ".run%05d.err" % run)
    for f in files:
        if os.path.exists(f):
            os.remove(f)
    return

## \brief
#  Request list of jobs in the queue be stopped.
#
#  \param[in] jobs A list of SGEJobs to be stopped.
def abort_jobs(jobs, screenman=ScreenManager.Dummy()):
    cmd = "qdel"
    for job in jobs:
        msg = "Cancelling job " + str(job.job_id)
        screenman.Message(msg)
        arg = str(job.job_id)
        subprocess.Popen([cmd, arg], stdout=subprocess.PIPE)
    return

## \brief
#  Get exit status of job
#
#  \details
#  Checks the exit status of a job using the qacct command.
#  No checks are done, the job is assumed to have finished.
#
#  \param id Job id to check exit status of
def exit_status(id):
    cmd = ["qacct", "-j", str(id)]
    ntrys = 0
    while ntrys < 100:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        [out, err] = proc.communicate();
        stats = dict(kv.split()[0:2] for kv in out.replace("=","").split("\n")[1:-1])
        if "exit_status" in stats:
            return int(stats["exit_status"])
        ntrys = ntrys + 1
        time.sleep(0.5)
    raise AlCapException("Job ID not in queue account: " + str(id))
    
