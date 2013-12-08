<?php

$mode=$_GET['mode'];


if ( $mode=="edit" ) {
  include('checklist_load.php');
} else if ( $mode=="new" ) {
  $SHIFT_NUMBER=$_GET['SHIFT_NUMBER'];
  $SHIFT_DAYTIME=$_GET['SHIFT_DAYTIME'];
  $SHIFT_DATE=$_GET['SHIFT_DATE'];
} 

if ( $mode=="view" ) {
  include('checklist_load.php');
  $disabled="DISABLED";
} else {
  $disabled="";
}

echo <<<END
<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0 Transitional//EN'>
<html>
<head>
<title>Checklist $SHIFT_DATE</title>
<META HTTP-EQUIV='Content-Type' CONTENT='text/html; charset=UTF-8'>
</head>
<body bgcolor="khaki">
<form action="form_save.php" method="POST">
Shift Nr. <INPUT type="text" name="SHIFT_NUMBER" maxlength="3" size="4" value="$SHIFT_NUMBER" $disabled> 
END;

if ( $mode == 'new' ) {
  echo <<<END
    Date (yyyy-mm-dd) <INPUT type="text" name="SHIFT_DATE" maxlength="10" size="10" value="$SHIFT_DATE"> 
    Daytime <SELECT name="SHIFT_DAYTIME">
END;
  echo "<OPTION label=\"Night\"   value=\"0\""; 
  if ($SHIFT_DAYTIME == 0) echo "selected";
  echo ">Night   (01:00-07:30) </OPTION>";

  echo "<OPTION label=\"Day\"     value=\"1\"";
  if ($SHIFT_DAYTIME == 1) echo "selected"; 
  echo ">Day     (07:30-16:00) </OPTION>";

  echo "<OPTION label=\"Evening\" value=\"2\"";
  if ($SHIFT_DAYTIME == 2) echo "selected";
  echo ">Evening (16:00-01:00) </OPTION>";


  echo "</SELECT>";
} else {
  echo <<<END
    <INPUT type="hidden" name="SHIFT_DATE" maxlength="3" size="4" value="$SHIFT_DATE"> 
    <INPUT type="hidden" name="SHIFT_DAYTIME" maxlength="3" size="4" value="$SHIFT_DAYTIME"> 
END;
}

if ( $mode == "view" ) {
  echo "[<a href='form.php?mode=edit&file=$file'>Edit</a>]";
}

/***

<OPTION label="Day"     value="0" <?php if ($SHIFT_DAYTIME == 0) echo "selected";?> >Day     (07:30-16:00) </OPTION>
<OPTION label="Evening" value="1" <?php if ($SHIFT_DAYTIME == 1) echo "selected";?> >Evening (16:00-01:00) </OPTION>
<OPTION label="Night"   value="2" <?php if ($SHIFT_DAYTIME == 2) echo "selected";?> >Night   (01:00-07:30) </OPTION>
***/

$fields_checked = array ("BEAMLINE_TREND_PLOTS",
			 "FE_WFD_ON",
			 "MB_ON",
			 "EB_ON",
			 "EMC_ON",
			 "FE_BEAMLINE_ON",
			 "FE_ONEWIRE_ON",
			 "FE_HV_ON",
			 "FE_SCALER_ON",
			 "FE_SEPARATOR_ON",
			 "FE_DAWN_ON",
			 "FE_COBRA_ON",
			 "FE_MLOGGER_ON",
			 "ANALYZER_RUNNING",
			 "FE_LAZY_TAPE_1",
			 "FE_LAZY_TAPE_2",
			 "LAZY_TB06",
			 "LAZY_PSI",
			 "RAID_STATUS",
			 "RAID_ALARM",
			 "KICKER_LED",
			 "WHATER_BATTLES_AREA_CHECKED",
			 "ONLINE_PLOTS_CHECK_MVP_VS_SN",
			 "AC_WATER_BOTTLES_CHECKED",
			 "CLEANUP_AREA");

foreach ($fields_checked as $field) {
  $chk=$field."_CHECKED";
  if (${$field} == "Y" || ${$field} == "y") 
    ${$chk}="CHECKED";
  else
    ${$chk}="N";
  //echo "value: $field, $BEAMLINE_TREND_PLOTS_CHECKED, $chk, [${$chk}]";
}


echo <<<END

<br>
Crue <INPUT type="text" name="SHIFT_CRUE" maxlength="80" size="80" value="$SHIFT_CRUE" $disabled><br>
<hr>
<h3>Run monitoring</h3>

Actual run number <INPUT type="text" name="ACTUAL_RUN_NUMBER" maxlength="10" size="10" value="$ACTUAL_RUN_NUMBER" $disabled>
   (at the moment of filling this checklist).

    <br>

<INPUT type='checkbox' name='FE_WFD_ON'       value='Y' $FE_WFD_ON_CHECKED       $disabled> All WFD frontends are running<br>
<INPUT type='checkbox' name='MB_ON'           value='Y' $MB_ON_CHECKED           $disabled> MagicBox frontend is running<br>
<INPUT type='checkbox' name='EB_ON'           value='Y' $EB_ON_CHECKED           $disabled> Event builder is running. Event rate:  <INPUT type="text" name="EB_EVENT_RATE" maxlength="3" size="3" value="$EB_EVENT_RATE" $disabled> s<sup>-1</sup> (7), Data rate:  <INPUT type="text" name="EB_DATA_RATE" maxlength="10" size="10" value="$EB_DATA_RATE" $disabled> kb/s (20000)<br>
<INPUT type='checkbox' name='EMC_ON'          value='Y' $EMC_ON_CHECKED          $disabled> FIFOslave frontend is running. (Should be on if target is open, off otherwise)<br>

<INPUT type='checkbox' name='FE_BEAMLINE_ON'  value='Y' $FE_BEAMLINE_ON_CHECKED  $disabled> Beamline frontend is running<br>
<INPUT type='checkbox' name='FE_ONEWIRE_ON'   value='Y' $FE_ONEWIRE_ON_CHECKED   $disabled> Onewire frontend is running<br>
<INPUT type='checkbox' name='FE_HV_ON'        value='Y' $FE_HV_ON_CHECKED        $disabled> HV frontend is running<br>
<INPUT type='checkbox' name='FE_SCALER_ON'    value='Y' $FE_SCALER_ON_CHECKED    $disabled> Scaler_slow frontend is running<br>
<INPUT type='checkbox' name='FE_SEPARATOR_ON' value='Y' $FE_SEPARATOR_ON_CHECKED $disabled> Separator frontend is running<br>
<INPUT type='checkbox' name='FE_DAWN_ON'      value='Y' $FE_DAWN_ON_CHECKED      $disabled> DAWN monitor is running<br>
<INPUT type='checkbox' name='FE_COBRA_ON'     value='Y' $FE_COBRA_ON_CHECKED     $disabled> Cobra monitor is running<br>

<INPUT type='checkbox' name='FE_MLOGGER_ON'   value='Y' $FE_MLOGGER_ON_CHECKED   $disabled> MIDAS Logger is running. Actual file:<INPUT type="text" name="FE_MLOGGER_FILE" maxlength="15" size="15" value="$FE_MLOGGER_FILE" $disabled> Total Gigabytes written <INPUT type="text" name="FE_MLOGGER_GB_TOTAL" maxlength="15" size="15" value="$FE_MLOGGER_GB_TOTAL" $disabled><br>

<INPUT type='checkbox' name='ANALYZER_RUNNING' value='Y' $ANALYZER_RUNNING_CHECKED $disabled> Analyzer is running <br>

<INPUT type='checkbox' name='FE_LAZY_TAPE_1' value='Y' $FE_LAZY_TAPE_1_CHECKED $disabled> Lazy Tape 1 logger is running. Label: <INPUT type="text" name="TAPE_1_LABEL" maxlength="9" size="10" value="$TAPE_1_LABEL" $disabled> File name: <INPUT type="text" name="TAPE_1_FILE" maxlength="20" size="20" value="$TAPE_1_FILE" $disabled><br>
<INPUT type='checkbox' name='FE_LAZY_TAPE_2' value='Y' $FE_LAZY_TAPE_2_CHECKED $disabled> Lazy Tape 2 logger is running. Label: <INPUT type="text" name="TAPE_2_LABEL" maxlength="9" size="10" value="$TAPE_2_LABEL" $disabled> File name: <INPUT type="text" name="TAPE_2_FILE" maxlength="20" size="20" value="$TAPE_2_FILE" $disabled><br>
<INPUT type='checkbox' name='LAZY_TB06'      value='Y' $LAZY_TB06_CHECKED      $disabled> Lazy tb06 logger is running. File name: <INPUT type="text" name="LAZY_TB06_FILE" maxlength="20" size="20" value="$LAZY_TB06_FILE" $disabled><br>
<INPUT type='checkbox' name='LAZY_PSI'       value='Y' $LAZY_PSI_CHECKED       $disabled> Lazy lynx logger is running. File name: <INPUT type="text" name="LAZY_PSI_FILE"  maxlength="20" size="20" value="$LAZY_PSI_FILE"  $disabled><br>

  

<h3>Raid status</h3>


<table>
 <tr>
  <td valign="top">
   <INPUT type='checkbox' name='RAID_STATUS' value='Y' $RAID_STATUS_CHECKED $disabled> Raid status checked.
   <div style="margin-left: 20pt;">
     How to check the status of the RAID: <br> 
     Go to <a href="https://be02:8080" target="aux">https://be02:8080</a> in
     your browser and login as <em>administrator</em> with mulan <em>root</em> password.
   </div>
  </td>
  <td valign="top">
    Comments on RAID status:<br>
    <TEXTAREA name="RAID_STATUS_COMMENT" rows="5" cols="60" tabindex="40" $disabled>$RAID_STATUS_COMMENT</TEXTAREA><br>
  </td>
 </tr>
 <tr>
  <td valign="top">
   <INPUT type='checkbox' name='RAID_ALARM' value='Y' $RAID_ALARM_CHECKED $disabled> Raid checked for alarms.<br>
  </td>
  <td valign="top">
    Comments on RAID alarms:<br>
    <TEXTAREA name="RAID_ALARM_COMMENTS" rows="5" cols="60" tabindex="40" $disabled>$RAID_ALARM_COMMENTS</TEXTAREA><br>
  </td>
</table>

<h3>Kicker</h3>

HV settings: MV1 <INPUT type="text" name="KICKER_HV_MV1" maxlength="10" size="10" value="$KICKER_HV_MV1" $disabled> kV; MV2: <INPUT type="text" name="KICKER_HV_MV2" maxlength="10" size="10" value="$KICKER_HV_MV2" $disabled> kV<br>

<INPUT type='checkbox' name='KICKER_LED' value='Y' $KICKER_LED_CHECKED $disabled> Check LEDs on kicker cards.
<div style="margin-left: 20pt;">
   Comment on the LEDs status:<br>
   <TEXTAREA name="KICKER_LED_COMMENT" rows="5" cols="60" tabindex="40" $disabled>$KICKER_LED_COMMENT</TEXTAREA>
</div>

<h3>Separator</h3>
HV settings: <INPUT type="text" name="SEPARATOR_HV" maxlength="10" size="10" value="$SEPARATOR_HV" $disabled> kV (185)<br>
Magnet setting: <INPUT type="text" name="SEPARATOR_MAGNET" maxlength="10" size="10" value="$SEPARATOR_MAGNET" $disabled> (920 w/HV at 185)<br>


<h3>EMC</h3>

EMC HV from top DMM <INPUT type="text" name="EMC_HV" maxlength="10" size="10" value="$EMC_HV" $disabled> V (-3000)<br>

EMC current from bottom DMM <INPUT type="text" name="EMC_CURRENT" maxlength="10" size="10" value="$EMC_CURRENT" $disabled> nA (3), (0.01mV=1nA) <br>

CF<sub>4</sub> bottle pressure <INPUT type="text" name="EMC_CF4_PRESSURE" maxlength="10" size="10" value="$EMC_CF4_PRESSURE" $disabled> bar (100),
demand  <INPUT type="text" name="EMC_CF4_PRESSURE_DEMAND" maxlength="10" size="10" value="$EMC_CF4_PRESSURE_DEMAND" $disabled> bar (0.5)<br>

C<sub>4</sub>H<sub>10</sub> bottle pressure <INPUT type="text" name="EMC_C4H10_PRESSURE" maxlength="10" size="10" value="$EMC_C4H10_PRESSURE" $disabled> bar (0),
demand  <INPUT type="text" name="EMC_C4H10_PRESSURE_DEMAND" maxlength="10" size="10" value="$EMC_C4H10_PRESSURE_DEMAND" $disabled> bar (0.5)<br>

flow rate <INPUT type="text" name="EMC_FLOW_RATE" maxlength="10" size="10" value="$EMC_FLOW_RATE" $disabled> cc/min (5)<br>

bubble rate <INPUT type="text" name="EMC_BUBBLE_RATE" maxlength="10" size="10" value="$EMC_BUBBLE_RATE" $disabled> min<sup>-1</sup> (20)<br>

<INPUT type="text" name="GAS_ALARM_READY" maxlength="1" size="1" value="$GAS_ALARM_READY" $disabled> gas alarm in ready state (Y/N)<br>


<h3>Laser</h3>
Read <a href="howto_laser_run.html" target="aux">how to take a "Laser" run.</a><br>

N<sub>2</sub> bottle pressure <INPUT type="text" name="LASER_N2_PRESSURE" maxlength="10" size="10" value="$LASER_N2_PRESSURE" $disabled> bar 
    (&gt;25), demand <INPUT type="text" name="LASER_N2_PRESSURE_DEMAND" maxlength="10" size="10" value="$LASER_N2_PRESSURE_DEMAND" $disabled> bar (6.5) <br>

<INPUT type="text" name="LASER_ENABLED" maxlength="1" size="1" value="$LASER_ENABLED" $disabled> Enabled? (Y/N)<br> If so, for what runs
<INPUT type="text" name="LASER_ENABLED_RUNS" maxlength="100" size="50" value="$LASER_ENABLED_RUNS" $disabled>

<h3>Beam</h3>

Beam current <INPUT type="text" name="BEAM_CURRENT" maxlength="12" size="13" value="$BEAM_CURRENT" $disabled> mkA <br>
Rate from ball scaler <INPUT type="text" name="SCALER_BALL_RATE" maxlength="12" size="13" value="$SCALER_BALL_RATE" $disabled>  (180000)<br>
Proton scaler <INPUT type="text" name="SCALER_PROTONS" maxlength="12" size="13" value="$SCALER_PROTONS" $disabled><br>
Time scaler <INPUT type="text" name="SCALER_TIME" maxlength="12" size="13" value="$SCALER_TIME" $disabled><br>
<INPUT type='checkbox' name='BEAMLINE_TREND_PLOTS' value='Y' $BEAMLINE_TREND_PLOTS_CHECKED $disabled>check <a href="../process_slow.php?title=Beamline&dir=slow&macro=Beamline&rollback=12" target="aux">beamline magnet trend plots</a><br>

<h3>Target open run</h3>
Read <a href="howto_target_open_run.html" target="aux">how to take a "target open" run.</a><br>
Rate in Xbuff scaler <INPUT type="text" name="SCALER_EMC_XBUFF" maxlength="12" size="13" value="$SCALER_EMC_XBUFF" $disabled>MHz (5-7)<br>
Beam spot Ok? (Y/N) <INPUT type="text" name="EMC_BEAMSPOT_OK" maxlength="1" size="1" value="$EMC_BEAMSPOT_OK" $disabled><br>
Comment:<br>
<TEXTAREA name="EMC_BEAMSPOT_COMMENT" rows="5" cols="60" tabindex="40" $disabled>$EMC_BEAMSPOT_COMMENT</TEXTAREA><br>
EMC current <INPUT type="text" name="EMC_CURRENT_TARGET_OPEN" maxlength="12" size="13" value="$EMC_CURRENT_TARGET_OPEN" $disabled><br>

<INPUT type='checkbox' name='WHATER_BATTLES_AREA_CHECKED' value='Y' $WHATER_BATTLES_AREA_CHECKED_CHECKED $disabled> Check water bottles while in area .<br>

<h3>WFD early start run</h3>
Read <a href="howto_wfd_early_start.html" target="aux">how to take a "WFD early start run"</a><br>

Runs: <INPUT type="text" name="WFD_EARLY_START_RUNS" maxlength="50" size="50" value="$WFD_EARLY_START_RUNS" $disabled> 

<h3>WFD long island run</h3>

Runs: <INPUT type="text" name="WFD_LONG_ISLAND_RUN_RUNS" maxlength="50" size="50" value="$WFD_LONG_ISLAND_RUN_RUNS" $disabled> 


<h3>Online plots</h3>

<INPUT type='checkbox' name='ONLINE_PLOTS_CHECK_MVP_VS_SN' value='Y' $ONLINE_PLOTS_CHECK_MVP_VS_SN_CHECKED $disabled> <a href="../process_run.php?title=defnan_MPV&dir=defnan&macro=defnan_MPV&run_nr=0" target="aux">Most Probable Value vs. Serial number plot</a>.<br>

N/B value: <INPUT type="text" name="ONLINE_PLOTS_N_OVER_B" maxlength="15" size="15" value="$ONLINE_PLOTS_N_OVER_B" $disabled><br>

<h3>Additional things:</h3>

<INPUT type='checkbox' name='AC_WATER_BOTTLES_CHECKED' value='Y' $AC_WATER_BOTTLES_CHECKED_CHECKED $disabled>Check AC water bottles.<br>
<INPUT type='checkbox' name='CLEANUP_AREA' value='Y' $CLEANUP_AREA_CHECKED $disabled>Clean up area<br>


<br>
<b><font color="#0000FF">Notes</font></b><br>
<TEXTAREA name="NOTES" rows="30" cols="80" tabindex="40" $disabled>$NOTES</TEXTAREA><br>
<br>

<b><font color="#0000FF">Messages for the next shift</font></b><br>
<TEXTAREA name="MESSAGES" rows="10" cols="80" tabindex="40" $disabled>$MESSAGES</TEXTAREA><br>


END;

if ( $mode == "view" ) {
  echo "[<a href='form.php?mode=edit&file=$file'>Edit</a>]";
} else {
  echo "<INPUT type='submit' value='Save'>";
}

echo <<<END

<INPUT TYPE="HIDDEN" NAME="mode" VALUE="$mode">


</form>

END;


?>

