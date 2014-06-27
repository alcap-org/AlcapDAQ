import sqlite3
import merlin_utils
import datetime
import os
from AlCapExceptions import *

class DBManager:

    _ALCAPANA = "alcapana"
    _ROOTANA = "rootana"
    _PROGRAMS = [_ALCAPANA, _ROOTANA]

    _DBFILE = merlin_utils.DATAdir + "/production.db"

    def __init__(self, prog, ver):
        if prog not in DBManager._PROGRAMS:
            raise UnknownProductionError(prog)
        self.prog = prog
        self.ver = ver
        if not os.path.isfile(db_fname):
            msg = "Database not found!"
            print msg
            raise DataBaseError(msg)
        self.db = sqlite3.connect(_DBFILE)
        self.production_table = production_table_name(prog, ver)

    ## \brief
    #  Get the run number of an avaliable run from a certian production.
    #
    #  \param[in] prod The production type of either alcapana or rootana (string)
    #  \param[in] ver The version of the production (int).
    #  \return A run number of a run not yet claimed (int).
    def GetAnyAvailableRunNumber(self):
        cmd = "SELECT run FROM " + self.production_table + " WHERE status='N' LIMIT 1"
        cur = self.db.execute(cmd)
        row = cur.fetchone()
        if not row:
            return None
        return row[0]

    ## \brief
    #  Find an available run in a production set and claim it in the database.
    #
    #  \param[in] prod The production tpye, either alcapana or rootana (string)
    #  \param[in] ver The version of the production (int); if none provided
    #  default to most recent version.
    #  \return The run number of the claimed run (int).
    def ClaimAnyAvailableRun(self):
    # We don't want someone to claim a run before we do, so we
    # lock the database in a context block before checking.
        with self.db:
            run = self.GetAnyAvailableRunNumber()
            if not run:
                return None
            self.ClaimRun(run)
        return run

    ## \brief
    #  Register a run as claimed in the database.
    def ClaimRun(self, run):
        cmd = "UPDATE " + self.production_table + " SET status='C' WHERE run=?"
        self.db.execute(cmd, (run,))
        self.db.commit()


    def RegisterRunStart(self, run):
        now = datetime.datetime.now()
        cmd = "UPDATE " + self.production_table + " SET status='R',start=?,user=? WHERE run=?"
        self.db.execute(cmd, (now, os.environ["USER"], run))
        self.db.commit()


    def RegisterRunFinish(self, run):
        now = datetime.datetime.now()
        cmd = "UPDATE " + self.production_table + " SET status='F',stop=? WHERE run=?"
        self.db.execute(cmd, (now, run))
        self.db.commit()


    def GetRecentProduction(self):
        cmd = "SELECT version FROM productions WHERE type='" + self.prog + "' ORDER BY version DESC LIMIT 1"
        return self.db.execute(cmd).fetchone()[0]


    def StartProduction(self, prod=self.prog, qual="gold"):
        if prod not in DBManager._PROGRAMS:
            raise UnknownProductionError(prod)
        self.prog = prod
        with self.db:
            cmd = "SELECT version FROM productions WHERE type=? ORDER BY version DESC LIMIT 1"
            row = self.db.execute(cmd, (self.prog,)).fetchone()
            if row:
                self.ver = row[0] + 1
            else:
                self.ver = 1
            self.production_table = production_table_name(self.prog, self.ver)
            start = datetime.datetime.now()
            cmd = "INSERT INTO productions VALUES (?, ?, ?, ?, ?)"
            self.db.execute(cmd, (self.prog, self.ver, start, None))
            if self.prog == DBManager._ALCAPANA:
                cmd = ("CREATE TABLE " + self.production_table +
                       "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, tree TEXT, hist TEXT, dump TEXT, modules BLOB)")
            elif self.prog == DBManager._ROOTANA:
                cmd = ("CREATE TABLE " + self.production_table +
                       "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, out TEXT, modules BLOB)")
            self.db.execute(cmd)
            cmd = "SELECT run FROM datasets WHERE quality=?"
            cur = self.db.execute(cmd, (qual,))
            cmd = "INSERT INTO " + self.production_table + "(run, status) VALUES (?, ?)"
            for row in cur:
                self.db.execute(cmd, (row[0], "N"))
        return self.ver

    def StartTestProduction(self):
        cmd = "CREATE TABLE alcapana_v4(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, tree TEXT, hist TEXT, dump TEXT)"
        self.db.execute(cmd)
        self.db.execute("INSERT INTO alcapana_v4(run, status) VALUES (3200, 'N')")
        self.db.execute("INSERT INTO alcapana_v4(run, status) VALUES (3201, 'N')")
        self.db.execute("INSERT INTO alcapana_v4(run, status) VALUES (3202, 'N')")
        self.db.execute("INSERT INTO alcapana_v4(run, status) VALUES (3203, 'N')")
        self.db.commit()

    def FinishProduction(self):
        stop = datetime.datetime.now()
        cmd = "UPDATE productions SET stop=? WHERE type=? AND version=?"
        self.db.execute(cmd, (stop, self.prog, self.ver))
        self.db.commit()


    def IsProductionFinished(self):
        cmd = "SELECT * FROM " + self.production_table + " WHERE status='N' OR status='R' OR status='C'"
        cur = self.db.execute(cmd);
        return not cur.fetchone()


    def RegisterFileLocation(self, run, loc, filetype=None):
        cmd = "UPDATE " + self.production_table + " SET "
        if self.prog == "alcapana":
            if filetype in ["tree", "hist", "dump"]:
                cmd = cmd + filetype + "=? WHERE run=?"
            else:
                raise UnkownAlcapanaFiletypeError(filetype)
        elif self.prog == "rootana":
            cmd = cmd + "out=? WHERE run=?"
        self.db.execute(cmd, (loc, run))
        self.db.commit()


def production_table_name(prod, ver):
    return prod + "_v" + str(ver)
