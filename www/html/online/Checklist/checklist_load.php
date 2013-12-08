<?php

$file=$_GET['file'];

$isnewelement=0;
$pelement="";

// First we define the handler functions to inform the parser what action to
// take on encountering a specific type of node.
// We'll just print out element opening and closing tags and character data
// The handler for element opening tags
function startElementHandler($parser, $name, $attribs) {  
  global $pelement;
  global $isnewelement;
  global ${$name};

  //echo("&lt;$name&gt;<BR>");  

  $pelement=$name;
  $isnewelement=1;
  
  //echo "pelement: [$pelement,${$pelement}]";
}
// The handler for element closing tags
function endElementHandler($parser, $name) {
  //echo("&lt;/$name&gt;<BR>");
  global $isnewelement;
  $isnewelement=0;
}
   // The handler for character data
function cdataHandler($parser, $data) {

  global $pelement;
  global $isnewelement;
  global ${$pelement};  

  //echo("$data<BR>");

  //echo "cdata: [$pelement,${$pelement}] isnew: $isnewelement<br>";
  if ( $isnewelement == 1 ) {
    //echo "changing element!<br>";
    $data = htmlspecialchars( $data );
    //$data = htmlentities($data);
    //$data = htmlspecialchars(htmlspecialchars( $data ));
    ${$pelement} = ${$pelement}.$data;
  }
  //echo "cdata: [$pelement,${$pelement}]<br>";

}
// Now we create the parser
$parser=xml_parser_create();

// Register the start and end element handlers
xml_set_element_handler($parser, "startElementHandler", "endElementHandler");

// Register the character data parser
xml_set_character_data_handler($parser, "cdataHandler");

if (!($fp = fopen($file, "r"))) {
  die("could not open $file for reading");
}
// Read chunks of 4K from the file, and pass it to the parser
while ($data = fread($fp, 4096)) {
  //if ( $isnewelement == 1 ) { 
  //$data = htmlspecialchars( $data );
  //echo $data;
  //echo $data;
  if (!xml_parse($parser, $data, feof($fp))) {
    die(sprintf("XML error %d %d", xml_get_current_line_number($parser),
		xml_get_current_column_number($parser)));
  }
}
fclose($fp);

xml_parser_free($parser);

?>
