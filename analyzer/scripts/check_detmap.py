#!/usr/bin/python

import sys

wm_key = '[/Analyzer/WireMap]\n'
bank_key = 'BankName'
det_key = 'DetectorName'
freq_key = 'SamplingFrequency'

if len(sys.argv) != 2:
    print 'check_detmap: Takes the ODB file as its only argument'
    print '              Prints out some info from ODB.'
    sys.exit(0)

odb = open(sys.argv[1], 'r')
det = []
chn = []
frq = []

wm_found = False
for ikey in odb:
    if ikey == wm_key:
        break

if ikey == '\n':
    print "ERROR: WireMap not found in ODB!"
    sys.exit(1)

in_banks = False
in_dets = False
in_freqs = False
for ikey in odb:
    if ikey[0] == '\n' or ikey[:2] == '[/':
        break
    elif ikey[0] != '[':
        in_banks = False
        in_dets = False
        in_freqs = False

    if ikey[:len(bank_key)] == bank_key:
        in_banks = True
        in_dets = False
        inf_freqs = False
        continue
    elif ikey[:len(det_key)] == det_key:
        in_banks = False
        in_dets = True
        in_freqs = False
        continue
    elif ikey[:len(freq_key)] == freq_key:
        in_banks = False
        in_dets = False
        in_freqs = True
        continue

    if in_banks:
        chn.append(ikey.split(' ')[1][:-1])
    elif in_dets:
        det.append(ikey.split(' ')[1][:-1])
    elif in_freqs:
        frq.append(ikey.split(' ')[1][:-1])

nbanks = len(chn)
ndets = len(det)
nfrqs = len(frq)
if nbanks != ndets or nbanks != nfrqs:
    print "ERROR: We have " + str(nbanks) + " channels but " + str(ndets) + " detectors and " + str(nfrqs) + "frequencies!"
    sys.exit(2)
elif nbanks == 0:
    print "ERROR: No detectors or channels found!"
    sys.exit(3)

table = '{0:4} <-> {1:20} --- {2:10}'
print table.format('Bank','Detector','Frequency')
for i in range(0,nbanks):
    if det[i] == 'blank' or chn[i] == 'ZZZZ':
        continue
    print table.format(chn[i],det[i],frq[i])

print "(Detectors named 'blank' and banks names 'ZZZZ' omitted.)"
