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
if ( !isset($_SESSION["prava"]) )
{
  header( 'Location: index.php' ) ;
}
else
if ( $_SESSION["prava"] == 0 )
{
  header( 'Location: index.php' ) ;
}
else
if(isset($_GET["cisloIZ"]))
{
  //show details
  header( 'Location: index.php?p=rooms_ins&show=&cisloIZ='.$_GET["cisloIZ"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Izba WHERE cisloIZ='".$key."'");
     header( 'Location: index.php?p=rooms' );
  }

} else
{
  //show table
  $result = mysql_query("SELECT * FROM Izba");
  
  echo '<form action="rooms.php" method="post">';
  
  echo '<table class="sample">';
  
     echo '<tr>';
     echo'<td></td>';
     echo '<td>èíslo izby:</td><td>poèet ložok</td><td>cena</td><td>stav<td>výbava</td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
     echo '<tr>';
     echo'<td><input type=checkbox name="delindices['.$row['cisloIZ'].']" value="'.$row['cisloIZ'].'"></td>';
     echo '<td><a href="index.php?p=rooms&show=&cisloIZ='.$row['cisloIZ'].'">'.$row['cisloIZ'].'</a></td><td>'.$row['pocetL'].'</td><td>'.$row['cena'].'</td><td>'.$row['stav'].'</td><td>'.$row['vybava'].'</td>';
     echo '</tr>'; 
   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';    
  echo '</form>';
}
?>        