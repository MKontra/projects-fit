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
if(isset($_POST["delindices"]))
{

  foreach ($_POST["delindices"] as $key=>$value)
  {
     $result = mysql_query("DELETE FROM ZahrnuteSL WHERE cisloZS='".$key."'");
     header( 'Location: index.php?p=list_service&cisloP='.isset($_POST["cisloP"]) );
  }

}
else
if ( !isset($_GET["cisloP"]) && !isset($_POST["cisloP"]) )
{
  echo '<form action="index.php?p=list_service" method="post">';
  echo 'Zvolte pobyt:<br/>';
    echo '
      <select name="cisloP">
      ';
      
                                        
    $checkresults = mysql_query("SELECT Pobyt.cisloP
                                  FROM Pobyt ");
    if($checkresults)
    {
     while($row = mysql_fetch_array($checkresults))
     {  

        echo '<option value="'.$row["cisloP"].'">'.$row["cisloP"].'</option>' ;
     }
    } else
    {
      //dberror ?
    } 
    echo '
      </select>
      ';
  echo '<input type="submit" value="Vyber..."/>';    
  echo '</form>'; 
}
else
{
  echo '<form action="list_service.php" method="post">';
  $cip = '';
  if ( isset($_GET["cisloP"]) )
     $cip = $_GET["cisloP"];
     
  if ( isset($_POST["cisloP"]) )
     $cip = $_POST["cisloP"];
       
  echo '<input type="hidden" name="cisloP" value="'.$cip.'">';
      $checkresults = mysql_query("SELECT *
                                   FROM ZahrnuteSL NATURAL JOIN Sluzby
                                   WHERE ZahrnuteSL.cisloS = Sluzby.cisloS
                                   AND ZahrnuteSL.cisloP='".$cip."'");
      echo 'Sluzby pobytu: '. $cip . '<br/>';
      if($checkresults)
      {
         echo '<table class="sample">';
      
         echo '<tr>';
         echo '<td></td>';
         echo '<td>Cislo sluzby</td>';
         echo '<td>Cena</td>';
         echo '</tr>'; 
         while($row = mysql_fetch_array($checkresults)) 
         {
           echo '<tr>';
           echo'<td><input type=checkbox name="delindices['.$row['cisloZS'].']" value="'.$row['cisloZS'].'"></td>';
           echo '<td><a href="index.php?p=services&show=&cisloS='.$row['cisloS'].'">'.$row['cisloS'].'</a></td><td>'.$row['cena'].'</td>';
           echo '</tr>'; 
         }  
         echo '</table>';                
      }
   echo '<input type="submit" value="Zmaz oznacene"/>';
  echo '</form>';    
}
?>
 