#!/bin/sh

if [ $# -lt 1 ]; then
  echo "Usage: $0 PATH\n"
  echo "You must specify the path where MODULES.h should be created."
  echo "For example, to make ../src/MODULES.h, use "
  echo "  $0 ../src/"

  exit 1
fi

OUTFILE=$1/MODULES.h

rm -f ${OUTFILE}
while read module; do 
  [ -z "$module" ] &&continue;
  module="`tr -d '\r\n' <<<$module`"
  if [[ "$module" == '#'* ]];then 
          echo "Ignoring: ${module}"
          continue
  fi
  basename=`basename $module`
  echo "extern ANA_MODULE ${basename}_module;" >> ${OUTFILE};
done < MODULES
echo >> ${OUTFILE}
echo "ANA_MODULE *trigger_module[] = {" >> ${OUTFILE}
while read module; do 
  [ -z "$module" ] &&continue;
  module="`tr -d '\r\n' <<<$module`"
  if [[ "$module" == '#'* ]];then 
     continue
  fi
    basename=`basename $module`
    echo "  &${basename}_module," >> ${OUTFILE}; 
done < MODULES
echo "NULL };" >> ${OUTFILE}
