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
   echo "Select table to import from XML file:<br/>";
   
   echo '<form enctype="multipart/form-data" method="POST" action="index.php?p=import">';
   
   echo '<select name="tablename">';
   
   echo '<option value="zakaznik">Zakaznici</option>' ;
   echo '<option value="pobyt">Pobyty</option>' ;
   echo '<option value="rezervacia">Rezervacie</option>' ;
   echo '<option value="platba">Platby</option>' ;   

   if ( $_SESSION["prava"] == 1 )
   {
     echo '<option value="izby">Izby</option>' ;
     echo '<option value="opravnenia">Loginy a opravnenia</option>' ;
     echo '<option value="platba">Sluzby</option>' ;  
   }
   
   echo '</select><br/><br/>';
   echo 'Select Source XML File<br/>';
   echo '<input name="sourcexml" type="file" /><br /><br />';

   
   echo '<input type="submit" value="Importuj..."/>'; 
   echo '</form>';


}
else
{
  require_once 'connect.php';  
  
  if (!isset($_POST["tablename"]) )
    header("Location:index.php");

  if (!isset($_FILES["sourcexml"]) )
    header("Location:index.php");
  
  if ( $_FILES['sourcexml']['error'] != 0 )
    header("Location:index.php");
   
  $xml = simplexml_load_file($_FILES["sourcexml"]["tmp_name"]); 
  $dom = dom_import_simplexml ($xml); 
  $root = $dom->tagName;
  
  if ( $root != $_POST["tablename"])
    header("Location:index.php");  
  $preparecn = '';
  
  foreach( $xml->columnnames->column as $cn )
    $preparecn = $preparecn.$cn.",";
  $preparecn = substr($preparecn,0,-1);

  foreach( $xml->rows->row as $rval )
  {
     $currdata = '';
     foreach( $rval->val as $enval )
     {
        $currdata .= "'".$enval."',";
     }
     $currdata = substr($currdata,0,-1);
     
     $prepquery = 'INSERT INTO '.$root.'('.$preparecn.') VALUES ('.$currdata.')';
     
     $rs = mysql_query($prepquery);
     if (!$rs)
      echo mysql_error()."<br/>";
     
  } 
}
?>