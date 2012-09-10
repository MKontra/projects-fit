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
if(isset($_GET["cisloPL"]))
{
  //show details
  header( 'Location: index.php?p=payment_ins&show=&cisloPL='.$_GET["cisloPL"] ) ;

} else
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM Platba WHERE cisloPL='".$key."'");
     header( 'Location: index.php?p=payment' );
  }

} else
{
  //show table
  if ( isset($_GET["orderby"]) )
  {
    if ( isset ($_GET["asc"]) && $_GET["asc"] == "desc" )
    {
      $result = mysql_query("SELECT * FROM Platba ORDER BY ".$_GET["orderby"]." DESC");
      $_GET["asc"] = "asc" ;
    }
    else
    {
      $result = mysql_query("SELECT * FROM Platba ORDER BY ".$_GET["orderby"]);
      $_GET["asc"] = "desc" ;
    }   
  }
  else
    $result = mysql_query("SELECT * FROM Platba");
  
  echo '<form action="payment.php" method="post">';
  
  echo '<table class="sample">';
     //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR
     echo '<tr>';
     echo '<td></td>';
     echo '<td>Cislo platby:</td><td>Cislo OP Zakaznika</td><td>Cislo pobytu</td><td>suma</td>';
     echo '<td><a href="index.php?p=payment&orderby=datumPL&asc='.(isset($_GET["asc"]) ? $_GET["asc"] : "asc").'">Datum platby</a></td>';
     echo '<td>sposob platby</td>';
     echo '</tr>'; 

   while($row = mysql_fetch_array($result))
   {
      if ( isset($_GET["filterby"]) && isset($_GET["filterval"]) )
      {
         if ( $row[$_GET["filterby"]] != $_GET["filterval"]) 
            continue;
      }
     echo '<tr>';
     echo '<td><input type=checkbox name="delindices['.$row['cisloPL'].']" value="'.$row['cisloPL'].'"></td>';
     echo '<td><a href="index.php?p=payment&show=&cisloPL='.$row['cisloPL'].'">'.$row['cisloPL'].'</a></td>';
     echo '<td><a href="index.php?p=zakaznik&show=&cop='.$row['cisloOP'].'">'.$row['cisloOP'].'</a></td>';
     //izba - treba doplnit        
     echo '<td><a href="index.php?p=ubytovanie&show=&cip='.$row['cisloP'].'">'.$row['cisloP'].'</a></td>';
     echo '<td>'.$row['suma'].'</td>';
     echo '<td>'.$row['datumPL'].'</td>';
     echo '<td>'.$row['sposobPL'].'</td>';
     echo '</tr>'; 
   } 
  echo '</table>';
  
  echo '<br/><br/>';
   
  echo '<input type="submit" value="Zmaz oznacene"/>';    
  echo '</form>';
}
?>
