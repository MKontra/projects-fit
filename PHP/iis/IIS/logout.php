<?php      
  session_start();
  unset($_SESSION["login"]);
  unset($_SESSION["prava"]);
  unset($_SESSION["lastaction"]);
  header( 'Location: index.php' ) ;
?>