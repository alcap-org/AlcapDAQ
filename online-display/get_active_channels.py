#!/bin/env python
from sys import argv
from os import listdir
from os.path import isfile, join, splitext
import pprint
import matplotlib.pyplot as plt
import re
import pickle
from datetime import datetime
import pprint

timestamp = []
runnumberarray = []
class AString(str):
  def getValue(self):
    return self[self.find(":")+1:-1].strip()

class ActiveChannel():
  def search(self, filename):
    dict = {}
    crates = ["Crate 1"]
    with open(filename) as f:
      for crate in crates:
        entry = ""
        for line in f:
          if re.match(r'\[/Runinfo\]', line):
            f.next()
            f.next()
            runnumber = AString(f.next() ).getValue()
            if not dict.get(runnumber):
              dict[runnumber] = {}
              runnumberarray.append(runnumber)
    pp = pprint.PrettyPrinter(depth=2)
    pp.pprint(dict)


if __name__ == '__main__':
  files = [f for f in listdir(argv[1]) if isfile(join(argv[1], f) )]
  for f in files:
    fileext = splitext(f)[1]
    if fileext == ".odb":
      print f + "\n"
      ActiveChannel().search(argv[1] + f)
