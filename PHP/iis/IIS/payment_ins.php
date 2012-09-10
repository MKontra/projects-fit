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
if( (isset($_GET["show"])) && (!isset($_GET["cisloPL"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=payment' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["cisloPL"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";
    //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR
    $oldfields["cisloPL"] = $cisloPL = mysql_escape_string($_POST["cisloPL"]);
    $oldfields["copz"] = $copz = mysql_escape_string($_POST["copz"]);
    $oldfields["cip"] = $cip = mysql_escape_string($_POST["cip"]);
    $oldfields["suma"] = $suma = mysql_escape_string($_POST["suma"]);    
    $oldfields["datumPL"] = $datumPL = mysql_escape_string($_POST["datumPL"]);
    $oldfields["spl"] = $spl = mysql_escape_string($_POST["spl"]);
    //Foreign key chceky
    
    $ok = true;
    
      $checkresults = mysql_query("SELECT * FROM Zakaznik WHERE cisloOP='".$copz."'");
      if($checkresults)
      {
          if(mysql_num_rows($checkresults)==0)
          {
            $ok == false;
            $oldfields["copz"] = "ZLA HODNOTA";
          }
        }  else
        {
            $ok == false;
            $oldfields["copz"] = "ZLA HODNOTA";
        } 
           

    
      $checkresults = mysql_query("SELECT * FROM Pobyt WHERE cisloP='".$cip."'");
      if($checkresults)
      {
          if(mysql_num_rows($checkresults)==0)
          {
            $ok == false;
            $oldfields["cip"] = "ZLA HODNOTA";
          }
        }  else
        {
            $ok == false;
            $oldfields["cip"] = "ZLA HODNOTA";
        } 
    
    //regex cheky
    if(!preg_match("/^[0-9]{1,10}$/", $suma))
    {
      $oldfields["suma"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    //kontrola poctu volnych miest na izbe
    
    if(!preg_match("/^[0-9]{4}[-]{1}[0-9]{1,2}[-]{1}[0-9]{1,2}/", $datumPL))
    {
      $oldfields["datumPL"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    $asflds = date_parse($datumPL);
    if ( !checkdate ($asflds["month"],$asflds["day"],$asflds["year"]) )
    {
      $oldfields["datumPL"] = "ZLA HODNOTA";
      $ok = false; 
    }

    if(!preg_match("/^[\s\S]{0,20}$/", $spl))
    {
      $oldfields["spl"] = "ZLA HODNOTA";
      $ok = false;
    }
          
    if($ok == true){

      $checkresults = mysql_query("SELECT * FROM Platba WHERE cisloPL='".$_POST["cisloPL"]."'");
      if($checkresults)
      {
        //zaznam xistuje - updatuj
        $row = mysql_fetch_row ($checkresults);
        mysql_query("DELETE FROM Platba WHERE cisloPL='".$_POST["cisloPL"]."'") or die(mysql_error());         
                  
                  
      }
      //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR  
      mysql_query("INSERT INTO Platba 
                  (cisloPL, datumPL, suma, sposobPL, cisloOP, cisloP ) 
                  VALUES('$cisloPL', '$datumPL', '$suma', '$spl', '$copz','$cip' ) ") 
                  or die('insert failed'.mysql_error()) or die(mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=payment') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=payment_ins&badform=rezervacia');  
    }

} 
else
{

  //zobraz folmular obsahujuci data
  
  $copdisabled = false;
  $txtdata["cisloPL"] = "";
  $txtdata["copz"] = "";
  $txtdata["cip"] = "";
  if ( isset($_GET["cisloP"]) )
      $txtdata["cip"] = $_GET["cisloP"];      
  $txtdata["suma"] = "";
  if ( isset($_GET["suma"]) )
      $txtdata["suma"] = $_GET["suma"];
  $txtdata["spl"] = "";
  $txtdata["datumPL"] = ""; 
   
  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Platba WHERE cisloPL='".$_GET["cisloPL"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    
      $txtdata["cisloPL"] = $row[0];
      $txtdata["datumPL"] = $row[1];
      $txtdata["suma"] = $row[2];
      $txtdata["spl"] = $row[3];
      $txtdata["copz"] = $row[4];
      $txtdata["cip"] = $row[5];

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
    <form action="payment_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if (true)
  {
      echo '<tr><td>Cislo platby[automaticke - nemozno menit]*:</td><td> <input type="text" name="ridronly" disabled=true value="'.$txtdata["cisloPL"].'"/></td></tr>';
      echo '<input type="hidden" name="cisloPL" value="'.$txtdata["cisloPL"].'"/>';
  }
  else
  {
      echo '<tr><td>Cislo platby*:</td><td> <input type="text" name="cisloPL" value="'.$txtdata["cisloPL"].'"/></td></tr>';
  }
         
    echo '
      <tr><td>Cislo op*:</td>
      <td>
      <select name="copz">
      ';
      if ( $txtdata["copz"] == "ZLA HODNOTA" )
      {
        echo '<option selected="selected" value="gdfgf">"ZLA HODNOTA"</option>'  ;
      }
      
    
                                        
    $checkresults = mysql_query("SELECT Zakaznik.cisloOP 
                                  FROM Zakaznik ");
    if($checkresults)
    {
     while($row = mysql_fetch_array($checkresults))
     {  
      if ( $row["cisloOP"] == $txtdata["copz"] )
        echo '<option selected="selected" value="'.$row["cisloOP"].'">'.$row["cisloOP"].'</option>'  ;
      else
        echo '<option value="'.$row["cisloOP"].'">"'.$row["cisloOP"].'"</option>'  ;
     }
    } else
    {
      //dberror ?
    } 
    echo '
      </select>
      </td>
      </tr>
      ';

      //Vyber izby

    echo '
      <tr><td>Cislo pobytu*:</td>
      <td>
      <select name="cip">
      ';
      
      if ( $txtdata["cip"] == "ZLA HODNOTA" )
      {
        echo '<option selected="selected" value="gdfgf">"ZLA HODNOTA"</option>'  ;
      }
                                        
    $checkresults = mysql_query("SELECT Pobyt.cisloP
                                  FROM Pobyt ");
    if($checkresults)
    {
     while($row = mysql_fetch_array($checkresults))
     {  
      if ( $row["cisloP"] == $txtdata["cip"] )
        echo '<option selected="selected" value="'.$row["cisloP"].'">"'.$row["cisloP"].'"</option>'  ;
      else
        echo '<option value="'.$row["cisloP"].'">'.$row["cisloP"].'</option>'  ;
     }
    } else
    {
      //dberror ?
    } 
    echo '
      </select>
      </td>
      </tr>
      ';
/**
 *      $txtdata["pocetl"] = $row[3];
      $txtdata["datumR"] = $row[4];
      $txtdata["dlzkaR"] = $row[5];
      **/

  echo '
    <tr><td>suma*:</td><td> <input type="text" name="suma" value="'.$txtdata["suma"].'"/></td></tr>
    <tr><td>Datum platby*:</td><td> <input type="text" name="datumPL" value="'.$txtdata["datumPL"].'"/></td></tr>
    <tr><td>Sposob platby*:</td><td> <input type="text" name="spl" value="'.$txtdata["spl"].'"/></td></tr>
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen platbu"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      datum sa zadava v tvare YYYY-MM-DD</br>   
    </form>
    
    ';
  }
    


?>
