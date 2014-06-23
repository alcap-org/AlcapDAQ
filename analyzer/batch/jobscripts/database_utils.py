import sqlite3
import merlin_utils

db_fname = merlin_utils.DATAdir + "/productionDB.db"
if not os.path.isfile(fname):
    print "Database not found! Populate first!"
    raise merlin_utils.AlCapException
db = sqlite3.connect(db_fname)

def get_run(production, quality = "gold"):
    cmd = "SELECT * FROM " + production + " WHERE quality='" + quality + " AND status='N'"
    try:
        cur = db.execute(cmd)
    except sqlite3.OperationalError:
        print "Job submission error: " + production + " is not a valid production!"
    return 0
