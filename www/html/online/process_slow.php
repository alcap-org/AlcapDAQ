<?php

$phpname="process_slow";

$dir_name   = $_GET['dir'];
$macro_name = $_GET['macro'];
$title      = $_GET['title'];

$rollback   = $_GET['rollback'];

//echo "dir: $dir_name, macro: $macro_name, title: $title rollback: $rollback";

echo "<center><h2><font color=\"#FF0000\">$title </font> <font size=\"+1\" color=\"#0000FF\"> last $rollback hours</font></h2></center>";

$out=shell_exec("./$phpname.sh $dir_name $macro_name $rollback");

#echo "Out: $out";

$rnd = rand();
echo "<center><img src=\"png/$macro_name.png?$rnd\"></center>";

echo "<hr>";

include("html/$macro_name.html");

?>