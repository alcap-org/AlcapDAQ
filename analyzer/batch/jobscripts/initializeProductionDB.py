#!/usr/bin/python
import sqlite3
import datetime

irange = lambda start, end: range(start, end + 1)

datasets = { 'Al100' : [ irange(9410, 9412),   irange(9494, 9505),
                         irange(9506, 9515),   irange(9516, 9518),
                         irange(9519, 9555),   irange(9559, 9585),
                         irange(9594, 9641),   irange(9655, 9682)   ],
             'Si16a' : [ irange(9735, 9743)   ],
             'Al50'  : [ irange(9889, 9996),   irange(9997, 10080),
                         irange(10087, 10129) ],
             'Ti50'  : [ irange(10157, 10206), irange(10208, 10254) ],
             'MT'    : [ irange(10272, 10309), irange(10310, 10317) ],
             'Si16b' : [ irange(10362, 10368), irange(10383, 10421),
                         irange(10425, 10450) ] }
geometry = { 'Al100' : 'P2',
             'Si16a' : 'S14',
             'Al50'  : 'P3',
             'Ti50'  : 'P4',
             'MT'    : 'C2',
             'Si16b' : 'P5' }

db = sqlite3.connect("productionR15b.db")

db.execute('CREATE TABLE R15bdatasets'
           '(run INT, dataset TEXT, tranche INT, quality TEXT, geometry TEXT)')

for ds, tranches in datasets.iteritems():
    for itr, tr in enumerate(tranches):
        for r in tr:
            db.execute("INSERT INTO R15bdatasets VALUES (?, ?, ?, ?, ?)",
                       (r, ds, itr+1, "gold", geometry[ds]))

db.commit()
