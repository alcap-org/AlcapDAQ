#!/bin/python
# check_missing_runs.py
# -- goes through the production scripts and checks that the tree files exist
# -- assumes that your tree files are in ~/data/tree
# USAGE: . ./check_missing_runs.sh

import os;

USER="edmonds_a"

# Get the list of production scripts
production_run_number="run-3"
production_dir="production-"+production_run_number
for PRODUCTION_SCRIPT in os.listdir(production_dir):
    # Make sure it's the production script
    if (PRODUCTION_SCRIPT.find("alcapana-prod-"+production_run_number) == -1):
        continue;

    f=open(production_dir + "/" + PRODUCTION_SCRIPT, 'r');
#    print(PRODUCTION_SCRIPT)

    # Go through the lines in the script and get the ranges
    for line in f:
        arr=line.split()

        # Don't want empty lines or comment lines
        if len(arr) == 0:
            continue;        
        if (arr[0] == '#'):
            continue;

        # Now have the important parts so get the run tranches
        for element in arr:
            
            # First element is always the script submission name
            # next four are always "-n 20 -t 60" so we want to start from there
            if (arr.index(element) <= 4):
                continue;

            first_run = "0";
            last_run = "0";
            if (element == "-r"):
                first_run=arr[arr.index(element)+1];
                last_run=arr[arr.index(element)+2];
#                print("Checking for missing runs in ~/data/tree between " + first_run + " and " + last_run);
            else:
                first_run=arr[arr.index(element)];
                last_run=first_run
#                print("Checking in ~/data/tree for run " + first_run);

            for run in range(int(first_run), int(last_run)):
                filename="/gpfs/home/" + USER + "/data/tree/tree0" + str(run) + ".root"
                if os.path.isfile(filename) == False:
                    print(filename + " does not exist")
            


#FIRST_RUN=$1
#LAST_RUN=$2

#echo "Checking for missing runs in ~/data/tree"

#for i in `seq -f "%05g" $FIRST_RUN $LAST_RUN`
#do
#    FILENAME=~/data/tree/tree$i.root
#    if [ ! -f $FILENAME ] ; then
#	echo $FILENAME "is missing"
#    fi
#done
