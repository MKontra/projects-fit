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
if( (isset($_GET["show"])) && (!isset($_GET["cisloS"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=services' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["cisloS"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";

    $oldfields["cisloS"] = $cisloS = mysql_escape_string($_POST["cisloS"]);
    $oldfields["cena"] = $cena = mysql_escape_string($_POST["cena"]);
    
    $ok = TRUE;
    if(!preg_match("/^[0-9]{1,3}$/", $cisloS))
    {
      $oldfields["cisloS"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[0-9]{1,5}$/", $cena))
    {
      $oldfields["cena"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    
    if($ok == TRUE){

      $checkresults = mysql_query("SELECT * FROM Sluzby WHERE cisloS='".$_POST["cisloS"]."'");
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
        mysql_query("DELETE FROM Sluzby WHERE cisloS='".$_POST["cisloS"]."'");         
                  
                  
      }
        
      mysql_query("INSERT INTO Sluzby 
                  (cisloS, cena) 
                  VALUES('$cisloS', '$cena') ") 
                  or die('insert failed'.mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=services') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=services_ins&badform=services');  
    }

} 
else
{

  //zobraz folmular obsahujuci data
  $copdisabled = false;
  $txtdata["cisloS"] = "";
  $txtdata["cena"] = "";

  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Sluzby WHERE cisloS = '".$_GET["cisloS"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    $txtdata["cisloS"] = $row[0];
    $txtdata["cena"] = $row[1];

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
    <form action="services_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if ($copdisabled)
  {
      echo '<tr><td>èíslo služby[rezim editacie - nemozno menit]*:</td><td> <input type="text" name="copronly" disabled=true value="'.$txtdata["cisloS"].'"/></td></tr>';
      echo '<input type="hidden" name="cisloS" value="'.$txtdata["cisloS"].'"/>';
  }
  else
  {
      echo '<tr><td>èíslo služby*:</td><td> <input type="text" name="cisloS" value="'.$txtdata["cisloS"].'"/></td></tr>';
  }
  echo '
    <tr><td>cena*:</td><td> <input type="text" name="cena" value="'.$txtdata["cena"].'"/></td></tr>
    
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen službu"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      èíslo služby 0-999</br>
      cena 0-99999</br>    
    </form>
    ';
}


?>
