import sqlite3
import datetime

irange = lambda start, end: range(start, end + 1)

sir = irange(2091, 2103) + irange(2119, 2172)
al100 = (irange(2808, 2813) + irange(2826, 2873) + irange(2889, 2894) +
         irange(2897, 2905) + irange(2934, 2944) + irange(2963, 2992) +
         irange(2994, 2995) + irange(2997, 2999) + [3012])
al50a = (irange(3101, 3104) + irange(3110, 3117) + irange(3141, 3144) +
         irange(3146, 3149) + irange(3153, 3156) + irange(3160, 3167) +
         irange(3183, 3226) + irange(3238, 3257) + irange(3263, 3295) +
         irange(3322, 3324) + irange(3326, 3329))
common = irange(3200, 3204)
al50andet = irange(3442, 3456)
al50b = irange(3563, 3646) + [3650] + irange(3667, 3740)
si16p = irange(3474, 3489) + irange(3491, 3540)
sir21pct = [3771] + irange(3773, 3779)
sir23pct = irange(3763, 3770)


db = sqlite3.connect("production.db")

db.execute("CREATE TABLE datasets(run INT, dataset TEXT, quality TEXT)")
db.commit()

for i in sir:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "SiR2", "gold"))
for i in al100:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "Al100", "gold"))
for i in al50a:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "Al50awithoutNDet2", "gold"))
for i in common:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "Common", None))
for i in al50andet:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "Al50awithNDet2", "gold"))
for i in al50b:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "Al50b", "gold"))
for i in si16p:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "Si16P", "gold"))
for i in sir21pct:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "SiR21pct", "gold"))
for i in sir23pct:
    db.execute("INSERT INTO datasets VALUES (?, ?, ?)", (i, "SiR23pct", "gold"))
db.commit()

db.execute("CREATE TABLE productions(type TEXT, version INTEGER, start TIMESTAMP, stop TIMESTAMP, base INTEGER)")
start = datetime.datetime(2014, 4, 30, 0, 0, 0, 0)
stop = datetime.datetime(2014, 5, 5, 0, 0, 0, 0) 
db.execute("INSERT INTO productions VALUES ('alcapana', 1, ?, ?, 0)", (start, stop))
start = datetime.datetime(2014, 5, 29, 0, 0, 0, 0)
stop = datetime.datetime(2014, 6, 2, 0, 0, 0, 0)
db.execute("INSERT INTO productions VALUES ('alcapana', 2, ?, ?, 1)", (start, stop))
start = datetime.datetime(2014, 6, 16, 0, 0, 0, 0)
stop = datetime.datetime(2014, 6, 20, 0, 0, 0, 0)
db.execute("INSERT INTO productions VALUES ('alcapana', 3, ?, ?, 2)", (start, stop))
db.commit()
