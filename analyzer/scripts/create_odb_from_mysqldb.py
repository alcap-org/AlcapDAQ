from getpass import getpass
import mysql.connector
import time, pytz, calendar

MYSQL_USER = 'alcap'
MYSQL_HOST = 'muon.npl.washington.edu'
MYSQL_DB   = 'alcap'

db = mysql.connector.connect(user=MYSQL_USER, password=getpass(),
                             host=MYSQL_HOST, database=MYSQL_DB)
cur = db.cursor()
cur.execute('SELECT run,stoptime,nblock,size FROM R15b')
for (run, tf, nb, size) in cur:
    if not tf:
        continue
    tz = pytz.timezone('Europe/Zurich')
    tf = tz.localize(tf)
    with open('run%05d_eor.odb' % run, 'w') as of:
        of.write('[Runinfo]\n')
        of.write('Stop time = STRING : [32] ' +
                 tf.strftime('%a %b %d %H:%M:%S %Y') + '\n')
        of.write('Stop time binary = DWORD : ' +
                 str(calendar.timegm(tf.astimezone(pytz.utc).timetuple())) +
                 '\n')
        of.write('\n')
        of.write('[Logger/Channels/0/Statistics]\n')
        of.write('Events written = DOUBLE : ' + str(nb) + '\n')
        of.write('Bytes written = DOUBLE : ' + str(size) + '\n')
