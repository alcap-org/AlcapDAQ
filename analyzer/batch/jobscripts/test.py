import SGEJob
import Screen
import time
import os

jobs = {}
jobs[3100] = SGEJob.SGEJob(1234567, "qw", "alcapana")
jobs[3101] = SGEJob.SGEJob(1234568, "qw", "alcapana")
jobs[3102] = SGEJob.SGEJob(1234569, "qw", "alcapana")


s = Screen.Screen()
s.SetProgram("rootana")
s.AddRun(3100)
s.AddRun(3101)
s.AddRun(3102)
s.AddRun(3103)
time.sleep(1)
s.UpdateStatus(jobs)
time.sleep(2)
jobs[3100] = SGEJob.SGEJob(1234567, "r", "alcapana")
s.UpdateStatus(jobs)
s.Draw()
time.sleep(2)
s.Draw()
s.FinishRun(3100)
s.Draw()
time.sleep(7)
s.Draw()
time.sleep(1)
s.Draw()
time.sleep(2)
os.system("reset")
