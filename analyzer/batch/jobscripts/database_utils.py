import sqlite3
import merlin_utils
import datetime
import os

form = "%Y-%m-%d %H:%M:%S.%f"

db_fname = merlin_utils.DATAdir + "/production.db"
if not os.path.isfile(db_fname):
    print "Database not found! Populate first!"
    raise merlin_utils.AlCapException("Database not found")
db = sqlite3.connect(db_fname)


def raise_if_not_valid_production(prod):
    if prod != "alcapana" and prod != "rootana":
        print "Job submission error: '" + prod + "' not a valid production type!"
        raise merlin_utils.AlCapException("Invalid production type")


def get_any_available_run_number(prod, ver):
    cmd = "SELECT run FROM " + production_table_name(prod, ver) + " WHERE status='N' LIMIT 1"
    cur = db.execute(cmd)
    row = cur.fetchone()
    if not row:
        return None
    return row[0]


def claim_any_available_run(prod, ver=None):
    if not ver:
        ver = get_recent_production(prod)
    with db:
        run = get_any_available_run_number(prod, ver)
        if not run:
            return None
        claim_run(prod, ver, run)
    return run


def claim_run(prod, ver, run):
    cmd = "UPDATE " + production_table_name(prod, ver) + " SET status='C' WHERE run=?"
    db.execute(cmd, (run,))
    db.commit()


def register_run_start(prod, ver, run):
    now = datetime.datetime.now()
    cmd = "UPDATE " + production_table_name(prod, ver) + " SET status='R',start=?,user=? WHERE run=?"
    db.execute(cmd, (now, os.environ["USER"], run))
    db.commit()


def register_run_finish(prod, ver, run):
    now = datetime.datetime.now()
    cmd = "UPDATE " + production_table_name(prod, ver) + " SET status='F',stop=? WHERE run=?"
    db.execute(cmd, (now, run))
    db.commit()


def get_recent_production(prod):
    cmd = "SELECT version FROM productions WHERE type='" + prod + "' ORDER BY version DESC LIMIT 1"
    return db.execute(cmd).fetchone()[0]


def start_production(prod, qual="gold"):
    cmd = "SELECT version FROM productions WHERE type=? ORDER BY version DESC LIMIT 1"
    row = db.execute(cmd, (prod,)).fetchone()
    if row:
        ver = row[0] + 1
    else:
        ver = 1
    start = datetime.datetime.now()
    cmd = "INSERT INTO productions VALUES ('alcapana', ?, ?, ?, ?)"
    db.execute(cmd, (ver, start, None, ver - 1))
    cmd = "CREATE TABLE " + production_table_name(prod, ver) + "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, location TEXT)"
    db.execute(cmd)
    cmd = "SELECT run FROM datasets WHERE quality=?"
    cur = db.execute(cmd, (qual,))
    cmd = "INSERT INTO " + production_table_name(prod, ver) + "(run, status) VALUES (?, ?)"
    for row in cur:
        db.execute(cmd, (row[0], "N"))
    db.commit()
    return ver


def finish_production(prod, ver):
    stop = datetime.datetime.now()
    cmd = "UPDATE " + production_table_name(prod, ver) + " SET stop=? WHERE type=? AND version=?"
    db.execute(cmd, (stop, prod, ver))


def is_production_finished(prod, ver):
    cmd = "SELECT * FROM " + production_table_name(prod, ver) + " WHERE staus='N' OR status='R' OR status='C'"
    cur = db.execute(cmd);
    if not cur.fetchone():
        return True
    return False


def register_file_location(prod, ver, run, loc):
    cmd = "UPDATE " + production_table_name(prod, ver) + " SET location=? WHERE run=?"
    db.execute(cmd, (loc, run))


def production_table_name(prod, ver):
    return prod + "_v" + str(ver)
