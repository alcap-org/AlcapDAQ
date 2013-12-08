#!/usr/bin/python
import os,sys
import MySQLdb

import tempfile
os.environ['MPLCONFIGDIR'] = tempfile.mkdtemp()
import matplotlib as mpl
mpl.use('Agg')

import matplotlib.pyplot as plt
import cgi

xaxis = []
yaxis = []
query = ['run6_slow_HV','timestamp','measured',''] #default table, x-axis, y-axis, channel name
limit = 1000000 #default limit
left = -24 # hours relative to current time
right = 0 # hours relative to current time

# Load parameters from URL
args = cgi.FieldStorage()
if 'limit' in args.keys():
    limit = args['limit'].value

if 'table' in args.keys():
    query[0] = str(args['table'].value)

if 'xaxis' in args.keys():
    query[1] = str(args['xaxis'].value)
    
if 'yaxis' in args.keys():
    query[2] = str(args['yaxis'].value)

if 'name' in args.keys():
    query[3] = str(args['name'].value)

if 'left' in args.keys():
    left = args['left'].value

if 'right' in args.keys():
    right = args['right'].value

    

# Open database connection (host, username, password, database)
db = MySQLdb.connect("localhost","musun","$mucap$","musun")

# Prepare a cursor object using cursor() method
cursor = db.cursor()

# Prepare SQL query to retrieve a record from the database.
sql = 'SELECT '+query[1]+','+query[2]+' FROM '+query[0]+' WHERE ';
if query[3] != '':
    sql += 'name=\''+query[3]+'\' and';

sql += ' timestamp>=NOW()+INTERVAL ' + str(left) + ' HOUR and timestamp <=NOW()+ INTERVAL ' + str(right) + ' HOUR ORDER BY '+query[1]+' DESC LIMIT '+str(limit)
sys.stderr.write(sql)

# Execute the SQL command
cursor.execute(sql)

# Fetch all the rows in a list of lists.
results = cursor.fetchall()

# Supply voltage
supply = 5.05

for row in results:
    xaxis.append(row[0])
    if query[3] in ('muSR B-field 1', 'muSR B-field 2', 'Ext. B-field x', 'Ext. B-field y', 'Ext. B-field z'):
        yaxis.append((row[1]-supply/2)/(.0003*supply-.00016))
    elif query[3] in ('Ext. humidity', 'ePC humidity'):
        yaxis.append((row[1]-.826)/.0315)
    else:
        yaxis.append(row[1])

# Disconnect from server
db.close()

# Generate the plot
plt.plot(xaxis, yaxis, marker='o', linestyle='-', color='r')
plt.xlabel(query[1])
plt.ylabel(query[2])
plt.title(query[3] + ": " + query[2]+' vs. '+query[1])
plt.setp(plt.xticks()[1], rotation=30)
if query[2]=='demand' or query[2]=='measured':
    plt.ylim(0,2000)
if query[2]=='current':
    plt.ylim(0,2000)
[content, filename] = tempfile.mkstemp(suffix='.png')
os.close(content)
plt.savefig(filename, bbox_inches='tight')

# Read the png back into Apache
content = open(filename, 'r').read()
os.remove(filename) #clean up
print "Content-type: image/png\n"
print content
