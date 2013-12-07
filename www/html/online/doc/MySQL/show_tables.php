<?php

include('../../common/db_online.php');

echo "<h2>List of tables in the database \"$mysql_db_online\"</h2>";

if (!mysql_connect($mysql_host, $mysql_user,$mysql_password)) {
  //echo 'Could not connect to mysql';
  die('Could not connect to mysql: ' . mysql_error());
  exit;
}

$sql = "SHOW TABLES FROM $mysql_db_online";
$result = mysql_query($sql);

if (!$result) {
   echo "DB Error, could not list tables\n";
   echo 'MySQL Error: ' . mysql_error();
   exit;
}

mysql_select_db($mysql_db_online);

while ($row = mysql_fetch_row($result)) {
   echo "<b>Table: <font color=\"#0000FF\">$row[0]</font></b><br>\n";
   
   $result_i = mysql_query("describe  $row[0]");

   $n_fields = mysql_num_fields($result_i);

   echo "<table border=\"1\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n";
   echo "<tr bgcolor=\"#A0FFA0\">";   
   for ($i=0; $i < $n_fields; $i++) {
     print "<th>".mysql_field_name($result_i, $i)."</th>";
   }
   echo "</tr>\n";

   while ($row_i = mysql_fetch_row($result_i)) {
     //echo " ---> $row_i[0]<br>\n";     
     echo "<tr>";
     for ($f=0; $f < $n_fields; $f++) {
       echo "<td>$row_i[$f]</td>";  
     }
     echo "</tr>\n";
   }
   echo "</table><p>";

   mysql_free_result($result_i);

}

mysql_free_result($result);

?> 
