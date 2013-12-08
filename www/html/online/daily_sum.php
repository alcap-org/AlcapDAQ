<?php

$cfiles=shell_exec('ls -r root/his999???.root');

// *** convert to array type ***
$files=split("\n",$cfiles);

echo "<h3>Daily sums of online histograms</h3>";

// *** extract run numbers from file name ***
foreach ($files as $file) {
  if ( $file == "" ) continue;
  $cday=substr($file,11,6);
  sscanf($cday,"%03d",$day);
  /*** convert to actual date: start date is 01-Aug-2006 ***/
  //$date = date('d-F-Y',mktime (0, 0, 0, 8, 1, 2006) + 24*60*60*$day);
  $date = date('d-F-Y',mktime (0, 0, 0, 6, 13, 2006) + 24*60*60*$day);
  if ( $day == 999 ) {
    echo "<b>All good runs:</b> <a href=\"$file\">$file</a><br>";
  } else if  ( $day == 998 ) {
    echo "<b>Target magnet right:</b> <a href=\"$file\">$file</a><br>";
  } else if  ( $day == 997 ) {
    echo "<b>Target magnet left:</b> <a href=\"$file\">$file</a><br>";
  } else if  ( $day == 996 ) {
    echo "<b>Target magnet right, hinge right:</b> <a href=\"$file\">$file</a><br>";
  } else if ( $day == 501 ) {
    echo "<b>Quartz target, 200ns fill:</b> <a href=\"$file\">$file</a><br>";
  } else {
    echo "$date <a href=\"$file\">$file</a><br>";
  }
}

    echo "<hr>";
    echo "<h4>Ag target, runs  63231 (June-27-2007) - 63774 (01-July-2007) </h4>";
    echo "<a href='root/his991110.root'>his991110.root</a>: Sum of all Ag 5mks fill runs<br>";
    echo "<a href='root/his991111.root'>his991111.root</a>: Magnet left, 5mks fill runs<br>";
    echo "<a href='root/his991112.root'>his991112.root</a>: Magnet right, 5mks fill runs<br>";
    echo "<a href='root/his991113.root'>his991113.root</a>: Magnet up, 5mks fill runs<br>";
    echo "<a href='root/his991114.root'>his991114.root</a>: Magnet down, 5mks fill runs<br>";

    echo "<h4>Ag target, ball displaced by -2cm (upstream), runs  63775 (01-July-2007) - 63877 (02-July-2007) </h4>";

    echo "<a href='root/his991120.root'>his991120.root</a>: All runs<br>";
    echo "<a href='root/his991122.root'>his991122.root</a>: Magnet right<br>";
    echo "<a href='root/his991123.root'>his991123.root</a>: Magnet up<br>";

    echo "<hr>";
    echo "<h4>Quartz target, ball displaced by -2cm (upstream), runs 63903 (02-July-2007) -  ...</h4>";
    echo "<h5>magnet pins at 7.3cm both sides</h5>";
    echo "<a href='root/his992111.root'>his992111.root</a>: Magnet left<br>";
    //echo "<a href='root/his992112.root'>his992112.root</a>: Magnet right<br>";

    echo "<h5>magnet pins at 7.5cm (beam right), 6.8cm (beam left)<br>plus runs after repositioning of the ball/magnet on July 05 (magnet pins are at 8.4cm right, 6.8cm left, ball displaced by extra 2.5mm upstream)</h5>";
    echo "<a href='root/his992121.root'>his992121.root</a>: Magnet left<br>";
    echo "<a href='root/his992122.root'>his992122.root</a>: Magnet right<br>";
    echo "<a href='root/his992123.root'>his992123.root</a>: Magnet up<br>";

    echo "<hr>";
    echo "<h4>AK3 target (runs 65144-..., July 14,15)</h4>";
    echo "<a href='root/his993000.root'>his993000.root</a>: All runs<br>";


    echo "<hr>";
    echo "<h4>Daily special runs</h4>";
    echo "18-June-2007 <a href='root/his998005.root'>his998005.root</a>: Daily sum before we tilted the ring magnet by 6deg; magnet up, hinge up.<br>";
    echo "18-June-2007 <a href='root/his999005.root'>his999005.root</a>: Sum of runs with ring magnet tilted by 6deg (top goes upstream), magnet up, hinge up.<br>";
    echo "19-June-2007 <a href='root/his998006.root'>his998006.root</a>: Sum of overnight runs with ring magnet tilted by 4.3deg top upstream (runs 62351-62414), magnet up, hinge up.<br>";
    echo "19-June-2007 <a href='root/his997006.root'>his997006.root</a>: Sum of day runs with ring magnet tilted by 6.4deg top upstream (runs 62415-62445), magnet up, hinge up.<br>";
    echo "19-June-2007 <a href='root/his996006.root'>his996006.root</a>: Sum of night runs with ring magnet tilted by about 6deg top downstream (runs 62342-62350), magnet up, hinge up.<br>";
    echo "01-July-2007 <a href='root/his998018.root'>his998018.root</a>: Daily sum before we moved the ball by -2cm (upstream)<br>";
    echo "08-July-2007 <a href='root/his998025.root'>his998025.root</a>: Daily sum, magnet left<br>";
    echo "21-July-2007 <a href='root/his999028.root'>his999028.root</a>: Daily sum: magnet left<br>";
    echo "22-July-2007 <a href='root/his998030.root'>his998030.root</a>: Daily sum: magnet left<br>";
    echo "22-July-2007 <a href='root/his998031.root'>his998031.root</a>: Systematics: magnet down, tilt -39.3&deg; (top downstream)<br>";
    echo "22-July-2007 <a href='root/his998032.root'>his998032.root</a>: Systematics: magnet down, tilt -23.2.3&deg; (top downstream)<br>";
    echo "22-July-2007 <a href='root/his998033.root'>his998033.root</a>: Systematics: magnet down, tilt -7.3.3&deg; (top downstream)<br>";
    echo "22-July-2007 <a href='root/his998034.root'>his998034.root</a>: Systematics: magnet down, tilt +6.2&deg; (top upstream)<br>";
    echo "22-July-2007 <a href='root/his998035.root'>his998035.root</a>: Systematics: magnet down, tilt +21.7&deg; (top upstream)<br>";
    echo "26-July-2007 <a href='root/his998036.root'>his998036.root</a>: Daily sum for \"Magnet right\" configuration<br>";
    echo "30-July-2007 <a href='root/his998037.root'>his998037.root</a>: Daily sum for \"Magnet left\" configuration (all runs from the day before the magnet had been rotated beam-right)<br>";
    echo "02-Aug-2007  <a href='root/his998038.root'>his998038.root</a>: Daily sum of \"Magnet right\" runs (all runs from the day before the magnet had been rotated beam-left)<br>";
    echo "04-Aug-2007  <a href='root/his998039.root'>his998039.root</a>: Daily sum of \"Magnet left\" runs (all runs from the day before the magnet had been rotated beam-right)<br>";
    echo "12-Aug-2007  <a href='root/his998040.root'>his998040.root</a>: Daily sum of \"Magnet Right-Down 15&deg;\" runs<br>";
    echo "12-Aug-2007  <a href='root/his999060.root'>his999060.root</a>: Daily sum of \"Magnet Left-Up 15&deg;\" runs<br>";
    echo "13-Aug-2007  <a href='root/his998041.root'>his998041.root</a>: Daily sum of \"Magnet Left-Up 15&deg;\" runs<br>";
    echo "13-Aug-2007  <a href='root/his998042.root'>his998042.root</a>: Daily sum of \"Magnet Left-Down 15&deg;\" runs<br>";
    echo "13-Aug-2007  <a href='root/his999061.root'>his999061.root</a>: Daily sum of \"Magnet Right-Up 15&deg;\" runs<br>";
    echo "14-Aug-2007  <a href='root/his998043.root'>his998043.root</a>: Daily sum of \"Magnet Right-Up 15&deg;\" runs<br>";

    echo "<a href='root/his992131.root'>his992131.root</a>: quartz, magnet left,  final ball position<br>";
    echo "<a href='root/his992132.root'>his992132.root</a>: quartz, magnet right, final ball position<br>";
    echo "<a href='root/his992130.root'>his992130.root</a>: quartz, magnet left+right, final ball position<br>";

?>

