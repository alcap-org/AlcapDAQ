<?php

$phpname="process_trend";

$dir_name   = $_GET['dir'];
$macro_name = $_GET['macro'];
$title      = $_GET['title'];

$run_nr_1   = $_GET['run_nr_1'];
$run_nr_2   = $_GET['run_nr_2'];
$rollback   = $_GET['rollback'];

if ( $run_nr_2 < $run_nr_1 ) {
  $run_nr_2 = $run_nr_1;
}

//echo "run: $run_nr, crate: $crate_nr, slots: $slot1_nr-$slot2_nr, dir_name: [$dir_name], macro: [$macro_name], title: [$title]";

echo "<P>";
echo "<form action=\"$phpname.php\" method=\"get\">";

echo "Last ";
echo "<a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=2\">2h</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=4\">4h</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=8\">8h</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=24\">24h</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=48\">2d</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=168\">1weak</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=720\">1month</a>";
echo ", <a href=\"$phpname.php?title=$title&dir=$dir_name&macro=$macro_name&run_nr_1=0&run_nr_2=0&rollback=64000\">All</a>";
echo " or Run range ";
echo "<INPUT type=\"text\" name=\"run_nr_1\" maxlength=\"6\" size=\"6\" value=\"$run_nr_1\"> ";
echo " - ";
echo "<INPUT type=\"text\" name=\"run_nr_2\" maxlength=\"6\" size=\"6\" value=\"$run_nr_2\"> ";

echo "<INPUT type=\"submit\" value=\"Plot\">";

/*** hidden fields ***/
echo "<INPUT TYPE=HIDDEN NAME=\"dir\" VALUE=\"$dir_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"macro\" VALUE=\"$macro_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"title\" VALUE=\"$title\">";
echo "<INPUT TYPE=HIDDEN NAME=\"rollback\" VALUE=\"0\">";

echo "</P>";
echo "</form>";

echo "<hr>";
echo "<P>";

echo "<center><h2><font color=\"#FF0000\">$title </font>";
if ( $rollback > 0 ) {
  $crollback = "";
  if ( $rollback <= 24 ) {
    $crollback = "last  $rollback hours";
  } else if ( $rollback <= 720 ) {
    $days = $rollback/24;
    $crollback = "last $days days";
  } else {
    $crollback = "All";
  }
  echo "<font size=\"+1\" color=\"#0000FF\">$crollback</font></h2></center>";
} else {
  echo "</font><font size=\"+1\" color=\"#0000FF\">runs $run_nr_1-$run_nr_2</font></h2></center>";  
}

$out=shell_exec("./$phpname.sh $dir_name $macro_name $run_nr_1 $run_nr_2 $rollback");

#echo $out;

echo "<center>";
$rnd = rand();
echo "<img src=\"png/$macro_name.png?$rnd\"><br>";
echo "</center>";

echo "<hr>";

/*** add html output of the macro ***/

include("html/$macro_name.html");

?>
