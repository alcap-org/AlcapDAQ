<?php

$phpname="process_run_crate_slotrange";

$run_nr        = $_GET['run_nr'];
$crate_nr      = $_GET['crate_nr'];
$slot1_nr      = $_GET['slot1_nr'];
$slot2_nr      = $_GET['slot2_nr'];
$dir_name      = $_GET['dir'];
$macro_name    = $_GET['macro'];
$title         = $_GET['title'];
$run_nr_manual = $_GET['run_nr_manual'];


if ( $slot2_nr < $slot1_nr ) {
  $slot2_nr = $slot1_nr;
}

//echo "run: $run_nr, crate: $crate_nr, slots: $slot1_nr-$slot2_nr, dir_name: [$dir_name], macro: [$macro_name], title: [$title]";

echo "<P>";
echo "<form action=\"$phpname.php\" method=\"get\">";

include 'runlist.php';
include 'cratelist.php';
include 'slotrange.php';

/*** hidden fields ***/
echo "<INPUT TYPE=HIDDEN NAME=\"dir\" VALUE=\"$dir_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"macro\" VALUE=\"$macro_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"title\" VALUE=\"$title\">";

echo "<INPUT type=\"submit\" value=\"Plot\">";
echo "</P>";
echo "</form>";

echo "<hr>";
echo "<P>";

$crun=$run_nr;
if ( $run_nr == 0 ) {
  $crun="on-line";
}

echo "<center><h2><font color=\"#FF0000\">$title </font> <font size=\"+1\" color=\"#0000FF\">run: $crun</font></h2></center>";

$out=shell_exec("./$phpname.sh $dir_name $macro_name $run_nr $crate_nr $slot1_nr $slot2_nr ");

$rnd = rand();
echo "<center><img src=\"png/$macro_name.png?$rnd\"></center>";

include("html/$macro_name.html");

//echo "<P>Out:[<pre>$out</P></pre>]";


?>

</P>