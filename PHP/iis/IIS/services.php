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
if ( !isset($_SESSION["prava"]))
{
  header( 'Location: index.php' ) ;
}
else
if ( $_SESSION["prava"] == 0 )
{
  header( 'Location: index.php' ) ;
}
else
if(isset($_GET["cisloS"]))
{
  //show details
  header( 'Location: index.php?p=services_ins&show=&cisloS='.$_GET["cisloS"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Sluzby WHERE cisloS='".$key."'");
     header( 'Location: index.php?p=services' );
  }

} else
{
  //show table
  
  if ( isset($_GET["orderby"]) )
  {
    if ( isset ($_GET["asc"]) && $_GET["asc"] == "desc" )
    {
      $result = mysql_query("SELECT * FROM Sluzby ORDER BY ".$_GET["orderby"]." DESC");
      $_GET["asc"] = "asc" ;
    }
    else
    {
      $result = mysql_query("SELECT * FROM Sluzby ORDER BY ".$_GET["orderby"]);
      $_GET["asc"] = "desc" ;
    }   
  }
  else
    $result = mysql_query("SELECT * FROM Sluzby");
  
  echo '<form action="services.php" method="post">';
  
  echo '<table class="sample">';
  
     echo '<tr>';
     echo'<td></td>';
     echo '<td><a href="index.php?p=services&orderby=cisloS&asc='.(isset($_GET["asc"]) ? $_GET["asc"] : "asc").'">Cislo sluzby</a></td>';
     echo '<td><a href="index.php?p=services&orderby=cena&asc='.(isset($_GET["asc"]) ? $_GET["asc"] : "asc").'">Cena</a></td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
   
     if ( isset($_GET["filterby"]) && isset($_GET["filterval"]) )
    {
       if ( $row[$_GET["filterby"]] != $_GET["filterval"]) 
          continue;
    }  
   
     echo '<tr>';
     echo'<td><input type=checkbox name="delindices['.$row['cisloS'].']" value="'.$row['cisloS'].'"></td>';
     echo '<td><a href="index.php?p=services&show=&cisloS='.$row['cisloS'].'">'.$row['cisloS'].'</a></td><td>'.$row['cena'].'</td>';
     echo '</tr>'; 
   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';    
  echo '</form>';
}
?>        