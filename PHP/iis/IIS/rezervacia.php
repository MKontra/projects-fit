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
if(isset($_GET["rid"]))
{
  //show details
  header( 'Location: index.php?p=rezervacia_ins&show=&rid='.$_GET["rid"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Rezervacia WHERE cisloREZ='".$key."'");
     header( 'Location: index.php?p=rezervacia' );
  }

} else
{
  //show table
  if ( isset($_GET["orderby"]) )
  {
    if ( isset ($_GET["asc"]) && $_GET["asc"] == "desc" )
    {
      $result = mysql_query("SELECT * FROM Rezervacia ORDER BY ".$_GET["orderby"]." DESC");
      $_GET["asc"] = "asc" ;
    }
    else
    {
      $result = mysql_query("SELECT * FROM Rezervacia ORDER BY ".$_GET["orderby"]);
      $_GET["asc"] = "desc" ;
    }   
  }
  else
    $result = mysql_query("SELECT * FROM Rezervacia");
  
  echo '<form action="rezervacia.php" method="post">';
  
  echo '<table class="sample">';
     //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR
     echo '<tr>';
     echo '<td></td>';
     echo '<td>Cislo rezervacie:</td><td>Cislo OP Zakaznika</td><td>Cislo izby</td><td>Pocet Lozok</td>';
     echo '<td><a href="index.php?p=rezervacia&orderby=datumR&asc='.(isset($_GET["asc"]) ? $_GET["asc"] : "asc").'">Datum rezervacie</a></td>';
     echo '<td>Dlzka rez.</td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
      if ( isset($_GET["filterby"]) && isset($_GET["filterval"]) )
      {
         if ( $row[$_GET["filterby"]] != $_GET["filterval"]) 
            continue;
      }
     echo '<tr>';
     echo '<td><input type=checkbox name="delindices['.$row['cisloREZ'].']" value="'.$row['cisloREZ'].'"></td>';
     echo '<td><a target="_top" href="index.php?p=rezervacia&show=&rid='.$row['cisloREZ'].'">'.$row['cisloREZ'].'</a></td>';
     echo '<td><a target="_top" href="index.php?p=zakaznik&show=&cop='.$row['cisloOP'].'">'.$row['cisloOP'].'</a></td>';
     //izba - treba doplnit
     echo '<td><a target="_top" href="index.php?p=rooms&show=&cisloIZ='.$row['cisloIZ'].'">'.$row['cisloIZ'].'</a></td>';
     echo '<td>'.$row['pocetL'].'</td>';
     echo '<td>'.$row['datumR'].'</td>';
     echo '<td>'.$row['dlzkaR'].'</td>';
     echo '</tr>'; 
   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';    
  echo '</form>';
}
?>
