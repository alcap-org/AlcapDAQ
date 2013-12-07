<?php

include('checklist_load.php');

$cdaytime = array("night", "day","evening");

echo <<<END
Shift Nr.: <b><font color="#FF0000">$SHIFT_NUMBER</font></b><br>
Date: <b>$SHIFT_DATE</b>, Daytime: <b>$cdaytime[$SHIFT_DAYTIME]</b><br>
Crue: <b>$SHIFT_CRUE</b><br>

<h3>Run monitoring</h3>
<ul>
  <li>Actual run number: <b><font color="blue">$ACTUAL_RUN_NUMBER</font></b></li>
  <li>Analyzer running?  <b>$ANALYZER_RUNNING </b></li>
  <li>Lazy Tape frontend on?  <b>$FE_LAZY_TAPE_ON</b><br>
      Labels: Tape 1: <b>$TAPE_1_LABEL</b>, Tape 2: <b>$TAPE_2_LABEL</b><br>
      Files:  Tape 1: <b>$TAPE_1_FILE</b>,  Tape 2: <b>$TAPE_2_FILE</b></li>
  <li>All WFD frontends on? <b>$FE_WFD_ON</b></li>
  <li>Event builder frontend on? <b>$EB_ON</b> Event rate: <b>$EB_EVENT_RATE</b> Hz, Data rate: <b>$EB_DATA_RATE</b> (kb/s)</li>
  <li>MagicBox frontend on?  <b>$MB_ON</b></li>
  <li>FIFOslave frontend on? <b>$EMC_ON</b></li>
  <li>Beamline frontend on?  <b>$FE_BEAMLINE_ON</b></li>
  <li>Onewire frontend on?   <b>$FE_ONEWIRE_ON</b></li>
  <li>HV frontend on?        <b>$FE_HV_ON</b></li>
  <li>Scaler frontend on?    <b>$FE_SCALER_ON</b></li>
  <li>Separator frontend on? <b>$FE_SEPARATOR_ON</b></li>
  <li>MIDAS Logger on?       <b>$FE_MLOGGER_ON</b><br>
      Actual file: <b>$FE_MLOGGER_FILE</b>, GB total: <b>$FE_MLOGGER_GB_TOTAL</b> </li>
  <li>DAWN monitor is on?    <b>$FE_DAWN_ON</b></li>
</ul>

<h3>Raid status</h3>

<ul>
<li>Raid status checked? <b>$RAID_STATUS</b><br>
Comments: <font color="#0000FF">
<pre>
$RAID_STATUS_COMMENT
</pre>
</font>
</li>
<li>Raid checked for alarms? <b>$RAID_ALARM</b><br>
Comments: <font color="#0000FF">
<pre>
$RAID_ALARM_COMMENTS
</pre>
</font>
</li>
</ul>

<h3>Kicker</h3>
<ul>
<li>HV settings on MV1: <b>$KICKER_HV_MV1</b> kV</li>
<li>HV settings on MV2: <b>$KICKER_HV_MV2</b> kV</li>
<li>Check LEDs on kicker cards. <br> 
Comment:<br>
<font color="#0000FF">
<pre>
$KICKER_LED_COMMENT
</pre>
</font>
</li>
</ul>

<h3>Separator</h3>
<ul>
 <li>HV settings: <b>$SEPARATOR_HV</b> kV </li>
 <li>Magnet setting: <b>$SEPARATOR_MAGNET </b> </li>
</ul>

<h3>EMC</h3>
<ul>
<li>EMC HV from top DMM: <b>$EMC_HV</b> V</li>
<li>EMC current from bottom DMM: <b>$EMC_CURRENT</b> nA</li>
<li>CF<sub>4</sub> bottle pressure: <b>$EMC_CF4_PRESSURE</b> bar, demand: <b>$EMC_CF4_PRESSURE_DEMAND</b> bar </li>
<li>C<sub>4</sub>H<sub>10</sub> bottle pressure: <b>$EMC_C4H10_PRESSURE</b> bar, demand <b>$EMC_C4H10_PRESSURE_DEMAND</b> bar</li>
<li>Flow rate: <b>$EMC_FLOW_RATE</b> cc/min </li>
<li>Bubble rate: <b>$EMC_BUBBLE_RATE</b> min<sup>-1</sup> </li>
<li>Gas alarm in ready state? <b>$GAS_ALARM_READY</b></li>
</ul>


<h3>Laser</h3>
<ul>
  <li>N<sub>2</sub> bottle pressure: <b>$LASER_N2_PRESSURE</b> bar, demand: <b>$LASER_N2_PRESSURE_DEMAND</b> bar</li>
  <li>Laser enabled? <b>$LASER_ENABLED</b><br> 
      Runs: <b>$LASER_ENABLED_RUNS</b></li>
</ul>

<h3>Beam</h3>
<ul>
  <li>Beam current:          <b>$BEAM_CURRENT</b> mkA </li>
  <li>Rate from ball scaler: <b>$SCALER_BALL_RATE</b> </li>
  <li>Protons scaler:        <b>$SCALER_PROTONS</b></li>
  <li>Time scaler:           <b>$SCALER_TIME</b></li>
  <li>Beamline magnet trend plots checked:  $BEAMLINE_TREND_PLOTS</li>
</ul>

<h3>Target open run</h3>
<ul>
  <li>Rate in Xbuff scaler:  <b>$SCALER_EMC_XBUFF</b> MHz</li>
  <li>Beam spot Ok?          <b>$EMC_BEAMSPOT_OK</b><br>
  Comment: <font color="#0000FF">
<pre>
$EMC_BEAMSPOT_COMMENT
</pre>
</font></li>
  <li>EMC current: <b>$EMC_CURRENT_TARGET_OPEN</b></li>
  <li>Water bottles in area checked? <b>$WHATER_BATTLES_AREA_CHECKED</b></li>
</ul>

<h3>WFD early start run</h3>
Runs: <b>$WFD_EARLY_START_RUNS</b>

<h3>WFD long island run</h3>
Runs: <b>$WFD_LONG_ISLAND_RUN_RUNS</b>

<h3>Online plots</h3>
<ul>
  <li>Most Probable Value vs. Serial number plot checked? <b>$ONLINE_PLOTS_CHECK_MVP_VS_SN</b><br>
Comment:<font color="#0000FF">
<pre>
$ONLINE_PLOTS_CHECK_MVP_VS_SN_COMMENT
</pre></font></li>
<li>N/B ratio: $ONLINE_PLOTS_N_OVER_B</li>
</ul>

<h3>Additional things:</h3>
<ul>
  <li>AC water bottles checked? <b>$AC_WATER_BOTTLES_CHECKED</b></li>
</ul>

<h3>Notes:</h3>
<font color="#0000FF">
<pre>
$NOTES
</pre>
</font>

<h3>Messages for the next shift:</h3>
<font color="#0000FF">
<pre>
$MESSAGES
</pre>
</font>

<br>
<font size="-1">Last modified on <em>$LAST_MODIFIED</em></font>
<hr>

<table width="100%">
<tr>
  <td>[<a href="form.php?mode=update&file=$file">Edit</a>] 
  </td>
  <td align="right">
      [<a href="makelist.php">Back to the List of Checklists</a>]
  </td>
</tr>
</table>
END;


/*** people did not want "delete" button, so I removed it ***/
//      [<a href="file_delete.php?file=$file">Delete</a>] 


?>

