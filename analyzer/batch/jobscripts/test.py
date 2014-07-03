import SGEJob

import os
import curses
import time

screen = curses.initscr()
curses.noecho()
curses.cbreak()
screen.keypad(1)

jobs = {}
jobs[3100] = SGEJob.SGEJob(1234567, "r", "batch_alca")
jobs[3101] = SGEJob.SGEJob(1234568, "E", "batch_alca")
jobs[3102] = SGEJob.SGEJob(1234569, "qw", "batch_alca")
jobs[3103] = SGEJob.SGEJob(1234570, "qw", "batch_alca")


def Title():
    screen.move(0,0)
    screen.addstr("Run Number | Status | Download Progress | Run Time")
    screen.move(1,0)
    screen.addstr("--------------------------------------------------")

def draw_runs(j):
    screen.move(3,0)
    for run, job in j.iteritems():
        pass

Title()
screen.refresh()
time.sleep(1)

os.system("reset")
