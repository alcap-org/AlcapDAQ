
<script type="text/javascript" language="JavaScript">

function standardClick(val, doc) {  
  document.mainmenu.menuitem[val].checked=true;
  parent.frames[1].location.replace(doc);
}

function selectRollback() {  
  //alert(document.mainmenu.rollbacklist.options[document.mainmenu.rollbacklist.selectedIndex].value);
  //alert(document.mainmenu.rollback.value);
  var $rollback=document.mainmenu.rollbacklist.options[document.mainmenu.rollbacklist.selectedIndex].value;
  parent.frames[0].location="left_slow.php?rollback="+$rollback;
  //document.mainmenu.rollback.value=document.mainmenu.rollbacklist.options[document.mainmenu.rollbacklist.selectedIndex].value;
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

<?

$rollback   = $_GET['rollback'];


echo "<a href=\"left.php\" target=\"framemenu\">Main menu</a> <font color=\"#FF0000\">-&gt;</font>";
echo "<hr>";

$menufile="slow.txt";

?>

<form name="mainmenu">

History: 
<SELECT name="rollbacklist" onchange="selectRollback();">
<OPTGROUP label="rollback">
<OPTION label="2h"  value="2"  <?php if( $rollback == 2  ) echo "selected" ?>>  2 hours </OPTION>
<OPTION label="4h"  value="4"  <?php if( $rollback == 4  ) echo "selected" ?>>  4 hours </OPTION>
<OPTION label="8h"  value="8"  <?php if( $rollback == 8  ) echo "selected" ?>>  8 hours </OPTION>
<OPTION label="12h" value="12" <?php if( $rollback == 12 ) echo "selected" ?>> 12 hours </OPTION>
<OPTION label="1d"  value="24" <?php if( $rollback == 24 ) echo "selected" ?>>  1 day   </OPTION>
<OPTION label="2d"  value="48" <?php if( $rollback == 48 ) echo "selected" ?>>  2 days  </OPTION>
<OPTION label="1w"  value="168"<?php if( $rollback == 168) echo "selected" ?>>  1 week  </OPTION>
<OPTION label="1m"  value="720"<?php if( $rollback == 720) echo "selected" ?>>  1 month </OPTION>
<OPTION label="All" value="0"  <?php if( $rollback == 0  ) echo "selected" ?>>    All   </OPTION>
</OPTGROUP>
</SELECT>
<br>

<?php

include 'left_makemenu.php';
echo "</form>";

?>


