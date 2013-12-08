<?php

include('fields.php');

foreach ($fields as $field) {
  ${$field}    = $_POST[$field];
  //echo "$field: ${$field} <br>";
}

$mode=$_POST['mode'];
$file=$_POST['file'];
$date=$_POST['DATE'];
$file_mtime=$_POST['file_mtime'];

/*** if mode is new, make sure that file does not exist ***/
if ( $mode == 'new' ) {
  $ymd = split("-",$date);
  $year  = $ymd[0];
  $mon   = $ymd[1];
  $day   = $ymd[2];
  $file = "xml/".$year.$mon.$day.".xml";
  if ( file_exists($file) ) {
    echo "<b><font color=\"#FF0000\">ERROR!</font></b><br>";
    echo "File [$file] exist! Please, delete it first!<br>";
    echo "Press \"Go Back\" button in your browser to return to form<br>";    
    die();    
  }
}

/*** if mode is 'edit', check modification time ***/
if ( $mode == 'edit' ) {
  $file_status = stat( $file );
  if ( $file_status[9] != $file_mtime ) {
    echo "<b><font color=\"#FF0000\">ERROR!</font></b><br>";
    echo "file [$file] was modified! Old time: [$file_mtime], new time: [$file_status[9]].<br>";
    echo "Please reload file and repeat your modifications!<br>";
    echo "Or click <a href=\"form_view.php\">here</a> to return to the list of shifts.<br>";
    die();
  }
}

if ( !($fd=fopen($file,'w') ) ) {
  die("could not open file $file for writing");
}
fwrite($fd,'<?xml version="1.0"?>'."\n");
fwrite($fd,'<SHIFTLIST>'."\n");

$isOk=TRUE;
foreach ($fields as $field) {
  ${$field}=str_replace("<","&lt;",${$field});
  ${$field}=str_replace(">","&gt;",${$field});
  $isOK = fwrite($fd,"    <$field>${$field}</$field>\n");
  if ( !$isOK ) break;
}

fwrite($fd,'</SHIFTLIST>');

fclose($fd);

if ( !$isOk ) {
  echo "***ERROR writing to the file [$file]!";
} else {
  echo "Shiftlist was updated successfully.<br>";
  echo "Click <a href=\"form_view.php\">here</a> to return to the list of shifts.<br>";
}

?>

