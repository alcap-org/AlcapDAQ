#! /bin/bash

ListOfSpecials=( module {TAP,TDP,TME}_generator )
Unimplemented=( {TDP,TME}_generator )
ScriptDirectory="$(readlink -f `dirname $0`)"
TemplateDirectory="$ScriptDirectory/templates"

#Usage: CreateComponent.sh ModuleName [TargetDirectory]
Indent=`printf "\n\t\t\t"`
PrintedSpecials=`printf "%s${Indent}"  "${ListOfSpecials[@]} "`
function Usage(){
#-----------------------------------------------------------------------------80
cat <<EOF

Usage: $0 Name [Type] [TargetDirectory]
  Script to create a set of skeleton files to implement a new module or 
generator

  ModuleName      = The name of the new component.
  Type (optional) = Make a special type of module.  
                    Must be one of: ${Indent}${PrintedSpecials}
                    Default is '${ListOfSpecials[0]}'.
  TargetDirectory = Specify a target directory (relative to rootana) to place 
  (optional)        the new files.  Default is src or one of the *_generators 
                    directories if 'type==*generator'
EOF
}

if [ $# -lt 1 ];then
        Usage
        exit 1
fi

ModuleName="$1"

# Decide the specialisation
Specialisation=""
RequestedSpecial="`tr 'A-Z' 'a-z' <<< $2`"
if [[ -n $2 ]];then
    for i in ${ListOfSpecials[@]}; do
        i="`tr 'A-Z' 'a-z' <<< $i`"
        [[ "${RequestedSpecial}" == ${i} ]] && Specialisation="${RequestedSpecial}" && break
    done
    if [ -z "$Specialisation" ];then
        echo "ERROR: Requested specialisation, '$RequestedSpecial' is not one of: ${ListOfSpecials[@]}"
	echo
        Usage
        exit 2
    fi
    for i in ${Unimplemented[@]}; do
        i="`tr 'A-Z' 'a-z' <<< $i`"
        if [[ "${RequestedSpecial}" == ${i} ]] ;then
		echo "Error: Requested specialisation '$RequestedSpecial' is not implemented yet."
		echo "       Contact Ben Krikler if it's urgent" 
		exit 1
	fi
    done
fi
[ -z "$Specialisation" ] && Specialisation="${ListOfSpecials[0]}"

# Check ModuleName is valid C++ which we need as we use this for the class
# names (probably an inexhaustive check)
BadChars="\.-+\& "
if [[ "$ModuleName" == *[${BadChars}]* ]];then
        echo "ERROR: The component name you have supplied contains illegal characters: '$ModuleName'"
        echo "       The name must be useable as a c++ variable name."
        echo "       It shouldn't contain any of: $BadChars"
        exit 3
fi

TargetDirectory=""
#Make TargetDirectory variable
if [ -n "$3" ];then
        TargetDirectory=$3
else
        # Find the directory containing the templates. We assume it is ../src 
        # to the one that contains this script
        TargetDirectory="`readlink -f "$ScriptDirectory"/../src`"
       if [[ "$Specialisation" == tap* ]];then
	       TargetDirectory="$TargetDirectory/TAP_generators"
       elif [[ "$Specialisation" == tdp* ]];then
	       TargetDirectory="$TargetDirectory/TDP_generators"
       elif [[ "$Specialisation" == tme* ]];then
	       TargetDirectory="$TargetDirectory/TME_generators"
       fi
fi

#Check TargetDirectory exists
if [ ! -d "$TargetDirectory" ];then
        echo "ERROR: TargetDirectory, $TargetDirectory doesn't exist"
        exit 4
fi

#Make sure we capitalise the first letter of the module
MODULE_NAME="`tr 'a-z' 'A-Z' <<<${ModuleName}`"
ModuleName=${MODULE_NAME:0:1}${ModuleName:1}

# Setup the Output file name
OutFilename="${ModuleName}"
if [[ "$Specialisation" == *_generator ]];then
      if [[ "$Specialisation" == tap* ]];then
	      OutFilename="${OutFilename}AP"
      elif [[ "$Specialisation" == tdp* ]];then
	      OutFilename="${OutFilename}DP"
      elif [[ "$Specialisation" == tme* ]];then
	      OutFilename="${OutFilename}ME"
      fi
      OutFilename="${OutFilename}Generator"
fi

InFilename="Template_${Specialisation}"
ListOfExtensions=( .{h,cpp} )
for Extension in ${ListOfExtensions[@]};do
    InFile=${InFilename}${Extension}
    OutFile=${OutFilename}${Extension}
    echo $InFile $OutFile
    #Check if OutFile exists
    if [ -e "$TargetDirectory/$OutFile" ] ;then
        echo "$OutFile already exists, so skip it..."
        continue;
    fi
    sed -e 's!%module%!'$ModuleName'!g' -e 's!%MODULE%!'$MODULE_NAME'!g' "$TemplateDirectory/$InFile" >"$TargetDirectory/$OutFile"
done
