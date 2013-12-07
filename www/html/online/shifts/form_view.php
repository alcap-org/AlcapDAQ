<?php

//echo <<< END
//<table>
//END;

include('fields.php');

$xmlpath="xml/";

$nfiles=0;
$files = array();
if ( $handle = opendir($xmlpath) ) {
  while ( ($file = readdir($handle)) != false ) {
      //echo "file: $file <br>";
     if ( !strcmp(substr($file,8),".xml") ) {
       //echo "file: $file <br>";
       $files[$nfiles] = $file;
       $nfiles++;
     }
  }
  closedir($handle);
} else {
  echo "***ERROR! Cannot read directory [$xmlpath]";
}

sort($files);

include('xmlparser.php');

echo <<< END
<h3>Our shifts</h3>
<table border="1" frame="border" bgcolor="#FFFFCC" align="center" valign="middle" rules="all">
<COLGROUP width="80*" span="1">
<COLGROUP width="30" span="24">
<THEAD>
<tr>
  <th>date\\time </th>
END;

for ($i=0; $i<24; $i++) {
  $j=$i+1;
  echo "<th>$i-$j</th>";
}
echo "</tr>";



/*** read files and build table ***/
foreach ($files as $file) {
  if ( !($fp = fopen($xmlpath.$file, "r")) ) {
    die("could not open $file for reading");
  }

  /*** reset fields ***/
  foreach ( $fields as $field ) 
    ${$field} = "";

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

  /*** fill table row ***/
  $hours = array($HOUR_0,$HOUR_1,$HOUR_2,$HOUR_3,$HOUR_4,$HOUR_5,$HOUR_6,$HOUR_7,$HOUR_8,$HOUR_9,$HOUR_10,
		 $HOUR_11,$HOUR_12,$HOUR_13,$HOUR_14,$HOUR_15,$HOUR_16,$HOUR_17,$HOUR_18,$HOUR_19,$HOUR_20,
		 $HOUR_21,$HOUR_22,$HOUR_23,$HOUR_24);

  echo "<tr>";
  /*** convert filename to date ***/
  $date_year = substr($file,0,4);
  $date_mon  = substr($file,4,2);
  $date_day  = substr($file,6,2);
  //$date=substr($file,0,4).'-'.substr($file,4,2).'-'.substr($file,6,2);
  $date=$date_year.'-'.$date_mon.'-'.$date_day;
  /*** today's date ***/
  $today = date(date('Y-m-d'));
  $bgcolor="FFFFCC";
  if ( $date == $today ) {
    $bgcolor = "7FFFD4";
  }
  /*** determine date of weak ***/
  $time = mktime(0,0,1,$date_mon,$date_day,$date_year);
  $date_wday = date('D',$time);
  $color="000000";
  if ( $date_wday  == 'Sat' || $date_wday  == 'Sun' ) {
    $color="FF0000";
  }
  echo "<td bgcolor=\"#$bgcolor\"><a href=\"form.php?file=$xmlpath/$file&mode=edit\">$date</a><br><font size=\"-2\" color=\"#$color\"><em>$date_wday</em></font></td>";
  for ($i=0; $i<24; $i++) {
    $bgcolor="00FF00";
    if ( $hours[$i] ==  "" ) {
      //$bgcolor="FF0000";
      //$bgcolor = "FF9933";
      $bgcolor = "0000FF";
    } else {
      /*** people in the shift ***/
      $persons = split(',',$hours[$i]);
      $isLocal = false;      /*** true if there are local people in the shift ***/
      $isHelpNeeded = false; /*** true if help is needed ***/
      $isFree = false;
      foreach ( $persons as $person ) {
	if (  $person === "" ) continue;
	if ( strpos($person,'*') === false ) {
	  //echo "[$person]: Local Shift";
	  $isLocal = true;
	}
	if ( !(strpos($person,"?") === false)  ) {
	  $isHelpNeeded = true;
	}
	if ( $person == '-'  ) {
	  $isFree = true;
	}
      }      
      if ( $isHelpNeeded ) {
	//$bgcolor = "FF9933";
	$bgcolor="FF0000";
      } else if ( $isFree ) {
	$bgcolor = "FAFAFA";
      } else if ( $isLocal ) {
	$bgcolor = "00FF00";
      } else {
	$bgcolor = "FFFF00";
      }
    }
    echo "<td bgcolor=\"#$bgcolor\" align=\"center\"><font size=\"-2\">$hours[$i]</font></td>";
  }
  echo "</tr>"; 

}

echo "<tr><th>time:</th>"; 
for ($i=0; $i<24; $i++) {
  $j=$i+1;
  echo "<th>$i-$j</th>";
}
echo "</tr>";


echo <<< END
</table>
<br>
<form name="AddShift" action="form.php" method="GET">
<INPUT type="submit" value=" New ">
<INPUT TYPE="HIDDEN" NAME="mode" VALUE="new">
</form>

<br>

<table>
<table border="1" frame="border" bgcolor="#FFFFCC" align="left" valign="middle" rules="all">
<tr bgcolor="BBBBBB">
  <th>color</th>
  <th>meaning</th>
</tr>
<tr>
  <td bgcolor="#0000FF">  </td>
  <td>nobody has signed yet. Please, sign. </td>
</tr>
<tr>
  <td bgcolor="#FFFF00" align="center"> <b>*</b> </td>
  <td>only remote people in the shift</td>
</tr>
<tr>
  <td bgcolor="#00FF00">  </td>
  <td>there is onsite person in the shift</td>
</tr>
<tr>
  <td bgcolor="#FF0000"  align="center"> <b>?</b> </td>
  <td>Urgent need to fill slot</td>
</tr>
</table>

<br>
<br>
END;




?>



