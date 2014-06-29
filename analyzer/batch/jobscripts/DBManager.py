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

    ## \brief
    #  Produces a database manager for a certain production type (alcapana or rootana)
    #  and a certain production version.
    #
    #  \param[in] prog The production type; rootana or alcapana.
    #  \param[in] ver The version number this %DBManager should be associated with.
    #  Essentially what production table in the database to look up info in.
    def __init__(self, prog, ver):
        if prog not in DBManager._PROGRAMS:
            raise UnknownProductionError(prog)
        if prog == DBManager._ROOTANA:
            print "rootana not implement yet...raising unknown production type error"
            raise UnknownProductionError(prog)
        self.prog = prog
        self.ver = ver
        if not os.path.isfile(DBManager._DBFILE):
            msg = "Database not found!"
            print msg
            raise DataBaseError(msg)
        elif not os.access(DBManager._DBFILE, os.W_OK):
            msg = "Database not writable!"
            print msg
            raise DataBaseError(msg)
        self.db = sqlite3.connect(DBManager._DBFILE)
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
    #  Register a run as claimed in the database.
    def ClaimRun(self, run):
        cmd = "UPDATE " + self.production_table + " SET status='C', user=? WHERE run=?"
        self.db.execute(cmd, (os.environ["USER"], run))
        self.db.commit()

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
    #  Register a run as having been submitted to the grid.
    #
    #  \param[in] run The run number of the run to register as processing.
    def RegisterRunStart(self, run):
        now = datetime.datetime.now()
        cmd = "UPDATE " + self.production_table + " SET status='R',start=? WHERE run=?"
        self.db.execute(cmd, (now, run))
        self.db.commit()
        return
        
    ## \brief
    #  Register a run in the database as complete.
    #
    #  \param[in] run The run number of the run to register as done.
    def RegisterRunFinish(self, run):
        now = datetime.datetime.now()
        cmd = "UPDATE " + self.production_table + " SET status='F',stop=? WHERE run=?"
        self.db.execute(cmd, (now, run))
        self.db.commit()
        return

    ## \brief
    #  Return the version number of the most recent production of this type.
    #
    #  \details
    #  The "most recent" is just assumed to be the highest version number.
    #  Checks the productions table for this information.
    #
    #  \return The version number of the most recent production, or 0
    #  if there have been no productions.
    def GetRecentProductionVersionNumber(self):
        cmd = "SELECT version FROM productions WHERE type=? ORDER BY version DESC LIMIT 1"
        row = self.db.execute(cmd, (self.prog,)).fetchone()
        if row:
            return row[0]
        return 0

    ## \brief
    #  Register a new production of the type this
    #  %DBManager was started as that has version number
    #  one higher than the previous production of this type.
    #
    #  \details
    #  This functions performs two actions. One is register a
    #  started production with the productions table, the other
    #  is creating a new table for this production.
    def StartNewProduction(self, qual="gold"):
        with self.db:
            self.ver = self.GetRecentProductionVersionNumber() + 1
            self.production_table = production_table_name(self.prog, self.ver)
            start = datetime.datetime.now()
            cmd = "INSERT INTO productions(type, version, start) VALUES (?, ?, ?)"
            self.db.execute(cmd, (self.prog, self.ver, start))
            if self.prog == DBManager._ALCAPANA:
                cmd = ("CREATE TABLE " + self.production_table +
                       "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, tree TEXT, hist TEXT, odb TEXT, modules TEXT)")
            elif self.prog == DBManager._ROOTANA:
                cmd = ("CREATE TABLE " + self.production_table +
                       "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, out TEXT, modules TEXT)")
            self.db.execute(cmd)
            cmd = "SELECT run FROM datasets WHERE quality=?"
            cur = self.db.execute(cmd, (qual,))
            cmd = "INSERT INTO " + self.production_table + "(run, status) VALUES (?, ?)"
            for row in cur:
                self.db.execute(cmd, (row[0], "N"))
        return self.ver

    ## \brief
    #  Register the production as finished.
    def FinishProduction(self):
        stop = datetime.datetime.now()
        cmd = "UPDATE productions SET stop=? WHERE type=? AND version=?"
        self.db.execute(cmd, (stop, self.prog, self.ver))
        self.db.commit()
        return

    ## \brief
    #  Check whether the production this %DBManager is associated with is finished.
    #
    #  \details
    #  The presence of rows with a status other than F (for finished)
    #  are searched for. If none are found, the production is considered
    #  finished and is marked accordingly.
    #
    #  \return True if production is finished, false otherwise.
    def IsProductionFinished(self):
        cmd = "SELECT * FROM " + self.production_table + " WHERE status!='F'"
        cur = self.db.execute(cmd);
        return not cur.fetchone()

    ## \brief
    #  Inform the database as to the location of a production-related
    #  file or store the entirety of ODB and MODULES files in it.
    #
    #  \details
    #  This command can only be used to register the location of files
    #  the database understands. For alcapana production, these are
    #  the tree and hist, files. For rootana, this is
    #  the out file. For the ODB dump from an alcapana production,
    #  and the MODULES files from each production type, the entirety
    #  of the text is stored in the database.
    #
    #  \param[in] run The run number of the run the file is associated
    #  with.
    #  \param[in] loc The absolute path of the file.
    #  \param[in] One of either "tree", "hist", "odb",
    #  or "modules" for alcapana, and "out" or "modules" for rootana.
    #  Indicates the type of file so the
    #  appropriate column can be filled in in the production database.
    def RegisterFile(self, run, loc, filetype=None):
        cmd = "UPDATE " + self.production_table + " SET " + filetype + "=? WHERE run=?"
        if self.prog == "alcapana":
            if filetype in ["tree", "hist"]:
                values = (loc, run)
            elif filetype in ["odb", "modules"]:
                data = open(loc, "r").read()
                values = (data, run)
            else:
                raise UnknownAlcapanaFiletypeError(filetype)
        elif self.prog == "rootana":
            if filetype == "out":
                values = (loc, run)
            elif filetype == "modules":
                data = open(loc, "r").read()
                values = (data, run)
            else:
                raise UnknownRootanaFiletypeError(filetype)
        self.db.execute(cmd, values)
        self.db.commit()
        return

    ## \brief
    #  If an error occurs, this can be called to register
    #  a run as if it was never claimed.
    #
    #  \param[in] run The run number to re-register as unclaimed.
    def RollbackRun(self, run):
        with self.db:
            cmd = "UPDATE " + self.production_table + " SET status='N', user=?, start=?, "
            if self.prog == DBManager._ALCAPANA:
                cmd = cmd + "tree=?, hist=?, modules=?, odb=? "
                values = (None, None, None, None, None, None, run)
            elif self.prog == DBManager._ROOTANA:
                cmd = cmd + "out=?, modules=? "
                values = (None, None, None, None, run)
            cmd = cmd + "WHERE run=?"
            self.db.execute(cmd, values)
        return


## \brief
#  The canonical production table names are PRODUCTIONTYPE_vVERSIONNUMBER
def production_table_name(prod, ver):
    return prod + "_v" + str(ver)
