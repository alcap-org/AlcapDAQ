#! /bin/bash

function Usage(){
cat <<EOF

Usage: $0 [-e] [-d] [-g GroupName] <libname> <TestDescription>

  Generates the boiler plate for a set of unit tests.  The output 
  Positional arguments:
    <libname>  The base for the output file name, which will be of the
        form "<libname>_test.cpp".  Typically this would be the class 
        name, or maybe a namespace holding free functions. 

    <TestDescription> The brief description printed when the tests are 
        run.  This is usually something like "MyClass tests". Quotes are 
        required.

  Optional arguments:
    -e  Expert mode: Don't generate explanatory comments in the file.
    -d  Dry run:  echo the resulting file to stdout.
    -g  Set the test group name. This should be unique. If the option is 
        not given, to_upper(<libname>) is used instead.
EOF
}


function check_type_name(){
    echo "$*" | grep -q ";"
    if [ $? -eq 0 ]; then 
        echo "ERROR: Bad C++ identifier \"$*\" (';' not allowed)" >&2 
        exit 1
    fi
    
    local tmpfile="/tmp/check_type_$RANDOM.h"
    echo "struct $* {};" > $tmpfile
    g++ -c $tmpfile 1> /dev/null 2>&1
    if [ $? -ne 0 ]; then
        rm -f $tmpfile
        echo "ERROR: Bad C++ identifier \"$*\" (will not compile)" >&2
        exit 1
    fi
    rm $tmpfile
}

while getopts ":edg:" OPTION; do
    case $OPTION in
        e)
            expert="YES"
            ;;
        d)
            dryrun="YES"
            ;;
        g)
            groupname=$OPTARG
            ;;
        ?)
            echo "Unknown parameter \"$OPTARG\"" >&2
            echo 
            Usage
            exit 1
            ;;
    esac
done
shift $(($OPTIND-1))

[ "$1" = "--" ] && shift

if [[ $# -ne 2 ]]; then
    Usage
    exit 1
fi

libname=$1
descript=$2

wd=$(readlink -f `dirname $0`)


### Set the output location
if [[ -z $dryrun ]]; then
    outdir=$(dirname $wd)
    outdir="${outdir}/src/unit_test/"
    outname="${libname}_test.cpp"
    if [[ -e ${outdir}${outname} ]]; then
        echo "ERROR: ${outname} already exists in rootana/src/unit_test" >&2
        exit 1
    fi
    echo "Output to file $outname in rootana/src/unit_test"
    outname="${outdir}${outname}"    
else
    outname=/dev/fd/1
fi

### Set the group name
if [[ -z $groupname ]]; then
    groupname=`echo "$libname" | tr 'a-z' 'A-Z'` 
fi
check_type_name $groupname


### Sanitise the descriptive text for sed
descript=`echo "$descript" | tr '"' "'"`
grep -q "^" <<< $descript || delim=^ 
grep -q "%" <<< "$descript" || delim=% 
grep -q "#" <<< "$descript" || delim=# 

if [[ -z $delim ]]; then
    echo "WARNING: Could not find an appropriate delimiter for \"$descript\"" >&2
    echo "  forcing '#' to '_' (you can edit this later)" >&2
    descript=`echo "$descript" | tr '#' '_'`
    delim="#"
fi

### Select how we deal with comments
if [[ -z $expert ]]; then
    comm='sed s!//C!//!g'
else
    comm='grep -v //C'
fi

### Do the output
cat "${wd}/tut_boilerplate.cpp" | $comm | sed \
    -e s${delim}"TEST_DESCRIPTION"${delim}"$descript"${delim}g \
    -e s/"GROUP_NAME"/$groupname/g   \
    > $outname

exit 0