<?php        
session_start();
require_once 'connect.php';
require_once 'utils.php';

checkInactivity();

if ( !isset($_SESSION["login"]) )     //neprihlaseny
{
  header( 'Location: index.php' ) ;
}
else
if ( !isset($_GET["cisloP"]) && !isset($_POST["cisloP"]) )
{
  echo '<form action="index.php?p=vyuctuj_pobyt" method="post">';
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
    echo 'Celkove naklady na pobyt:<br/>';
    
  $cip = '';
  if ( isset($_GET["cisloP"]) )
     $cip = $_GET["cisloP"];
     
  if ( isset($_POST["cisloP"]) )
     $cip = $_POST["cisloP"]; 
    
    $checkresults = mysql_query("SELECT * FROM Pobyt WHERE cisloP='".$cip."'") or die(mysql_error());

      //pobyt existuje
      $row = mysql_fetch_row ($checkresults);
      $pocetdni = $row[3];
      $ciz = $row[4];

    $checkresults = mysql_query("SELECT cena FROM Izba WHERE cisloIZ='".$ciz."'") or die(mysql_error());
    
      $row = mysql_fetch_row ($checkresults);
      $pricepnight = $row[0];
    
    $suma =  $pricepnight *  $pocetdni;
      
      $checkresults = mysql_query("SELECT *
                                   FROM ZahrnuteSL NATURAL JOIN Sluzby
                                   WHERE ZahrnuteSL.cisloS = Sluzby.cisloS
                                   AND ZahrnuteSL.cisloP='".$cip."'");

      if($checkresults)
      { 
         while($row = mysql_fetch_array($checkresults)) 
         {
            $suma = $suma + $row["cena"];
         }                  
      }

      echo $suma.'</br></br>';


  echo '<br/><br/><a href="index.php?p=payment_ins&cisloP='.$cip.'&suma='.$suma.'">Kliknite sem pre vytvorenie platby</a><br/>';




}

?>

