<?php

//echo <<< END
//<table>
//END;

$file=$_GET['file'];
$mode=$_GET['mode'];

include('xmlparser.php');

if ( $mode == "edit" ) {
  echo "<h3>Edit file: $file</h3>";
}

echo <<< END

<table border="1" frame="border" align="center" valign="middle" rules="all">
<tr>
<td>
<form name="shift_table" action="form_save.php" method="POST">
END;

if ( $mode == 'new' ) {
  $DATE = date('Y-m-d');
  echo "Date (yyyy-mm-dd): <INPUT type=\"text\" name=\"DATE\" maxlength=\"10\" size=\"10\" value=\"$DATE\"><br><br>";
}

echo <<< END

<table border="1" frame="border" bgcolor="#FFFFA0" valign="middle" rules="all">
<tr>
  <th>PSI time </th>
  <th>crue</th>
</tr>
END;

/*** read file and build table ***/
$file_mtime = 0; /*** time of last modification ***/
if ( $mode == 'edit' ) {

  $file_status = stat( $file );/*** file stat() before reading ***/

  if (!($fp = fopen($file, "r"))) {
    die("could not open $file for reading");
  }

  $file_mtime = $file_status[9];

  // Now we create the parser
  $parser=xml_parser_create();
  
  // Register the start and end element handlers
  xml_set_element_handler($parser, "startElementHandler", "endElementHandler");
  
  // Register the character data parser
  xml_set_character_data_handler($parser, "cdataHandler");


  // Read chunks of 4K from the file, and pass it to the parser
  while ($data = fread($fp, 4096)) {
    if (!xml_parse($parser, $data, feof($fp))) {
      die(sprintf("XML error %d %d", xml_get_current_line_number($parser),
		  xml_get_current_column_number($parser)));
    }
  }

  fclose($file);
  xml_parser_free($parser);

}

/*** fill table row ***/
$hours = array($HOUR_0,$HOUR_1,$HOUR_2,$HOUR_3,$HOUR_4,$HOUR_5,$HOUR_6,$HOUR_7,$HOUR_8,$HOUR_9,$HOUR_10,
	       $HOUR_11,$HOUR_12,$HOUR_13,$HOUR_14,$HOUR_15,$HOUR_16,$HOUR_17,$HOUR_18,$HOUR_19,$HOUR_20,
	       $HOUR_21,$HOUR_22,$HOUR_23,$HOUR_24);

for ($i=0; $i<24; $i++) {
  $time_start = $i;
  $time_stop  = $i+1;
  echo "<tr>";
  echo "<td align=\"center\"><b>$time_start-$time_stop</b></td>";
  echo "<td><INPUT type='text' name='HOUR_$i' maxlength='20' size='20' value='$hours[$i]'></td>";
  echo "</tr>";
}

echo <<< END
</table>
<br>
<INPUT TYPE="HIDDEN" NAME="mode" VALUE="$mode">
<INPUT TYPE="HIDDEN" NAME="file" VALUE="$file">
<INPUT TYPE="HIDDEN" NAME="file_mtime" VALUE="$file_mtime">
<INPUT type="submit" value="Save">
 | <a href="form_view.php">back to shift table</a> | 
</form>

</td>
<td valign="top">

<h3>Tips:</h3>

If you are taking remote shift, add "*" to your name. <br>
<em>Example:</em> <b>QP*</b>.<br><br>

If you need another person to join you in the shift, put "?" as a name 
of the second person (in particular, if you need help from
off-site people). <br>
<em>Example:</em> <b>VT,?</b>.<br><br>



</td>
</tr>
</table> 

END;




?>



