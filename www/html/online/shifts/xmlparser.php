<?php


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
    ${$pelement} = ${$pelement}.$data;
  }
  //echo "cdata: [$pelement,${$pelement}]<br>";

}


?>




