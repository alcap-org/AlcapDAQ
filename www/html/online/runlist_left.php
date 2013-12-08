<?php

if ( $run_nr_manual > 0 ) {
  $run_nr = $run_nr_manual;
}

$rootdir = "root";
/*** open directory ***/
$cfiles = array();
$files  = array();
$nruns=0;
if ( $handle = opendir($rootdir) ) {
  while ( ($file = readdir($handle)) != false ) {
    //    if ( strncmp($file,"his",3) ) continue;
    if ( strcmp(substr($file,5),".root") ) continue;          
    $files[$nruns] = $file;    
    $nruns++;
  }    
  closedir($handle);  
} else {
  echo "ERROR! Cannot  open root directory";
  die();
}

rsort($files);
$id=0;
foreach ($files as $file) {
    $cruns [$id]=substr($file,0,5);
    sscanf($cruns[$id],"%05d",$iruns[$id]);
    $id++;
}


echo "Run <SELECT name='run_nr' onchange='RunSelected();'>";
echo "<OPTGROUP label='Run #' >";

$id=0;
if (  $run_nr_manual > 0 ) {
  echo "<OPTION label=\"run_0\" value=\"0\">   </OPTION> ";
} else {
  //  echo "<OPTION label=\"run_0\" value=\"0\">on-line</OPTION> ";
}
foreach ($cruns as $crun) {
  echo "<OPTION label=\"run_$crun\" value=\"$iruns[$id]\"";
  if ($iruns[$id] == $run_nr) 
    echo " selected ";
  echo ">$crun</OPTION>\n";
  $id++;
  if ( $id > 500 ) break;
}


echo "</OPTGROUP>";
echo "</SELECT>";

echo "<INPUT type=\"text\" name=\"run_nr_manual\" maxlength=\"6\" size=\"6\" value=\"$run_nr_manual\"> ";

?>
