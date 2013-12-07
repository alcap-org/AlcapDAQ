<?php

$cdaytime = array("night", "day", "evening");

$cfiles=shell_exec('ls XML/200[6-9]-[0-1][0-9]-[0-3][0-9]_[0-2].xml');

$date_old="";
$files=split("\n",$cfiles);
foreach ($files as $file) {
  if ( $file == "" ) continue;
  $date=substr($file,4,10);
  $daytime=substr($file,15,1);
  if ( $daytime < 0 || $daytime >2 )
    continue;

  if ( $daytime == 0 || $date != $date_old ) {
    echo "<br>\n".$date;
    $date_old = $date;
  }

  //echo " <a href=\"checklist_view.php?file=$file\">".$cdaytime[$daytime]."</a>";
  echo " <a href=\"form.php?file=$file&mode=view\">".$cdaytime[$daytime]."</a>";
}

#echo date('Y-m-d',time()-mktime(0,0,0,8,1,2006));
#echo "time: ".time()."<br>";
$date=date('Y-m-d');
$hour=date('H');
if ( $hour >= 1 && $hour < 7 ) 
     $daytime = 0;
     else
     if ( $hour >= 7 && $hour < 16 )
     $daytime = 1;
     else
     $daytime = 2;

echo "<br>";
#echo "hour: $hour, daytime: $daytime <br>";
$nextshift_nr = 1+intval((time()-mktime(0,0,0,6,9,2007))/(24*60*60))*3+$daytime;
//$nextshift_nr = (mktime(0,0,0,8,1,2006)-mktime(0,0,0,8,1,2006))/(24*60*60)*3+$daytime;
#echo "next  shift: $nextshift_nr <br>";

echo <<<END
<br><br>
[<a href="form.php?mode=new&SHIFT_NUMBER=$nextshift_nr&SHIFT_DAYTIME=$daytime&SHIFT_DATE=$date">Add new checklist</a>]
END;

?>
