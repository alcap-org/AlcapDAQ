#!/usr/bin/python

import sys
import csv
import sqlite3

def none_or_float(s):
    if s == '':
        return None
    return float(s)

if len(sys.argv) < 2:
    print "Usage: ./merge_calibration_database.py DATABASE FILE1.csv FILE2.csv FILE3.csv..."
    exit(0)

db = sqlite3.connect(sys.argv[1])
infiles = sys.argv[2:]

for ifile in infiles:
    table_name = ifile.split('.')[2]
    db.execute("CREATE TABLE IF NOT EXISTS " + table_name + "(run INT, channel TEXT)")
    with open(ifile, 'rb') as f:
        table = csv.reader(f)
        cols = tuple(x.strip() for x in table.next())

        exist = [x[1] for x in db.execute("PRAGMA table_info(" + table_name + ")")]
        for col in cols:
            if col not in exist:
                with db:
                    db.execute("ALTER TABLE " + table_name + " ADD COLUMN '" + col + "' REAL")

        for line in table:
            row = (int(line[0]),line[1].strip()) + tuple(none_or_float(word) for word in line[2:])
            cmd = "SELECT * FROM " + table_name + " WHERE run==? AND channel==?"
            args = row[0:2]
            cur = db.execute(cmd, args)
            if cur.fetchone():
                cmd = "UPDATE " + table_name + " SET '" + ",'".join(s + "'=?" for s in cols[2:]);
                cmd = cmd + " WHERE run==? AND channel==?"
                args = row[2:] + row[0:2]
            else:
                cmd = "INSERT INTO " + table_name + "('" + "','".join(s for s in cols) + "') VALUES (" + ",".join("?" for s in cols) + ")"
                args = row
            with db:
                db.execute(cmd, args)
