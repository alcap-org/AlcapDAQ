#!/usr/bin/python

# Look through:
# /Equipment:
#   EB:
#     Settings: Number of Fragment (INT), Fragment Required BOOL[4]
#
# /Clients: If /System/Clients/####/??? exists. Only in old ODBs...

import sys
import os.path
import cPickle
import xml.etree.cElementTree as ET
from array import array
from ROOT import TFile, TTree

def findkey(root, keypath):
    dirs = keypath.lstrip('/').split('/', 1)
    for node in root.findall('*'):
        if node.attrib['name'].startswith(dirs[0]):
            if node.tag == 'dir':
                return findkey(node, dirs[1])
            elif node.tag == 'key':
                return node
            
def keyval(key):
    dtype = key.attrib['type']
    if dtype in ['BYTE', 'WORD', 'DWORD', 'INT']:
        return int(key.text)
    elif dtype in ['FLOAT', 'DOUBLE']:
        return float(key.text)
    elif dtype == 'BOOL':
        return True if key.text == 'y' else False
    elif dtype == 'STRING':
        return key.text


########## MAIN ##########
keyname = ['Run']
odbkeys = []
for cfgfname in sys.argv[1:]:
    for l in open(cfgfname):
        if l.rstrip():
            name, key = l.split(None, 1)
            keyname.append(name)
            odbkeys.append(key.rstrip())
trends  = [[]]
trends  += [[] for i in xrange(len(odbkeys))]

for run in xrange(11000):
    print 'Run '+str(run)+'...'
    fname = 'xml/run{0:05d}.xml'.format(run)
    if not os.path.isfile(fname):
        continue
    trends[0].append(run)
    odb = ET.parse(fname)
    for trend, keypath in zip(trends[1:], odbkeys):
        key = findkey(odb, keypath)
        if key is None:
            raise LookupError('Cannot find ODB key ' + keypath)
        trend.append(keyval(key))

ofile = TFile('odbtrends.root', 'RECREATE')
tr    = TTree('odbtrend', 'Trends from ODB')
bradd = []
for key, trend in zip(keyname,trends):
    if type(trend[0]) is int or type(trend[0]) is bool:
        bradd.append(array('l', [0]))
        tr.Branch(key, bradd[-1], key+'/I')
    elif type(trend[0]) is float:
        bradd.append(array('f', [0]))
        tr.Branch(key, bradd[-1], key+'/F')
        print "type float okay"
for i in xrange(len(trends[0])):
    for cbradd, ctrend in zip(bradd, trends):
        cbradd[0] = ctrend[i]
    tr.Fill()

ofile.Write()
ofile.Close()
cPickle.dump([dict(zip(keyname,trends)),odbkeys],open('trends.p','wb'))
