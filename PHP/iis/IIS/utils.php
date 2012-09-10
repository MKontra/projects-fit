<?php      
  function checkInactivity()
  {
    $current = time();
    $last = $_SESSION["lastaction"];
    
    if ( ($current - $last) > 600 )
      require 'logout.php';
    else
      $_SESSION["lastaction"] = $current; 
  
  }
?>