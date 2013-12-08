<?php

$confirmed = $_GET['confirmed'];
$file      = $_GET['file'];

if ( $confirmed == 'Y' ) {
  if  (unlink($file) ) {
    echo "File [$file] deleted successfully. "; 
  } else {
    echo "<b><font color=\"#FF0000\">ERROR!</font></b>. Cannot delete file $file. Please, check permissions. ";
  }
    echo "Click <a href=\"makelist.php\">here</a> to return to the list of checklists.";
} else {
  echo "Are you sure you want to delete file $file? <a href=\"file_delete.php?file=$file&confirmed=Y\">____YES____</a>, <a href=\"checklist_view.php?file=$file\">no</a>";
}

?>
