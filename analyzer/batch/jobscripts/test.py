import SGEJob

import os
import curses
import time

screen = curses.initscr()
curses.noecho()
curses.cbreak()
screen.keypad(1)

jobs = {}
jobs[3100] = SGEJob.SGEJob(1234567, "r", "alcapana")
jobs[3101] = SGEJob.SGEJob(1234568, "E", "alcapana")
jobs[3102] = SGEJob.SGEJob(1234569, "qw", "alcapana")
jobs[3103] = SGEJob.SGEJob(1234570, "qw", "alcapana")


fields = (1, 12, 21, 41)

def Title():
    screen.move(0,0)
    screen.addstr("Run Number | Status | Download Progress | Run Time")
    screen.move(1,0)
    screen.addstr("--------------------------------------------------")

def DrawRuns(j):
    y = 2
    for run, job in j.iteritems():
        y = y + 1
        screen.move(y, fields[0])
        screen.addstr(str(run))
        screen.move(y, fields[1])
        screen.addstr(job.status[0])

Title()
DrawRuns(jobs)
screen.refresh()
print "test"
time.sleep(2)

os.system("reset")
