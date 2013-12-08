
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

echo "<a href=\"left.php\" target=\"framemenu\">Main menu</a> <font color=\"#FF0000\">-&gt;</font>";

echo "<hr>";

$menufile="all_macros.txt";
echo "<form name=\"mainmenu\">";
include 'runlist_left.php';
echo "<br>";
include 'left_makemenu.php';
echo "</form>";

?>
