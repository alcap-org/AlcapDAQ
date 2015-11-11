#!/usr/bin/env python2

import serial
import time
import sys

vacuum = serial.Serial(port=0,baudrate=9600)
vacuum.write('COM,2\r\n')

try:
    start = time.time()
    while True:
        resp = vacuum.readline()
        if(len(resp)<10):
            continue
        p = float(resp[3:13])
        t = time.time() - start
        sys.stdout.write("%f    %f\n" % (p, t))
        sys.stdout.flush()
except KeyboardInterrupt:
    print "Goodbye..."
