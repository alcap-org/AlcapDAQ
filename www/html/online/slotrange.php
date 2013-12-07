<?php

echo " Slots ";
echo "<SELECT name=\"slot1_nr\">";
echo "<OPTGROUP label=\"slot1 #\">";

for ($i=2; $i<21; $i++) {
  echo "<OPTION label=\"slot1_$i\" value=\"$i\" ";
  if ( $i == $slot1_nr ) {
    echo " selected ";
  }
  echo ">$i</OPTION>\n";
}

echo "</OPTGROUP>";
echo "</SELECT>";
echo " -- ";
echo "<SELECT name=\"slot2_nr\">";
echo "<OPTGROUP label=\"slot2 #\">";

for ($i=2; $i<21; $i++) {
  echo "<OPTION label=\"slot2_$i\" value=\"$i\" ";
  if ( $i == $slot2_nr ) {
    echo " selected ";
  }
  echo ">$i</OPTION>\n";
}

echo "</OPTGROUP>";
echo "</SELECT>";

?>
