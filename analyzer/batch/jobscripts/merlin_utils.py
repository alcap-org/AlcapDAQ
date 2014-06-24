import os
import subprocess
import ftplib

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

if not os.path.exists(DATAdir):
    os.makedirs(DATAdir)
if not os.path.exists(RAWdir):
    os.makedirs(RAWdir)
if not os.path.exists(TREEdir):
    os.makedirs(TREEdir)
if not os.path.exists(HISTdir):
    os.makedirs(HISTdir)

if not os.path.exists(ODBdir):
    print "Job submission error: ODB dir (" + ODBdir + ") does not exist!"
    raise AlCapException


class SGEJob:
    WAITING = "qw"
    RUNNING = "r"
    ALCAPANA = "batch_alca"
    ROOTANA = "batch_root"
    def __init__(self, id, status, prog):
        self.id = id
        self.status = status
        self.prog = prog

    def __eq__(self, rhs):
        if isinstance(rhs, self.__class__):
            return self.id == rhs.id
        else:
            return False


def quota():
    cmd = "/usr/lpp/mmfs/bin/mmlsquota"
    arg = "merliny"
    proc = subprocess.Popen([cmd, arg], stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    out = out.split()
    use_loc = 21
    quota_loc = 22
    usage = float(out[use_loc])/float(out[quota_loc])

    return usage


def submitted_jobs():
    cmd = "qstat"
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = proc.communicate()

    out = out.split()
    header_size = 11
    for i in range(header_size):
        out.pop(0)

    id_loc = 0
    status_loc = 4
    info_length = 9
    prog_name_loc = 2

    jobs = []
    while len(out) > 0:
        jobs.append(SGEJob(int(out[id_loc]), out[status_loc], out[prog_name_loc]))
        for i in range(info_length):
            out.pop(0)

    return jobs


def submit_job(run, prog):
    if prog != "alcapana" and prog != "rootana":
        print ("Job submission error: Program not understood! (" + prog + ")")
        return 0
 
    if prog == "rootana":
        print "Rootana not implemented yet."
        return 0

    log = DAQdir + "/analyzer/batch/log/" + prog + "." + str(run) + "."
    olog = log + "log"
    elog = log + "err"

    cmd = ["qsub","-v", "DAQdir=" + DAQdir, "-e", elog, "-o", olog, DAQdir + "/analyzer/batch/scripts/batch_alcapana.sge", str(run)]
    print cmd
    con = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    [out, err] = con.communicate()
    id = int(out.split()[2])

    if prog == "alcapana":
        prog = "batch_alca"
    elif prog == "rootana":
        prog = "batch_root"
    job = SGEJob(id, "qw", prog)

    return job


def get_file(run):
    if run > 99999 or run < 0:
        print "Job submission error: Run out of range (" + str(run) + ")"
        return 2
    url = "archivftp.psi.ch"
    loc = "mu2e/run2013/"
    fname = "run" + ("%05d" % run) + ".mid"
    dest = os.environ["HOME"] + "/data/raw/"
    user = "mucap"
    passwd_file = DAQdir + "/analyzer/batch/password.txt"
    if not os.path.isfile(passwd_file):
        print "Job submission error: Password file does not exists!"
        return 3
    passwd = open(passwd_file, "r").next()[:-1]

    if not os.path.exists(dest):
        os.makedirs(dest)

    con = ftplib.FTP(url)
    try:
        con.login(user, passwd)
    except ftplib.error_perm:
        print "Job submission error: Too many connection to FTP server!"
        return 4
    con.cwd(loc)
    con.retrbinary("RETR " + fname, open(dest + fname, "wb").write)

    return 0


def make_space():
    pass


def cleanup(run):
    pass
