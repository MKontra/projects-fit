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
if(isset($_GET["uid"]))
{
  //show details
  header( 'Location: index.php?p=ubytovanie_ins&show=&uid='.$_GET["uid"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Pobyt WHERE cisloP='".$key."'");
     header( 'Location: index.php?p=ubytovanie' );
  }

} else
{
  //show table
  if ( isset($_GET["orderby"]) )
  {
    if ( isset ($_GET["asc"]) && $_GET["asc"] == "desc" )
    {
      $result = mysql_query("SELECT * FROM Pobyt ORDER BY ".$_GET["orderby"]." DESC");
      $_GET["asc"] = "asc" ;
    }
    else
    {
      $result = mysql_query("SELECT * FROM Pobyt ORDER BY ".$_GET["orderby"]);
      $_GET["asc"] = "desc" ;
    }   
  }
  else
    $result = mysql_query("SELECT * FROM Pobyt");
  
  echo '<form action="ubytovanie.php" method="post">';
  
  echo '<table class="sample">';
     //cisloP 	cisloOP 	zaciatokP 	trvanieP 	cisloIZ
     echo '<tr>';
     echo '<td></td>';
     echo '<td>Cislo pobytu:</td><td>Cislo OP Zakaznika</td>';
     echo '<td><a href="index.php?p=ubytovanie&orderby=zaciatokP&asc='.(isset($_GET["asc"]) ? $_GET["asc"] : "asc").'">Datum zaciatku</a></td>';
     echo '<td>Trvanie pobytu</td>';
     echo '<td>Cislo izby</td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
    if ( isset($_GET["filterby"]) && isset($_GET["filterval"]) )
    {
       if ( $row[$_GET["filterby"]] != $_GET["filterval"]) 
          continue;
    }
     echo '<tr>';
     echo '<td><input type=checkbox name="delindices['.$row['cisloP'].']" value="'.$row['cisloP'].'"></td>';
     echo '<td><a target="_top" href="index.php?p=ubytovanie_ins&show=&cip='.$row['cisloP'].'">'.$row['cisloP'].'</a></td>';
     echo '<td><a target="_top" href="index.php?p=zakaznik&show=&cop='.$row['cisloOP'].'">'.$row['cisloOP'].'</a></td>';
     echo '<td>'.$row['zaciatokP'].'</td>';
     echo '<td>'.$row['trvanieP'].'</td>'; 
     //izba - treba doplnit
     echo '<td><a target="_top" href="index.php?p=rooms&show=&cisloIZ='.$row['cisloIZ'].'">'.$row['cisloIZ'].'</a></td>';
     echo '</tr>'; 
   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';    
  echo '</form>';
}
?>
