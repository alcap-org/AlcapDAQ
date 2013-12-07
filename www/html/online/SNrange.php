<?php

echo " Serial Numbers ";
echo "<SELECT name=\"SN1_nr\">";
echo "<OPTGROUP label=\"SN1 #\">";

for ($i=1; $i<341; $i++) {
  echo "<OPTION label=\"SN1_$i\" value=\"$i\" ";
  if ( $i == $SN1_nr ) {
    echo " selected ";
  }
  echo ">$i</OPTION>\n";
}

echo "</OPTGROUP>";
echo "</SELECT>";
echo " -- ";
echo "<SELECT name=\"SN2_nr\">";
echo "<OPTGROUP label=\"SN2 #\">";

for ($i=1; $i<341; $i++) {
  echo "<OPTION label=\"SN2_$i\" value=\"$i\" ";
  if ( $i == $SN2_nr ) {
    echo " selected ";
  }
  echo ">$i</OPTION>\n";
}

echo "</OPTGROUP>";
echo "</SELECT>";

?>
