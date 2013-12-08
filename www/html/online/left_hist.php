
<script type="text/javascript" language="JavaScript">

function standardClick(val) {  
  document.mainmenu.menuitem[val].checked=true;
}

</script>


<?php

$run_nr = $_GET['run_nr'];

echo "<a href=\"left.php\" target=\"framemenu\">Main menu</a> <font color=\"#FF0000\">-&gt;</font>";
echo "<hr>";

include 'runlist_dynamic.php';
//echo "<br>";
//echo "<hr>";
echo "<form name=\"mainmenu\">";


$out=shell_exec("./print_hist_list.sh  $run_nr");

#echo "run nr: $run_nr, $out";

$hnames=split(" ",$out);
$id=0;
foreach ($hnames as $hname) {

  if ( $hname == "") continue;

  echo "<INPUT type=\"radio\" name=\"menuitem\" disabled></INPUT><a href=\"plot_histo.php?run=$run_nr&hname=$hname\" target=\"framemain\" onclick=\"standardClick($id);\">$hname</a><br>\n";
  $id++;
  
}

echo "</form>";

?>