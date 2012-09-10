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
if(isset($_GET["cop"]))
{
  //show details
  header( 'Location: index.php?p=zakaznik_ins&show=&cop='.$_GET["cop"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Zakaznik WHERE cisloOP='".$key."'");
     header( 'Location: index.php?p=zakaznik' );
  }

} else
{
  //show table
  
  //sort
  if ( isset($_GET["orderby"]) )
  {
    if ( isset ($_GET["asc"]) && $_GET["asc"] == "desc" )
    {
      $result = mysql_query("SELECT * FROM Zakaznik ORDER BY ".$_GET["orderby"]." DESC");
      $_GET["asc"] = "asc" ;
    }
    else
    {
      $result = mysql_query("SELECT * FROM Zakaznik ORDER BY ".$_GET["orderby"]);
      $_GET["asc"] = "desc" ;
    }   
  }
  else
    $result = mysql_query("SELECT * FROM Zakaznik");
  
  echo '<form action="zakaznik.php" method="post">';
  
  echo '<table class="sample">';
  
     echo '<tr>';
     echo'<td></td>';
     echo '<td>Meno:</td>';
     echo '<td><a href="index.php?p=zakaznik&orderby=priezvisko&asc='.(isset($_GET["asc"]) ? $_GET["asc"] : "asc").'">Priezvisko</a></td>';
     echo '<td>COP</td><td>Adresa<td>Tel. cislo</td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
     if ( isset($_GET["filterby"]) && isset($_GET["filterval"]) )
    {
       if ( $row[$_GET["filterby"]] != $_GET["filterval"]) 
          continue;
    }
     echo '<tr>';
     echo'<td><input type=checkbox name="delindices['.$row['cisloOP'].']" value="'.$row['cisloOP'].'"></td>';
     echo '<td>'.$row['meno'].'</td><td>'.$row['priezvisko'].'</td><td><a href="index.php?p=zakaznik&show=&cop='.$row['cisloOP'].'">'.$row['cisloOP'].'</a></td><td>'.$row['adresa'].'</td><td>'.$row['telefon'].'</td>';
     echo '</tr>'; 

   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';   
  echo '</form>';

}
?>                                      