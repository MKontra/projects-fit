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
if(isset($_GET["meno"]))
{
  //show details
  header( 'Location: index.php?p=users_ins&show=&meno='.$_GET["meno"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Opravnenia WHERE meno='".$key."'");
     header( 'Location: index.php?p=users' );
  }

} else
{
  //show table
  $result = mysql_query("SELECT * FROM Opravnenia");
  
  echo '<form action="users.php" method="post">';
  
  echo '<table class="sample">';
  
     echo '<tr>';
     echo'<td></td>';
     echo '<td>Meno:</td><td>Heslo</td><td>Privilegia</td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
     echo '<tr>';
     echo'<td><input type=checkbox name="delindices['.$row['meno'].']" value="'.$row['meno'].'"></td>';
     echo '<td><a href="index.php?p=users&show=&meno='.$row['meno'].'">'.$row['meno'].'</a></td><td>'.$row['heslo'].'</td><td>'.$row['prava'].'</td>';
     echo '</tr>'; 
   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';    
  echo '</form>';
}
?>   