<?php

$phpname="process_void";

$dir_name   = $_GET['dir'];
$macro_name = $_GET['macro'];
$title      = $_GET['title'];

//echo "run: $run_nr, dir_name: [$dir_name], macro: [$macro_name], title: [$title]";

echo "<P>";
echo "<form action=\"$phpname.php\" method=\"get\">";


/*** hidden fields ***/
echo "<INPUT TYPE=HIDDEN NAME=\"dir\" VALUE=\"$dir_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"macro\" VALUE=\"$macro_name\">";
echo "<INPUT TYPE=HIDDEN NAME=\"title\" VALUE=\"$title\">";

echo "</P>";
echo "</form>";

echo "<center><h2><font color=\"#FF0000\">$title </font> </h2></center>";

$out=shell_exec("./$phpname.sh $dir_name $macro_name");

$rnd = rand();
echo "<center><img src=\"png/$macro_name.png?$rnd\"></center>";

echo "<hr>";

/*** add html output of the macro ***/

include("html/$macro_name.html");

?>

</P>