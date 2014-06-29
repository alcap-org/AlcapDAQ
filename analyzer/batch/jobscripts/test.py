import sys
import getopt
from AlCapExceptions import *
#import os
#import time
#import datetime
#import merlin_utils
#import DBManager
#import RunManager
#import subprocess
#import make_test_table


#merlin_utils.remove_all_files(3200, "alcapana")
#merlin_utils.remove_all_files(3201, "alcapana")
#merlin_utils.remove_all_files(3202, "alcapana")
#merlin_utils.remove_all_files(3203, "alcapana")

#import run_production

def usage():
    print "Usage: ./run_production --production=PROD_TYPE [optional arguments...]"
    print "--------------------------------------------------------------------------------"
    print "This program checks the production database located at"
    print "$HOME/data/production.db and takes care of downloading, submitting,"
    print "and cleanup, and updates the database accordingly so multiple people can process"
    print "at the same time."
    print "--------------------------------------------------------------------------------"
    print "Required:"
    print "    --production=PROD_TYPE"
    print "                   The production type (alcapana or rootana)."
    print "Optional:"
    print "    -h, --help     Print this help then exit."
    print "    --usage        Print this help then exit."
    print "    --version=#    Version number to produce. Default is highest"
    print "                   production version number in production table."
    print "    --new          Start a new production, incrememted once from the"
    print "                   highest number present in productions table."
    print "    --pause=#      Wait # seconds between the event loop, which"
    print "                   checks for finished jobs and runs to download."
    print "                   (Default: 10)"
    print "    --nproc=#      Maximum number of processes to submit"
    print "                   to the grid simultaneously."
    print "                   (Default: 10)"
    print "    --nprep==#     Maximum number of MOIDAS files to have downloaded"
    print "                   in addition to the one running on the grid. This"
    print "                   means have, at most, NPROC + NPREP MIDAS files at"
    print "                   once."
    print "                   (Default: 5)"


short_args = "h"
long_args = ["usage", "help", "production=", "version=", "new", "pause=", "nproc=", "nprep="]
opts, unrecognized = getopt.getopt(sys.argv[1:], short_args, long_args)

# Default
production = None
version = None
new = False
wait = 10
nproc = 10
nprep = 5

for opt, val in opts:
    if opt == "h" or opt == "--help" or opt == "--usage":
        usage()
        exit(0)
    elif opt == "--production":
        production = val
    elif opt == "--version":
        version = int(val)
    elif opt == "--new":
        new = True
    elif opt == "--pause":
        wait = int(val)
    elif opt == "--nproc":
        nproc = int(val)
    elif opt == "--nprep":
        nprep = int(val)

# Argument checking
if len(unrecognized) > 0:
    msg = "Unrecognized arguments: " + unrecognized
    raise ArgumentError(msg)
if new and version:
    msg = "Version argument given and new production requested."
    raise ArgumentError(msg)
if (version < 0 or version > 100) and not new:
    msg = "Version argument less than zero or geater than 100."
    raise ArgumentError(msg)
if production not in ["alcapana", "rootana"]:
    msg = "Unknown production type: " + str(production)
    raise ArgumentError(msg)
# Remove this when rootana implemented
if production == "rootana":
    msg = "Rootana not implemented yet."
    raise ArgumentError(msg)
if wait <= 0 or wait > 60:
    msg = "Requested pause less than or equal to 0 or greater than a minute."
    raise ArgumentError(msg)
if nproc < 1 or nproc > 100:
    msg = "Number of processes shouldn't be less than 1 and maybe shouldn't be more than 100."
    raise ArgumentError(msg)
if nprep < 1 or nprep > 100:
    msg = "Number of preparations shouldn't be less than 1 and maybe shouldn't be more than 100."
    raise ArgumentError(msg)

print "Production:", production
print "Version:", version
print "New:", new
print "Pause:", wait
print "NPROC:", nproc
print "NPREP:", nprep
