import os
import subprocess
import ftplib
import netrc

class AlCapException(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.valiue)

if "DAQdir" not in os.environ.keys():
    print "Job submission error: DAQdir not set! Source thisdaq.sh!"
    raise AlCapException("DAQdir not set")

DAQdir = os.environ["DAQdir"]
DATAdir = os.environ["HOME"] + "/data"
RAWdir = DATAdir + "/raw"
TREEdir = DATAdir + "/tree"
HISTdir = DATAdir + "/hist"
ODBdir = DATAdir + "/odb"
CORRdir = DATAdir + "/corr"
DUMPdir = DATAdir + "/dump"

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

if not os.path.exists(ODBdir):
    print "Job submission error: ODB dir (" + ODBdir + ") does not exist!"
    raise AlCapException

ftpurl = "archivftp.psi.ch"
ftpdir = "mu2e/run2013"
ftpinfo = netrc.netrc(os.environ["HOME"] + "/.netrc").authenticators(ftpurl)
if not ftpinfo:
    print "Could not find correct FTP info in netrc file!"
    raise AlCapException("Could not find correct FTP info in netrc file!")


class SGEJob:
    WAITING = "qw"
    RUNNING = "r"
    TRANSFERRING = "t"
    ALCAPANA = "batch_alca"
    ROOTANA = "batch_root"
    def __init__(self, job_id, status, prog):
        self.job_id = job_id
        self.status = status
        self.prog = prog

    def __eq__(self, rhs):
        if isinstance(rhs, self.__class__):
            return self.job_id == rhs.job_id
        else:
            return False


def fraction_of_quota_used():
    cmd = "/usr/lpp/mmfs/bin/mmlsquota"
    arg = "merliny"
    proc = subprocess.Popen([cmd, arg], stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    quota_line = 2
    use_loc = 2
    quota_loc = 3
    [use, quota] = out.split("\n")[quota_line].split()[use_loc:quota_loc+1]
    
    return float(use)/float(quota)


def submitted_jobs():
    cmd = "qstat"
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    out_lines = out.split("\n")
    header_size = 2
    id_loc = 0
    status_loc = 4
    prog_name_loc = 2
    jobs = []
    for line in out_lines[header_size:-1]:
        tokens = line.split()
        jobs.append(SGEJob(int(tokens[id_loc]), tokens[status_loc], tokens[prog_name_loc]))

    return jobs


def submit_job(run, prog):
    if prog not in ["alcapana", "rootana"]:
        msg = "Job submission error: Program not understood! (" + prog + ")"
        print msg
        raise AlCapException(msg)
    elif prog == "rootana":
        msg = "Rootana not implemented yet."
        print msg
        raise AlCapException(msg)
    
    log = DAQdir + "/analyzer/batch/log/" + prog + ".run" + ("%05d" % run) + "."
    olog = log + "out"
    elog = log + "err"

    cmd = ["qsub", "-v", "DAQdir=" + DAQdir, "-e", elog, "-o", olog,
           DAQdir + "/analyzer/batch/scripts/batch_alcapana.sge", str(run)]
    con = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = con.communicate()
    job_id = int(out.split()[2])

    if prog == "alcapana":
        prog = "batch_alca"
    elif prog == "rootana":
        prog = "batch_root"
    job = SGEJob(job_id, "qw", prog)

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
    return 0
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

def stage_files_and_get_others(stages, gets):
    target_dir = RAWdir + "/"
    stagenames = ["run%05d.mid" % run for run in stages]
    getnames = ["run%05d.mid" % run for run in gets]

    if not os.path.exists(target_dir):
        os.makedirs(target_dir)

    ftp = ftplib.FTP(ftpurl)
    try:
        ftp.login(ftpinfo[0], ftpinfo[2])
    except ftplib.error_perm:
        print "Job submission error: Problem connecting to FTP server!"
        return 1
    ftp.cwd(ftpdir)
    for run in stagenames:
        ftp.sendcmd("STAGE " + run)
    for run in getnames:
        ftp.retrbinary("RETR " + run, open(target_dir + run, "wb").write)
    ftp.close()
    return 0


def make_space():
    pass


def cleanup(run):
    pass


def remove_midas_file(run):
    fname = RAWdir + ("/run%05d.mid" % run)
    os.remove(fname)
