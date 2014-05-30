#!/usr/bin/python

import sys

wm_key = '[/Analyzer/WireMap]\n'
bank_key = 'BankName'
det_key = 'DetectorName'

if len(sys.argv) != 2:
    print 'check_detmap: Takes the ODB file as its only argument'
    sys.exit(0)

odb = open(sys.argv[1], 'r')
det = []
chn = []

wm_found = False
for ikey in odb:
    if ikey == wm_key:
        break

if ikey == '\n':
    print "ERROR: WireMap not found in ODB!"
    sys.exit(1)

in_banks = False
in_dets = False
for ikey in odb:
    if ikey[0] == '\n' or ikey[:2] == '[/':
        break
    elif ikey[0] != '[':
        in_banks = False
        in_dets = False

    if ikey[:len(bank_key)] == bank_key:
        in_banks = True
        in_dets = False
        continue
    elif ikey[:len(det_key)] == det_key:
        in_banks = False
        in_dets = True
        continue

    if in_banks:
        chn.append(ikey[4:-1])
    elif in_dets:
        det.append(ikey[5:-1])

nbanks = len(chn)
ndets = len(det)
if nbanks != ndets:
    print "ERROR: We have " + str(nbanks) + " channels but " + str(ndets) + " detectors!"
    sys.exit(2)
elif nbanks == 0:
    print "ERROR: No detectors or channels found!"
    sys.exit(3)

table = '{0:4} <-> {1:20}'
print table.format('Bank','Detector')
for i in range(0,nbanks):
    if det[i] == 'blank':
        continue
    print table.format(chn[i],det[i])

print "(Detectors named 'blank' omitted.)"
