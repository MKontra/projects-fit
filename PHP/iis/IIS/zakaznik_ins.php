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
if( (isset($_GET["show"])) && (!isset($_GET["cop"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=zakaznik' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["cop"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";

    $oldfields["cop"] = $cop = mysql_escape_string($_POST["cop"]);
    $oldfields["meno"] = $meno = mysql_escape_string($_POST["meno"]);
    $oldfields["priezvisko"] = $priezvisko = mysql_escape_string($_POST["priezvisko"]);
    
    if(isset($_POST["add"]))
      $add = $oldfields["add"] = mysql_escape_string($_POST["add"]);
    else
    {
      $add = $oldfields = 'no address';
      $oldfields["add"] = 'no address';
    }
    
    if(isset($_POST["tel"]) && $_POST["tel"] != '')
      $tel = $oldfields["tel"] = mysql_escape_string($_POST["tel"]);
    else
    {
      $tel = $oldfields = '99999999';
      $oldfields["tel"] = '99999999';
    }
  
    $ok = TRUE;
    if(!preg_match("/^[A-Z]{2}[0-9]{6}$/", $cop))
    {
      $oldfields["cop"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[\S]{1,20}$/", $meno))
    {
      $oldfields["meno"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[\S]{1,20}$/", $priezvisko))
    {
      $oldfields["priezvisko"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[\S\s]{0,50}$/", $add))
    {
      $oldfields["add"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[\d]{0,20}$/", $tel))
    {
      $oldfields["tel"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    if($ok == TRUE){

      $checkresults = mysql_query("SELECT * FROM Zakaznik WHERE cisloOP='".$_POST["cop"]."'");
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
        mysql_query("DELETE FROM Zakaznik WHERE cisloOP='".$_POST["cop"]."'");         
                  
                  
      }
        
      mysql_query("INSERT INTO Zakaznik 
                  (cisloOP, meno, priezvisko, adresa, telefon) 
                  VALUES('$cop', '$meno', '$priezvisko', '$add', '$tel' ) ") 
                  or die('insert failed'.mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=zakaznik') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=zakaznik_ins&badform=zakaznik');  
    }

} 
else
{

  //zobraz folmular obsahujuci data
  $copdisabled = false;
  $txtdata["cop"] = "";
  $txtdata["meno"] = "";
  $txtdata["priezvisko"] = "";
  $txtdata["add"] = "";
  $txtdata["tel"] = "";
  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Zakaznik WHERE cisloOP='".$_GET["cop"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    $txtdata["cop"] = $row[0];
    $txtdata["meno"] = $row[1];
    $txtdata["priezvisko"] = $row[2];
    $txtdata["add"] = $row[3];
    $txtdata["tel"] = $row[4];
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
    <form action="zakaznik_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if ($copdisabled)
  {
      echo '<tr><td>cisloOP[rezim editacie - nemozno menit]*:</td><td> <input type="text" name="copronly" disabled=true value="'.$txtdata["cop"].'"/></td></tr>';
      echo '<input type="hidden" name="cop" value="'.$txtdata["cop"].'"/>';
  }
  else
  {
      echo '<tr><td>cisloOP*:</td><td> <input type="text" name="cop" value="'.$txtdata["cop"].'"/></td></tr>';
  }
  echo '
    <tr><td>meno*:</td><td> <input type="text" name="meno" value="'.$txtdata["meno"].'"/></td></tr>
    <tr><td>priezvisko*:</td><td> <input type="text" name="priezvisko" value="'.$txtdata["priezvisko"].'"/></td></tr>
    <tr><td>adresa:</td><td> <textarea rows="6" name="add" />'.$txtdata["add"].'</textarea></td></tr>
    <tr><td>telefon:</td><td> <input type="text" name="tel" value="'.$txtdata["tel"].'"/></td></tr>
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen zakaznika"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      cislo op pozostava z dvoch velkych pismen a 6 cisiel</br>
      meno a priezvisko moze obsahovat len znaky a-z A-Z</br>    
    </form>
    ';
    
  echo 'Asociovane rezervacie:</br>';
  echo '<IFRAME src="index.php?p=rezervacia&nomenu&filterby=cisloOP&filterval='.$txtdata["cop"].'"
      width="80%" height="30%" name="youriframe" frameborder="1" vspace="0" hspace="0"
      marginwidth="0" marginheight="0" scrolling="yes" noresize></IFRAME><br/>';
  
  echo 'Asociovane pobyty:</br>';
  echo '<IFRAME src="index.php?p=ubytovanie&nomenu&filterby=cisloOP&filterval='.$txtdata["cop"].'"   
      width="80%" height="30%" name="youriframe" frameborder="1" vspace="0" hspace="0"
      marginwidth="0" marginheight="0" scrolling="yes" noresize></IFRAME><br/>';   
}


?>
