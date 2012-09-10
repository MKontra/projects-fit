<?php 
session_start();                                         
require_once 'connect.php';
require_once 'utils.php';

checkInactivity();

if ( !isset($_SESSION["login"]) )
{
  header( 'Location: index.php' ) ;
}
else
if ( !isset($_GET["tablename"]) && !isset($_POST["tablename"]) )
{
   echo "Select table to export as XML file:<br/>";
   
   echo '<form action="generateXml.php" method="post">';
   
   echo '<select name="tablename">';
   
   echo '<option value="Zakaznik">Zakaznici</option>' ;
   echo '<option value="Pobyt">Pobyty</option>' ;
   echo '<option value="Rezervacia">Rezervacie</option>' ;
   echo '<option value="Platba">Platby</option>' ;   

   if ( $_SESSION["prava"] == 1 )
   {
     echo '<option value="Izba">Izby</option>' ;
     echo '<option value="Opravnenia">Loginy a opravnenia</option>' ;
     echo '<option value="Platba">Sluzby</option>' ;  
   }
   
   echo '</select>';
   
   echo '<input type="submit" value="Exportuj..."/>'; 
   echo '</form>';


}
else
{
   echo '<form action="generateXml.php" method="post">';
   echo '<input name="tablename" value="'.(isset($_POST["tablename"])? $_POST["tablename"] : $_GET["tablename"] ).'"/>';
   echo '<input type="submit" value="Exportuj..."/>'; 
   echo '</form>';
}
?>

