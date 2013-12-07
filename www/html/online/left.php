
<script type="text/javascript" language="JavaScript">

function standardClick(val,doc) {  
  document.mainmenu.menuitem[val].checked=true;
  //parent.frames[1].location.reload(doc);
  var run_nr = document.mainmenu.run_nr.value;
  var run_nr_manual = document.mainmenu.run_nr_manual.value;
  if ( run_nr_manual > 0) 
     run_nr = run_nr_manual;  
  parent.frames[1].location.replace(doc+"&run_nr="+run_nr);
}

function RunSelected() {
  document.mainmenu.run_nr_manual.value="";
}

</script>

<STYLE tupe="text/css">

span.menu {
  cursor: pointer;
  color: blue;
  font-family: veranda, tahoma, sans-serif;
  font-size: 12px;
}

span.menu:hover {
  text-decoration: underline;
}

</STYLE>

<body>

<?php

$run_nr_manual=$_GET['run_nr_manual'];

echo "<a href=\"left_hist.php?run_nr=0\" target=\"framemenu\">Histogram List</a> <font color=\"#FF0000\">-&gt;</font><br>";
echo "<a href=\"left_slow.php?rollback=2\" target=\"framemenu\">Slow Control</a> <font color=\"#FF0000\">-&gt;</font><br>";
echo "<a href=\"left_trend.php\" target=\"framemenu\">Trend Plots</a> <font color=\"#FF0000\">-&gt;</font><br>";
echo "<a href=\"Checklist/makelist.php\" target=\"framemain\">Checklists</a><br>";
echo "<a href=\"framemain.html\" target=\"framemain\">Home</a><br>";
echo "<hr>";


$menufile="index.txt";
echo "<form name=\"mainmenu\">";
include 'runlist_left.php';
echo "<br>";
include 'left_makemenu.php';

echo "<hr>";
echo "<a href=\"left_all_macros.php\" target=\"framemenu\">All macros</a> <font color=\"#FF0000\">-&gt;</font><br>";

/***
echo <<< END
<a href="/cgi-bin/mulan/make_run_lists.cgi" target="framemain">Run Log</a><br>
<a href="/cgi-bin/mulan/make_run_selection.cgi?L:20" target="framemain">Last 20 runs</a><br>
<a href="/cgi-bin/mulan/make_run_selection.cgi?L:100" target="framemain">Last 100 runs</a><br>
END;
***/

echo "</form>";

?>

</body>