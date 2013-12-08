#!/usr/bin/python
import os,sys
import MySQLdb

import tempfile
os.environ['MPLCONFIGDIR'] = tempfile.mkdtemp()
import matplotlib as mpl
mpl.use('Agg')

import matplotlib.pyplot as plt
import cgi

interval = 15

args = cgi.FieldStorage()
if 'interval' in args.keys():
    interval = args['interval'].value


xlabels = []
for i in range(1,16):
	xlabels.append("IU{0:02d}".format(i)); 
	xlabels.append("ID{0:02d}".format(i)); 
	xlabels.append("OU{0:02d}".format(i)); 
	xlabels.append("OD{0:02d}".format(i)); 

for i in [3, 6, 11, 14]:
	xlabels.append("NU{0:02d}".format(i)); 
	xlabels.append("ND{0:02d}".format(i)); 

xaxis = []
yaxis = []



# Open database connection (host, username, password, database)
db = MySQLdb.connect("localhost","musun","$mucap$","musun")

# Prepare a cursor object using cursor() method
cursor = db.cursor()

# Prepare SQL query to retrieve a record from the database.
sql = 'SELECT name,measured-demand from run6_slow_HV where timestamp >= NOW() + INTERVAL -' + str(interval) + ' MINUTE' 

# Execute the SQL command
cursor.execute(sql)

# Fetch all the rows in a list of lists.
results = cursor.fetchall()

for row in results:
    if(row[0] in xlabels):
       xaxis.append(xlabels.index(row[0]) + 1)
       yaxis.append(row[1])

# Disconnect from server
db.close()

# Generate the plot
plt.figure(1, figsize=(15,3), dpi=100)
plt.plot(xaxis, yaxis, 'ro')
plt.xticks(range(1,len(xlabels)+1), xlabels, rotation='vertical')
plt.gca().grid(True)
plt.ylabel('Channel')
plt.ylabel('Measured - Demand')
plt.title('High voltage discrepancy monitor')
[content, filename] = tempfile.mkstemp(suffix='.png')
os.close(content)
plt.savefig(filename, bbox_inches='tight')

# Read the png back into Apache
content = open(filename, 'r').read()
os.remove(filename) #clean up
print "Content-type: image/png\n"
print content
