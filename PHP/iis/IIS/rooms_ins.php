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
if( (isset($_GET["show"])) && (!isset($_GET["cisloIZ"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=rooms' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["cisloIZ"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";

    $oldfields["cisloIZ"] = $cisloIZ = mysql_escape_string($_POST["cisloIZ"]);
    $oldfields["pocetL"] = $pocetL = mysql_escape_string($_POST["pocetL"]);
    $oldfields["cena"] = $cena = mysql_escape_string($_POST["cena"]);
     $stav = $oldfields["stav"] = mysql_escape_string($_POST["stav"]);
    if(isset($_POST["vybava"]))
      $vybava = $oldfields["vybava"] = mysql_escape_string($_POST["vybava"]);
    else
    {
      $vybava = $oldfields = 'neni';
      $oldfields["vybava"] = 'neni';
    }
    
    $ok = TRUE;
    if(!preg_match("/^[0-9]{1,3}$/", $cisloIZ))
    {
      $oldfields["cisloIZ"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[0-9]{1,2}$/", $pocetL))
    {
      $oldfields["pocetL"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[0-9]{1,7}$/", $cena))
    {
      $oldfields["cena"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[0-1]{1}$/", $stav))
    {
      $oldfields["stav"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/[\s\S]{0,50}$/", $vybava))
    {
      $oldfields["vybava"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    if($ok == TRUE){

      $checkresults = mysql_query("SELECT * FROM Izba WHERE cisloIZ='".$_POST["cisloIZ"]."'");
      if($checkresults)
      {
        //zaznam xistuje - updatuj
        $row = mysql_fetch_row ($checkresults);
        /**
        mysql_query("UPDATE Zakaznik 
                    SET meno='".$meno."',
                    SET priezvisko='".$priezvisko."',
                    SET adresa='".$add."',
                    SET telefon='".$tel."'                                                                                 
                  WHERE cisloOP='".$_POST["cop"]."'") 
                  or die('insert failed'.mysql_error());
                  **/
        mysql_query("DELETE FROM Izba WHERE cisloIZ='".$_POST["cisloIZ"]."'");         
                  
                  
      }
        
      mysql_query("INSERT INTO Izba 
                  (cisloIZ, pocetL, cena, stav, vybava) 
                  VALUES('$cisloIZ', '$pocetL', '$cena', '$stav', '$vybava' ) ") 
                  or die('insert failed'.mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=rooms') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=rooms_ins&badform=rooms');  
    }

} 
else
{

  //zobraz folmular obsahujuci data
  $copdisabled = false;
  $txtdata["cisloIZ"] = "";
  $txtdata["pocetL"] = "";
  $txtdata["cena"] = "";
  $txtdata["stav"] = "";
  $txtdata["vybava"] = "";
  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Izba WHERE cisloIZ='".$_GET["cisloIZ"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    $txtdata["cisloIZ"] = $row[0];
    $txtdata["pocetL"] = $row[1];
    $txtdata["cena"] = $row[2];
    $txtdata["stav"] = $row[3];
    $txtdata["vybava"] = $row[4];
  }
  else
  if ( isset($_GET["badform"]) )  //nepresiel regchcekmi
  {
    if ( isset($_SESSION['olddata']) )
    {
      $txtdata = $_SESSION['olddata'];
      unset($_SESSION['olddata']);
    }
  }
  
  //zobraz folmular
  
   echo '    
    <form action="rooms_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if ($copdisabled)
  {
      echo '<tr><td>cislo izby[rezim editacie - nemozno menit]*:</td><td> <input type="text" name="copronly" disabled=true value="'.$txtdata["cisloIZ"].'"/></td></tr>';
      echo '<input type="hidden" name="cisloIZ" value="'.$txtdata["cisloIZ"].'"/>';
  }
  else
  {
      echo '<tr><td>cislo izby*:</td><td> <input type="text" name="cisloIZ" value="'.$txtdata["cisloIZ"].'"/></td></tr>';
  }
  echo '
    <tr><td>poèet lôžok*:</td><td> <input type="text" name="pocetL" value="'.$txtdata["pocetL"].'"/></td></tr>
    <tr><td>cena*:</td><td> <input type="text" name="cena" value="'.$txtdata["cena"].'"/></td></tr>
     <tr><td>stav*:</td><td> <input type="text" name="stav" value="'.$txtdata["stav"].'"/></td></tr>
    <tr><td>výbava:</td><td> <textarea rows="6" name="vybava" />'.$txtdata["vybava"].'</textarea></td></tr>
   
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen izbu"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      èíslo izby  1-299</br>
      poèet lôzok 0-99 </br>
      cena        0-9999999 </br>
      stav        1 alebo 0 </br>
      
      vybava popis obsahu izby</br>    
    </form>
    ';
}


?>
