<?php

$phpname="process_run_pairnumber";

$run_nr        = $_GET['run_nr'];
$pair_nr       = $_GET['pair_nr'];
$dir_name      = $_GET['dir'];
$macro_name    = $_GET['macro'];
$title         = $_GET['title'];
$run_nr_manual = $_GET['run_nr_manual'];

//echo "run: $run_nr, LED box: $ledbox_nr, dir_name: [$dir_name], macro: [$macro_name], title: [$title]";

echo "<P>";
echo "<form action=\"$phpname.php\" method=\"get\">";

include 'runlist.php';

?>

Pair number
<SELECT name="pair_nr">
<OPTGROUP label="pair #">

<?php
for ($i=1; $i<=170; $i++) {
  echo "<OPTION label=\"pair_$i\" value=\"$i\" ";
  if ( $i == $pair_nr ) {
    echo " selected ";
  }
  echo ">$i</OPTION>\n";
}

?>

</OPTGROUP>
</SELECT>

<?php

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

$out=shell_exec("./$phpname.sh $dir_name $macro_name $run_nr $pair_nr");

$rnd = rand();
echo "<center><img src=\"png/$macro_name.png?$rnd\"></center>";

include("html/$macro_name.html");

//echo "<P>Out:[<pre>$out</P></pre>]";


?>

</P>