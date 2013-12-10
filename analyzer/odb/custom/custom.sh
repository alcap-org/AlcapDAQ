#! /bin/bash

########################################
# Define important variables
########################################
OutputFile="custom.html"
TopControlButtonTitles=( Status Start Pause Stop ODB Messages )
TopControlButtonSpans=( 4 4 4 4 4 4 4 )

ColumnTitles=( \
"Detector" \
"Bank Name" \
"Sampling Frequency" \
"Trigger Polarity" \
"Pedestal" \
"DCM Phase" \
"Trigger Mask" \
"Presamples" \
"Stretch Sample" \
"Threshold Low" \
"Threshold High" \
"Board/Channel Enabled?" \
)

NumBanks=48
FadcPaths=( "Crate 9/Settings/NFADC 8"{0..3} )
FadcChannels=( "Channel "{0..7} )
Crate4Paths=( "Crate 4/Settings/CAEN0" )
Crate4Channels=( "Ch"{0..7} )
Crate5Paths=( "Crate 5/Settings/CAEN" )
Crate5Channels=( "Ch0"{0..3} )
EmptyBanks=4

BankOptions=( \
"DCM phase" \
"Trigger mask" \
"Presamples" \
"Stretch samples" \
"Lower threshold" \
"Upper threshold" \
"Enabled" 
)

# Sets up the editable values
# 1 and 0 are as needed by ODB
# if value is 2 then we skip this option altogether for the entire board
# if value is 3 then we skip this option altogether for all channels other than the first
  FadcEdit=( 1 1 1 1 1 1 3 )
Crate4Edit=( 2 2 2 2 2 2 1 )
Crate5Edit=( 2 2 2 2 2 2 2 )

AllBanks=( "DetectorName" "BankName" "SamplingFrequency" "TriggerPolarity" "Pedestal" )
AllBanksEdit=( 1 0 0 1 1 )

########################################
# Define important functions
########################################
function ClearFile(){
cat /dev/null >"$OutputFile"
}
function PrintToFile(){
cat >> "$OutputFile"
}
function BeginRow(){
PrintToFile <<< "<tr>"
}
function EndRow(){
PrintToFile <<< "</tr>"
echo |PrintToFile
}

function CreateTopMatter(){
# Create all the html top-matter
PrintToFile <<EOF
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Mozilla/4.5 [en] (X11; U; Linux 2.0.34 i686)
[Netscape]">
   <title>Wiremap summary</title>
</head>
<body>

<h1>
<u>Wiremap Summary</u></h1>

<form name="form1" method="Get" action="/CS/DetectorBankMap">

<input type=hidden name="redir" value="Test">

<h3>Editable values hard-linked to bank name.  
After switching channels, please edit Detector field.</font><h3>

EOF
}

function StartTable(){
# Create the header row for the table
PrintToFile <<<'<table border="6" width="640">'
BeginRow
count=0
for Title in ${TopControlButtonTitles[@]}; do 
count=$((count+1))
PrintToFile<<EOF
<th colspan="${TopControlButtonSpans[count]}" align="center"><input name="cmd" value="${Title}" type="submit"></th>
EOF
done
EndRow
}

function MakeHeaderRow(){
BeginRow
count=0
for Header in "${ColumnTitles[@]}"; do 
count=$((count+1))
PrintToFile<<EOF
<td colspan="2" align="center">${Header}</td>
EOF
done
EndRow
}

function CreateRow(){
BankID="$1"; shift
Channel="$1"; shift
BankPath="$1" ;shift
ChannelPath="$1" ;shift
OptionEdits=( $@ )
#for Option in ${BankOptions[@]};do
#       OptionEdits[${#OptionEdits[@]}]="$1"
#       shift
#done

BeginRow
# Make the columns that apply to all banks
count=0
for Value in "${AllBanks[@]}";do
PrintToFile <<EOF
<td colspan="2" align="center"><odb src="/Analyzer/WireMap/${Value}[${BankID}]" edit=${AllBanksEdit[$count]}></td>
EOF
count=$((count+1))
done

# make sure we have a bank path (don't do the rest for empty banks)
if [ -n "$BankPath" ];then

        # Now make the columns that apply to only some banks based on the values passed
        # to this function
        i_count=0
        for Option in "${BankOptions[@]}";do
        echo -n '<td colspan="2" align="center">' |PrintToFile 
        if [ "${OptionEdits[$i_count]}" -lt 2 ] ;then
                 echo -n '<odb src="'"$BankPath"'/'"$ChannelPath"'/'"$Option"'" edit='"${OptionEdits[$i_count]}"'>'|PrintToFile
         elif [ "${OptionEdits[$i_count]}" -eq 3 -a $Channel -eq 0 ]; then
                 echo -n '<odb src="'"$BankPath"'/'"$Option"'" edit=1>'|PrintToFile
        fi
        echo '</td>' |PrintToFile 
        i_count=$((i_count+1))
        done
fi
EndRow
}

function MainBody(){
Bank=0
# Loop over FADC
for  Path in "${FadcPaths[@]}"; do
        ChannelID=0
        for Channel in "${FadcChannels[@]}";do
                FullPath="/Equipment/${Path}"
                echo CreateRow $Bank $FullPath $Channel ${FadcEdit[@]}
                CreateRow "$Bank" $ChannelID "$FullPath" "$Channel" ${FadcEdit[@]}
                Bank=$((Bank+1))
                ChannelID=$((ChannelID+1))
        done
done
# Fill the Crate 4 bankse
for  Path in "${Crate4Paths[@]}";do
        ChannelID=0
        for Channel in "${Crate4Channels[@]}";do
                FullPath="/Equipment/${Path}"
                echo CreateRow "$Bank" $ChannelID "$FullPath" "$Channel" ${Crate4cEdit[@]}
                CreateRow "$Bank" $ChannelID "$FullPath" "$Channel" ${Crate4Edit[@]}
                Bank=$((Bank+1))
                ChannelID=$((ChannelID+1))
        done
done
# Fill the Crate 5 banks
for  Path in "${Crate5Paths[@]}";do
        ChannelID=0
        for Channel in "${Crate5Channels[@]}";do
                FullPath="/Equipment/${Path}"
                echo CreateRow $Bank $FullPath $Channel ${Crate5Edit[@]}
                CreateRow $Bank $ChannelID "$FullPath" "$Channel" ${Crate5Edit[@]}
                Bank=$((Bank+1))
                ChannelID=$((ChannelID+1))
        done
done
# Fill the empty banks
for  ((ChannelID=0;ChannelID<$EmptyBanks;ChannelID+=1));do
        echo CreateRow $Bank 
        CreateRow $Bank 
        Bank=$((Bank+1))
done
}

function EndTable(){
# Create the last tags for the table
PrintToFile <<<"</table>"
}

function CreateBottomMatter(){
# Create the last tags for the table
PrintToFile <<EOF
</form>
</body>
</html>
EOF
}

########################################
# Main Body
########################################
ClearFile
CreateTopMatter
StartTable
MakeHeaderRow
MainBody
EndTable
CreateBottomMatter
