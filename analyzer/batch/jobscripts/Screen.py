import SGEJob
import AlCapExceptions

import datetime
import curses

class Screen:

    _HEADER_LINES   = 4
    _MAX_RUNS       = 20
    _RUN_WIDTH      = 9
    _STATUS_WIDTH   = 14
    _DOWNLOAD_WIDTH = 31
    _TIME_WIDTH     = 16

    _TIMEOUT        = 5
    _MAXMESSAGES    = 1000

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

    def SetProgram(self, prog):
        if prog == "alcapana":
            self.header[0] = self.header[0].replace(self.header[0][0:8], "ALCAPANA")
        elif prog == "rootana":
            self.header[0] = self.header[0].replace(self.header[0][0:8], "ROOTANA")
        else:
            raise UnknownProgramError(prog)
        self.Draw()
        return

    def AddRun(self, run):
        self.runs.append(run)
        self.start[run] = None
        self.stop[run] = None
        self.status[run] = "Claimed"
        self.progress[run] = [None, None, ""]
        self.Draw()
        return

    def RemoveRun(self, run):
        del self.runs[self.runs.index(run)]
        del self.start[run]
        del self.stop[run]
        del self.status[run]
        del self.progress[run]
        return

    def FinishRun(self, run):
        self.status[run] = "Finished"
        self.stop[run] = datetime.datetime.now()
        self.Draw()
        return

    def UpdateStatus(self, jobs):
        for run, job in jobs.iteritems():
            if job.HasError():
                self.status[run] = "Error"
            elif job.IsRunning():
                if self.status[run] != "Running":
                    self.start[run] = datetime.datetime.now()
                self.status[run] = "Running"
            elif job.IsWaiting():
                self.status[run] = "Submitted"
        return

    def StartProgress(self, run, size):
        self.downloading = run
        self.status[run] = "Downloading"
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
        for line in self.header:
            self.screen.move(y,0)
            self.screen.addstr(line)
            y += 1
        for run in self.runs:
            if self.start[run]:
                if self.stop[run]:
                    dt = (self.stop[run] - self.start[run]).seconds
                    if (datetime.datetime.now() - self.stop[run]).seconds > Screen._TIMEOUT:
                        self.RemoveRun(run)
                        self.Draw()
                        return
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
        for i in range(Screen._MAXLINES - 1 - len(self.header) - len(self.runs)):
            self.screen.move(y,0)
            self.screen.addstr("%80s" % "")
            y += 1
        self.move.screen(y,0)
        self.screen.addstr(self.messages[self.print_msg])
        self.screen.refresh()
        return

    def Message(self, msg):
        self.print_msg = -1
        self.messages.append(str(datetime.datetime.now())[:19] + ": " + msg)
        if len(self.messages) > Screen._MAXMESSAGES:
            self.messages.pop(0)
        return
