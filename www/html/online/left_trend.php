
<script type="text/javascript" language="JavaScript">

function standardClick(val, doc) {  
  document.mainmenu.menuitem[val].checked=true;
  parent.frames[1].location.replace(doc);
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

echo "<a href=\"left.php\" target=\"framemenu\">Main menu</a> <font color=\"#FF0000\">-&gt;</font>";
echo "<hr>";

$menufile="trend.txt";

?>

<form name="mainmenu">

<?php

include 'left_makemenu.php';

echo "</form>";

?>


