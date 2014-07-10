import SGEJob
import AlCapExceptions

import datetime
import curses

class ScreenManager:

    ## \name Constants
    #\{
    _HEADER_LINES   = 4
    _MAXLINES       = 24
    _MAX_RUNS       = 20
    _RUN_WIDTH      = 9
    _STATUS_WIDTH   = 14
    _DOWNLOAD_WIDTH = 31
    _TIME_WIDTH     = 16

    _TIMEOUT        = 5
    _MAXMESSAGES    = 1000
    #\}

    ## \name Statuses
    #\{
    _RUNNING        = "Running"
    _SUBMITTED      = "Submitted"
    _CLAIMED        = "Claimed"
    _ERROR          = "Error"
    _FINISHED       = "Finished"
    ## \brief
    #  The order of the status constants list ir orders according
    #  to what should display highest on the list.
    _STATUSES       = [_RUNNING, _SUBMITTED, _CLAIMED, _ERROR, _FINISHED]
    #\}

    ## \name Programs
    #\{
    _ALCAPANA       = "alcapana"
    _ROOTANA        = "rootana"
    _PROGRAMS       = [_ALCAPANA, _ROOTANA]
    #\}

    ## \brief
    #  Initializes the curses enviroment.
    def __init__(self):
        self.screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        self.screen.keypad(1)
        self.header = ["PPPPPPPP |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| Quota QQQ%",
                       "--------------------------------------------------------------------------------",
                       "   Run   |    Status    |         Download Progress         |    Run Time    ---",
                       "--------------------------------------------------------------------------------"]
        self.runs        = []
        self.start       = {}
        self.stop        = {}
        self.status      = {}
        self.progress    = {}
        self.downloading = 0
        self.messages    = []
        self.print_msg   = -1
        return

    ## \brief
    #  Sets the program to be printed in upper-left corner.
    #  Raises if the program isn't recognized.
    #
    #  \param[in] prog Program this screen is for. Either "alcapana"
    #  or "rootana".
    def SetProgram(self, prog):
        if prog == ScreenManager._ALCAPANA:
            self.header[0] = self.header[0].replace(self.header[0][0:8], "ALCAPANA")
        elif prog == ScreenManager._ROOTANA:
            self.header[0] = self.header[0].replace(self.header[0][0:8], "ROOTANA")
        else:
            raise UnknownProductionError(prog)
        self.Draw()
        return

    ## \brief
    #  Start a run on the display
    #
    #  \param[in] run The run number to display.
    def AddRun(self, run):
        self.runs.append(run)
        self.start[run] = None
        self.stop[run] = None
        self.status[run] = ScreenManager._CLAIMED
        self.progress[run] = [None, None, ""]
        self.Draw()
        return

    ## \brief
    #  Remove a run from being displayed.
    #
    #  \param[in] eun The run number to remove from the display.
    def RemoveRun(self, run):
        del self.runs[self.runs.index(run)]
        del self.start[run]
        del self.stop[run]
        del self.status[run]
        del self.progress[run]
        return

    ## \brief
    #  Mark a run as completed and reduce its display priority.
    def FinishRun(self, run):
        self.status[run] = ScreenManager._FINISHED
        self.stop[run] = datetime.datetime.now()
        self.Draw()
        return

    ## \brief
    #  Update the displayed job status of jobs that have been
    #  submitted to the grid.
    #
    #  \param[in] jobs A dictionary of run numbers to SGEJobs.
    def UpdateStatus(self, jobs):
        for run, job in jobs.iteritems():
            if job.HasError():
                self.status[run] = ScreenManager._ERROR
            elif job.IsRunning():
                if self.status[run] != ScreenManager._RUNNING:
                    self.start[run] = datetime.datetime.now()
                self.status[run] = ScreenManager._RUNNING
            elif job.IsWaiting():
                self.status[run] = ScreenManager._SUBMITTED
        return

    def StartProgress(self, run, size):
        self.downloading = run
        self.progress[run] = [0, size, "[                    ]   0%      "]
        self.Draw()
        return

    def UpdateProgress(self, size):
        run = self.downloading
        self.progress[run][0] += size
        pct = int(100.*float(self.progress[run][0])/float(self.progress[run][1]))
        bar = ""
        for i in range(pct / 5):
            bar += "="
        self.progress[run][2] = self.progress[run][2].replace(self.progress[run][2][1:21], "%-20s" % bar)
        self.progress[run][2] = self.progress[run][2].replace(self.progress[run][2][23:27], "%3d%%" % pct)
        self.Draw()
        return

    def FinishProgress(self):
        self.progress[self.downloading][2] = "[====================] 100% Done!"
        self.downloading = 0
        self.Draw()
        return

    def UpdateQuota(self, q):
        self.header[0] = self.header[0].replace(self.header[0][-4:-1], "%3d" % int(100.*q))
        self.Draw()
        return

    def Draw(self):
        y = 0
        now = datetime.datetime.now()
        self.SortByStatus()
        for line in self.header:
            self.screen.move(y,0)
            self.screen.addstr(line)
            y += 1
        for run in self.runs[:19]:
            if self.start[run]:
                if self.stop[run]:
                    dt = (self.stop[run] - self.start[run]).seconds
                else:
                    dt = (now - self.start[run]).seconds
                m, s = dt/60, dt % 60
            else:
                m, s = 0, 0

            self.screen.move(y, 0)
            self.screen.addstr("%7d  |" % run)
            self.screen.addstr("%13s |" % self.status[run])
            self.screen.addstr(" %-33s |" % self.progress[run][2])
            self.screen.addstr(" %9dm %2ds " % (m, s))
            y += 1
        for i in range(ScreenManager._MAXLINES - 1 - len(self.header) - len(self.runs)):
            self.screen.move(y,0)
            self.screen.addstr("%80s" % "")
            y += 1
        self.screen.move(y,0)
        self.screen.addstr("%-80s" % self.messages[self.print_msg])
        self.screen.refresh()
        return

    def Message(self, msg):
        self.print_msg = -1
        self.messages.append(str(datetime.datetime.now())[:19] + ": " + msg)
        if len(self.messages) > ScreenManager._MAXMESSAGES:
            self.messages.pop(0)
        return

    def SortByStatus(self):
#        def sort(x):
#            self.Message(self.status[x])
#            return ScreenManager._STATUSES.index(self.status[x])
#        self.runs.sort(key=sort)
        self.runs.sort(key=lambda run: ScreenManager._STATUSES.index(self.status[run]))
        return

    def Close(self):
        curses.nocbreak()
        self.screen.keypad(0)
        curses.echo()
        curses.endwin()
        for msg in self.messages:
            print msg
        return

## \brief
#  A dummy class so all calls to ScreenManager will work, but do nothing
#  (except print for Message call).
class Dummy():
    def __init__(self):
        pass
    def SetProgram(self, prog):
        pass
    def AddRun(self, run):
        pass
    def RemoveRun(self, run):
        pass
    def FinishRun(self, run):
        pass
    def UpdateStatus(self, jobs):
        pass
    def StartProgress(self, run, size):
        pass
    def UpdateProgress(self, size):
        pass
    def FinishProgress(self):
        pass
    def UpdateQuota(self, q):
        pass
    def Draw(self):
        pass
    def Message(self, msg):
        print msg
        pass
    def SortByStatus(self):
        pass
    def Close(self):
        pass
