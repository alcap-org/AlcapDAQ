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
    if ( strncmp($file,"his",3) ) continue;
    if ( strcmp(substr($file,8),".root") ) continue;          
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
    $cruns [$id]=substr($file,3,5);
    sscanf($cruns[$id],"%05d",$iruns[$id]);
    $id++;
}


//$cfiles=shell_exec('ls -r root/his??????.root');
// *** convert to array type ***
//$files=split("\n",$cfiles);
// *** extract run numbers from file name ***
//$nruns=0;
//foreach ($files as $file) {
//  if ( $file == "" ) continue;
//  $cruns[$nruns]=substr($file,8,6);
//  sscanf($cruns[$nruns],"%06d",$iruns[$nruns]);
//  $nruns++;
//}


echo "Run <SELECT name=\"run_nr\">";
echo "<OPTGROUP label=\"Run #\">";

$id=0;
if (  $run_nr_manual > 0 ) {
  echo "<OPTION label=\"run_0\" value=\"0\">   </OPTION> ";
} else {
  echo "<OPTION label=\"run_0\" value=\"0\">on-line</OPTION> ";
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

echo " or <INPUT type=\"text\" name=\"run_nr_manual\" maxlength=\"6\" size=\"6\" value=\"$run_nr_manual\"> ";

?>