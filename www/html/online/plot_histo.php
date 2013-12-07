<?php

$run_nr = $_GET['run'];
$hname  = $_GET['hname'];

//echo "run: $run_nr, hname=[$hname]";

$out=shell_exec("./process_run_hname.sh common plot_histo $run_nr $hname");

echo "<CENTER>\n";
echo "<h2><font color=\"#FF0000\">$hname </font> <font size=\"+1\" color=\"#0000FF\">run: $run_nr</font></h2></center>";
$rnd = rand();
echo "<img src=\"png/plot_histo.png?$rnd\"><br>";
echo "</CENTER>\n";

?>
