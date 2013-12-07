<?php

echo " Crate ";
echo "<SELECT name=\"crate_nr\">";
echo "<OPTGROUP label=\"crate #\">";

for ($i=1; $i<7; $i++) {
  echo "<OPTION label=\"crate_$i\" value=\"$i\" ";
  if ( $i == $crate_nr ) {
    echo " selected ";
  }
  echo ">$i</OPTION>\n";
}

echo "</OPTGROUP>";
echo "</SELECT>";


?>
