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
if( (isset($_GET["show"])) && (!isset($_GET["meno"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=users' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["meno"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";

    $oldfields["meno"] = $meno = mysql_escape_string($_POST["meno"]);
    $oldfields["heslo"] = $heslo = mysql_escape_string($_POST["heslo"]);
    $oldfields["prava"] = $prava = mysql_escape_string($_POST["prava"]);
    
    $ok = TRUE;
    if(!preg_match("/^[A-Za-z0-9]{6,20}$/", $meno))
    {
      $oldfields["meno"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[A-Za-z0-9]{6,20}$/", $heslo))
    {
      $oldfields["heslo"] = "ZLA HODNOTA";
      $ok = false;
    }
    if(!preg_match("/^[0-9]{1}$/", $prava))
    {
      $oldfields["prava"] = "ZLA HODNOTA";
      $ok = false;
    }
        
    if($ok == TRUE){

      $checkresults = mysql_query("SELECT * FROM Opravnenia WHERE meno='".$_POST["meno"]."'");
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
        mysql_query("DELETE FROM Opravnenia WHERE meno='".$_POST["meno"]."'");         
                  
                  
      }
        
      mysql_query("INSERT INTO Opravnenia 
                  (meno, heslo, prava) 
                  VALUES('$meno', '$heslo', '$prava') ") 
                  or die('insert failed'.mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=users') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=users_ins&badform=user');  //KKKKKKKKKK
    }

} 
else
{

  //zobraz folmular obsahujuci data
  $copdisabled = false;
  $txtdata["meno"] = "";
  $txtdata["heslo"] = "";
  $txtdata["prava"] = "";
 
  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Opravnenia WHERE meno='".$_GET["meno"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    $txtdata["meno"] = $row[0];
    $txtdata["heslo"] = $row[1];
    $txtdata["prava"] = $row[2];

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
    <form action="users_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if ($copdisabled)
  {
      echo '<tr><td>meno[rezim editacie - nemozno menit]*:</td><td> <input type="text" name="copronly" disabled=true value="'.$txtdata["meno"].'"/></td></tr>';
      echo '<input type="hidden" name="meno" value="'.$txtdata["meno"].'"/>';
  }
  else
  {
      echo '<tr><td>meno*:</td><td> <input type="text" name="meno" value="'.$txtdata["meno"].'"/></td></tr>';
  }
  echo '
    
    <tr><td>heslo*:</td><td> <input type="text" name="heslo" value="'.$txtdata["heslo"].'"/></td></tr>
    <tr><td>prava*:</td><td> <input type="text" name="prava" value="'.$txtdata["prava"].'"/></td></tr>
    
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen uživate¾a"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      
      meno a heslo moze obsahovat len znaky a-z A-Z 0-9 (minimalne 6 maximalne 20)</br>
      prava: 1-plne privilegia 0-obmedzene moznosti     
    </form>
    ';
}


?>
