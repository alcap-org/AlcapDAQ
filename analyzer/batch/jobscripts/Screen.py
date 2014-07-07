import SGEJob
import datetime


class Screen:

    _HEADER_LINES   = 4
    _MAX_RUNS       = 20
    _RUN_WIDTH      = 9
    _STATUS_WIDTH   = 14
    _DOWNLOAD_WIDTH = 31
    _TIME_WIDTH     = 16

    def __init__(self):
        self.screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        screen.keypad(1)
        self.header = ["PPPPPPPP |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| Quota QQQ%",
                       "--------------------------------------------------------------------------------",
                       "   Run   |    Status    |       Download Progress       |    Run Time    -------",
                       "--------------------------------------------------------------------------------"]
        self.runs        = []
        self.start       = {}
        self.status      = {}
        self.progress    = {}
        self.downloading = 0

    def StartProgress(self, run, size):
        pass
    def UpdateProgress(self, size):
        pass
    def FinishProgress(self):
        pass

    def Draw(self):
        y, x = 0
        for i in range(_HEADER_LINES):
            self.screen.move(y,x)
            self.screen.addstr(self.header[i])
            y += 1
        for i in range(len(runs)):
            x = 0
            run = self.runs[i]
            self.screen.move(y, x)
            self.screen.addstr("%7d  |" % run)
            self.screen.addstr("%13s |" % self.status[run])
            self.screen.addstr("%30s |" % self.progress[run])
            self.screen.addstr(" -----
