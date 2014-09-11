import merlin_utils as mu
from AlCapExceptions import *
import ScreenManager

import sqlite3
import datetime
import os

class DBManager:

    _ALCAPANA = "alcapana"
    _ROOTANA = "rootana"
    _PROGRAMS = [_ALCAPANA, _ROOTANA]

    ## \brief
    #  Produces a database manager for a certain production type (alcapana or rootana)
    #  and a certain production version.
    #
    #  \param[in] prog The production type; rootana or alcapana.
    #  \param[in] ver The version number this %DBManager should be associated with.
    #  Essentially what production table in the database to look up info in.
    def __init__(self, prog, ver, screenman, dbfile):
        if prog not in DBManager._PROGRAMS:
            raise UnknownProductionError(prog)
        self.prog = prog
        if not os.path.isfile(dbfile):
            raise DataBaseError("Database not found!")
        if not os.access(dbfile, os.W_OK):
            raise DataBaseError("Database not writable!")
        self.db = sqlite3.connect(dbfile)
        if not ver:
            self.ver = self.GetRecentProductionVersionNumber()
        else:
            self.ver = ver
        self.production_table = production_table_name(self.prog, self.ver)
        self.screenman = screenman

    ## \brief
    #  Get the run number of an avaliable run from a certian production.
    #
    #  \param[in] prod The production type of either alcapana or rootana (string)
    #  \param[in] ver The version of the production (int).
    #  \return A run number of a run not yet claimed (int).
    def GetAnyAvailableRunNumber(self, datasets=[]):
        cmd = "SELECT run FROM " + self.production_table + " WHERE status='N'"
        cur = self.db.execute(cmd)
        for row in cur:
            if len(datasets) == 0:
                return row[0]
            cmd = "SELECT dataset FROM datasets WHERE run==?"
            data = self.db.execute(cmd, (row[0],))
            for datum in data:
                if str(datum[0]) in datasets:
                    return row[0]
        return None

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
    def ClaimAnyAvailableRun(self, datasets=[]):
    # We don't want someone to claim a run before we do, so we
    # lock the database in a context block before checking.
        with self.db:
            run = self.GetAnyAvailableRunNumber(datasets)
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
    #
    #  \param[in] tag The software tag used for this production. (String)
    #  \param[in] qual The quality of data to use.
    #  \param[in] base The alcapana version base for rootana production.
    #  Ignored for alcapana production.
    def StartNewProduction(self, tag, qual, base):
        if self.prog == DBManager._ALCAPANA:
            base = None
        elif base == None:
            msg = "Rootana production must have alcapana base version!"
            raise AlCapError(msg)
        with self.db:
            self.ver = self.GetRecentProductionVersionNumber() + 1
            self.production_table = production_table_name(self.prog, self.ver)
            start = datetime.datetime.now()
            cmd = "INSERT INTO productions(type, version, software, start, base) VALUES (?, ?, ?, ?, ?)"
            self.db.execute(cmd, (self.prog, self.ver, tag, start, base))
            if self.prog == DBManager._ALCAPANA:
                cmd = ("CREATE TABLE " + self.production_table +
                       "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, tree TEXT, hist TEXT, odb TEXT, olog TEXT, elog TEXT, modules TEXT)")
            elif self.prog == DBManager._ROOTANA:
                cmd = ("CREATE TABLE " + self.production_table +
                       "(run INTEGER, status TEXT, user TEXT, start TIMESTAMP, stop TIMESTAMP, out TEXT, olog TEXT, elog TEXT, modules TEXT)")
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
    #  the tree, hist, and ODB files. For rootana, this is
    #  the out file. For the MODULES files from each production type, the entirety
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
        if self.prog == DBManager._ALCAPANA:
            if filetype in ["tree", "hist", "odb", "olog", "elog"]:
                values = (loc, run)
            elif filetype in ["modules"]:
                data = open(loc, "r").read()
                values = (data, run)
            else:
                raise UnknownAlcapanaFiletypeError(filetype)
        elif self.prog == DBManager._ROOTANA:
            if filetype in ["out", "olog", "elog"]:
                values = (loc, run)
            elif filetype in ["modules"]:
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
        cmd = "UPDATE " + self.production_table + " SET status='N', user=?, start=?, modules=?, "
        if self.prog == DBManager._ALCAPANA:
            cmd = cmd + "tree=?, hist=?, odb=? "
            values = (None, None, None, None, None, None, run)
        elif self.prog == DBManager._ROOTANA:
            cmd = cmd + "out=? "
            values = (None, None, None, None, run)
        cmd = cmd + "WHERE run=?"
        self.db.execute(cmd, values)
        self.db.commit()
        return


    ## \brief
    #  Get the output tree file name from alcapana for input
    #  into rootana.
    #
    #  \details
    #  The correct file to use as input is from the "base"
    #  alcapana production.
    #
    #  \param[in] run Run number to find the right file for.
    #  \return The absolute path to the correct file.
    def GetRootanaInputFile(self, run):
        fname = None
        cmd = "SELECT base FROM productions WHERE type=? AND version=?"
        for irow in self.db.execute(cmd, (DBManager._ROOTANA, self.ver)):
            base = int(irow[0])
            cmd = "SELECT tree FROM " + production_table_name(DBManager._ALCAPANA, base) + " WHERE run=?"
            for jrow in self.db.execute(cmd, (run,)):
                fname = jrow[0]
        if fname and os.path.exists(fname):
            return fname
        return None


## \brief
#  The canonical production table names are PRODUCTIONTYPE_vVERSIONNUMBER
def production_table_name(prod, ver):
    return prod + "_v" + str(ver)
