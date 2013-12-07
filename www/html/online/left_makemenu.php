<?php

$fd = fopen($menufile, "r");
$id=0;
if ( $fd ) {

  $namemode=0;
  $title="";
  $handle="";
  $dirname="";
  $macroname="";
  
  while (!feof($fd)) {
    
    $char = fgetc($fd);
    //$name = fgets($fd, 4096);
     //$path = fgets($fd, 4096);

    if ( $char == "\"" ) {
      $namemode += 1;
      if ( $namemode == 8 ) {
	$namemode = 0;

	$handleparam="";
	/**
	if ( $handle == "process_run.php" )
	  $handleparam="&run_nr=0";
	else if ( $handle == "process_run_crate_slotrange.php" )
	  $handleparam="&run_nr=0&crate_nr=1&slot1_nr=2&slot2_nr=3";
	else if ( $handle == "process_run_SNrange.php" )
	  $handleparam="&run_nr=0&SN1_nr=1&SN2_nr=3";
	else if ( $handle == "process_run_ledbox.php" )
	  $handleparam="&run_nr=0&ledbox_nr=1";
	else if ( $handle == "process_run_pairnumber.php" )
	  $handleparam="&pair_nr=1";
	else if ( $handle == "process_slow.php" )
	  $handleparam="&rollback=$rollback";
	else if ( $handle == "process_trend.php" )
	  $handleparam="&run_nr_1=0&run_nr_2=0&rollback=2";
	**/

	if ( $handle == "process_run.php" )
	  ;
	else if ( $handle == "process_run_crate_slotrange.php" )
	  $handleparam="&crate_nr=1&slot1_nr=2&slot2_nr=3";
	else if ( $handle == "process_run_SNrange.php" )
	  $handleparam="&SN1_nr=1&SN2_nr=3";
	else if ( $handle == "process_run_ledbox.php" )
	  $handleparam="&ledbox_nr=1";
	else if ( $handle == "process_run_pairnumber.php" )
	  $handleparam="&pair_nr=1";
	else if ( $handle == "process_slow.php" )
	  $handleparam="&rollback=$rollback";
	else if ( $handle == "process_trend.php" )
	  $handleparam="&run_nr_1=0&run_nr_2=0&rollback=2";

	//echo "<INPUT type=\"radio\" name=\"menuitem\" disabled></INPUT><a href=\"$handle?title=$title&dir=$dirname&macro=$macroname$handleparam\" target=\"framemain\" onclick=\"standardClick($id,'$handle?title=$title&dir=$dirname&macro=$macroname$handleparam');\">$title</a><br>\n";

        echo "<INPUT type=\"radio\" name=\"menuitem\" disabled></INPUT><span class='menu' onclick=\"standardClick($id,'$handle?title=$title&dir=$dirname&macro=$macroname$handleparam');\">$title</span><br>\n";

	$id++;
      }
      continue;
    } else if (  $char == "\n" ) {
      $namemode=0;
      $title="";
      $handle="";
      $dirname="";
      $macroname="";      
    }

    if ( $namemode == 1)
      $title=$title.$char;
    else if ( $namemode == 3 )
      $handle=$handle.$char;
    else if ( $namemode == 5 )
      $dirname=$dirname.$char;
    else if ( $namemode == 7 )
      $macroname=$macroname.$char;
    
    //echo $name;
    //echo $path;
   }
  
  fclose($fd);
}
else
{
  die( "fopen failed for $filename" ) ;
}
?>
