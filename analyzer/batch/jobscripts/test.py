import ftplib

user = "mucap"
passwd = "84mfQhhU]"
loc = "mu2e/run2013"
url = "archivftp.psi.ch"
cmd = "RETR"
fname = "run01998.mid"

con = ftplib.FTP(url)

s = con.login(user, passwd)
print s
s = con.cwd(loc)
print s
s = con.retrbinary(cmd + " " + fname, open(fname, "wb").write)
print s
