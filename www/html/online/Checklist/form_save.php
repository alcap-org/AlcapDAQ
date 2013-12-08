<?php

$fields = array ("SHIFT_CRUE",
		 "SHIFT_NUMBER",
		 "SHIFT_DATE",
		 "SHIFT_DAYTIME",
		 "ACTUAL_RUN_NUMBER",
                 "ANALYZER_RUNNING",
		 "FE_LAZY_TAPE_ON",
		 "FE_LAZY_TAPE_1",
		 "FE_LAZY_TAPE_2",
		 "TAPE_1_LABEL",
		 "TAPE_2_LABEL",
		 "TAPE_1_FILE",
		 "TAPE_2_FILE",
		 "FE_WFD_ON",
		 "EB_ON",
		 "EB_EVENT_RATE",
		 "EB_DATA_RATE",
		 "MB_ON",
		 "EMC_ON",
		 "FE_BEAMLINE_ON",
		 "FE_ONEWIRE_ON",
		 "FE_HV_ON",
		 "FE_SCALER_ON",
		 "FE_SEPARATOR_ON",
		 "FE_COBRA_ON",
		 "FE_MLOGGER_ON",
		 "FE_MLOGGER_FILE",
		 "FE_MLOGGER_GB_TOTAL",
		 "FE_DAWN_ON",
		 "LAZY_TB06",
		 "LAZY_TB06_FILE",
		 "LAZY_PSI",
		 "LAZY_PSI_FILE",
		 "RAID_STATUS",
		 "RAID_STATUS_COMMENT",
		 "RAID_ALARM",
		 "RAID_ALARM_COMMENTS",
		 "KICKER_HV_MV1",
		 "KICKER_HV_MV2",
		 "KICKER_LED",
		 "KICKER_LED_COMMENT",
		 "SEPARATOR_HV",
		 "SEPARATOR_MAGNET",
		 "EMC_HV",
		 "EMC_CURRENT",
		 "EMC_CF4_PRESSURE",
		 "EMC_CF4_PRESSURE_DEMAND",
		 "EMC_C4H10_PRESSURE",
		 "EMC_C4H10_PRESSURE_DEMAND",
		 "EMC_FLOW_RATE",
		 "EMC_BUBBLE_RATE",
		 "GAS_ALARM_READY",
		 "LASER_N2_PRESSURE",
		 "LASER_N2_PRESSURE_DEMAND",
		 "LASER_ENABLED",
		 "LASER_ENABLED_RUNS",
		 "BEAM_CURRENT",
		 "SCALER_BALL_RATE",
		 "SCALER_TIME",
		 "SCALER_PROTONS",
		 "SCALER_EMC_XBUFF",
		 "EMC_BEAMSPOT_OK",
		 "EMC_BEAMSPOT_COMMENT",
		 "EMC_CURRENT_TARGET_OPEN",
		 "WFD_EARLY_START_RUNS",
		 "WFD_LONG_ISLAND_RUN_RUNS",
		 "WHATER_BATTLES_AREA_CHECKED",
		 "ONLINE_PLOTS_CHECK_MVP_VS_SN",
		 "ONLINE_PLOTS_CHECK_MVP_VS_SN_COMMENT",
		 "ONLINE_PLOTS_N_OVER_B",
		 "AC_WATER_BOTTLES_CHECKED",
		 "LAST_MODIFIED",
		 "MESSAGES",
		 "NOTES",
		 "BEAMLINE_TREND_PLOTS",
		 "CLEANUP_AREA");

foreach ($fields as $field) {
  
  //echo "$field<br>";
  ${$field}    = $_POST[$field];
  //echo "$field, ${$field}<br>\n";

}

/* some control parameters */
$mode=$_POST['mode'];
//echo "mode: $mode<br>";
$LAST_MODIFIED=date('l d-F-Y, H:i:s');

/* validate fields */
$array = split("-",$SHIFT_DATE);
$year  = $array[0];
$mon   = $array[1];
$day   = $array[2];

if ( $year < 2006 || $mon < 1 || $mon > 12 || $day < 1 || $day > 31 ) {
  echo "Invalid date [$SHIFT_DATE]! Press \"Go Back\" button in your browser and fix the error!\n";
  die();
}

$file = "XML/".$year."-".$mon."-".$day."_".$SHIFT_DAYTIME.".xml";

/*** if mode=new and file exist, do not overwrite file ***/
if ( $mode == 'new' ) {
  //echo "mode == new -&gt; checking if file exists...<br>";
  if ( file_exists($file) ) {
    echo "<b><font color=\"#FF0000\">ERROR!</font></b><br>";
    echo "File [$file] exist! Please, delete it first!<br>";
    echo "Press \"Go Back\" button in your browser to return to form<br>";    
    die();
  }
}


#echo <<<END
#date: $SHIFT_DATE -- $year-$mon-$day<br>
#file: $file<br>
#END;


$fd=fopen($file,'w');
fwrite($fd,'<?xml version="1.0"?>'."\n");
fwrite($fd,'<CHECKLIST>'."\n");

$isOk=TRUE;
foreach ($fields as $field) {
  //${$field}=str_replace("<","&lt;",${$field});
  //${$field}=str_replace(">","&gt;",${$field});
  ${$field}=htmlspecialchars(${$field});
  //${$field}=htmlentities(${$field});
  $isOK = fwrite($fd,"    <$field>${$field}</$field>\n");
  if ( !$isOK ) break;
}

#fprintf($fd,"    <SHIFT_NUMBER>%s</SHIFT_NUMBER>\n",$SHIFT_NUMBER);

fwrite($fd,'</CHECKLIST>');

fclose($fd);

if ( !$isOk ) {
  echo "***ERROR writing to the file [$file]!";
} else {
  echo "checklist was save to the file [$file] successfully!<br>";
  echo "Click <a href=\"makelist.php\">here</a> to return to the list of checklists.<br>";
}

?>
