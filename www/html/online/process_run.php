<?php

$phpname="process_run";

$run_nr        = $_GET['run_nr'];
$dir_name      = $_GET['dir'];
$macro_name    = $_GET['macro'];
$title         = $_GET['title'];
$run_nr_manual = $_GET['run_nr_manual'];


//echo "run: $run_nr, dir_name: [$dir_name], macro: [$macro_name], title: [$title]";

echo "<P>";
echo "<form action=\"$phpname.php\" method=\"get\">";

include 'runlist.php';

/*** hidden fields ***/
echo "<INPUT TYPE=HIDDEN NAME=\"dir\" VALUE=\"$dir_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"macro\" VALUE=\"$macro_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"title\" VALUE=\"$title\">";

echo "<INPUT type=\"submit\" value=\"Plot\">";
echo "</P>";
echo "</form>";

?>

<hr>
<P>

<?php

$crun=$run_nr;
if ( $run_nr == 0 ) {
  $crun="on-line";
}

echo "<center><h2><font color=\"#FF0000\">$title </font> <font size=\"+1\" color=\"#0000FF\">run: $crun</font></h2></center>";

$out=shell_exec("./$phpname.sh $dir_name $macro_name $run_nr");

#$im = imagecreatefrompng("png/$macro_name.png");
#imagepng($im);

$rnd = rand();
echo "<center><img src=\"png/$macro_name.png?$rnd\"></center>";

echo "<hr>";

/*** add html output of the macro ***/

include("html/$macro_name.html");

?>

</P>