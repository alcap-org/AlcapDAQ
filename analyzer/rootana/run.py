#!/usr/bin/env python
import os.path
import string
import subprocess
import re

MID_PATH='/home/nam/work/RunPSI2013/data/mid/'
RAWTREE_PATH='/home/nam/work/RunPSI2013/data/root/rawtree/'
ANALYSEDTREE_PATH='/home/nam/work/RunPSI2013/data/root/analysedtree/'
ALCAPANAHIST_PATH='/home/nam/work/RunPSI2013/data/root/alcapanahist/'
ROOTANAHIST_PATH='/home/nam/work/RunPSI2013/data/root/rootanahist/'


def process_run(runNo, timecut):
    """Process a run

    :runNo: @todo
    :returns: @todo

    """
    inputfile = RAWTREE_PATH + 'tree%05d.root' % runNo
    analysedtreeout = ANALYSEDTREE_PATH + \
            'anatree%05d_%dns.root' % (runNo,timecut)
    if os.path.isfile(analysedtreeout):
        print analysedtreeout + ' existed, skipping ...'
    else:
        command = ['./rootana', '-i', inputfile, '-o', analysedtreeout]
        subprocess.call(command)
    return

def process_all_runs(runlist_filename, timecut):
    """Fetch run list from a text file, and run
    """
    listfile = open(runlist_filename, 'r')
    runlist = [string.atoi(line.strip()) for line in listfile]
    for runNo in runlist:
        process_run(runNo, timecut)
    pass

def get_timecut():
    """@todo: Docstring for get_timecut
    :returns: @todo

    """
    src_filename = 'src/main.cpp'
    src_file = open(src_filename, 'r')
    lines = src_file.readlines()
    timecut = 1000
    for line in lines:
        match = re.search(r'double t1 = \d+;', line)
        if match:
            #print line
            #print re.findall(r'\b\d+', line)
            timecut = string.atoi(re.findall(r'\b\d+',line)[0])
            pass
        pass
    return timecut

###########
runlist_path = '../work/nam/'
runlist_file = 'Si16 passive_1.06_runs'

subprocess.call(['make'])
process_all_runs(runlist_path + runlist_file, get_timecut())
