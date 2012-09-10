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
if( (isset($_GET["show"])) && (!isset($_GET["rid"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=rezervacia' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["rid"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";
    //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR
    $oldfields["rid"] = $rid = mysql_escape_string($_POST["rid"]);
    $oldfields["copz"] = $copz = mysql_escape_string($_POST["copz"]);
    $oldfields["ciz"] = $ciz = mysql_escape_string($_POST["ciz"]);
    $oldfields["pocetL"] = $pocetL = mysql_escape_string($_POST["pocetL"]);    
    $oldfields["datumR"] = $datumR = mysql_escape_string($_POST["datumR"]);
    $oldfields["dlzkaR"] = $dlzkaR = mysql_escape_string($_POST["dlzkaR"]);
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
           

    
      $checkresults = mysql_query("SELECT * FROM Izba WHERE cisloIZ='".$ciz."'");
      if($checkresults)
      {
          if(mysql_num_rows($checkresults)==0)
          {
            $ok == false;
            $oldfields["ciz"] = "ZLA HODNOTA";
          }
        }  else
        {
            $ok == false;
            $oldfields["ciz"] = "ZLA HODNOTA";
        } 
    
    //regex cheky
    if(!preg_match("/^[0-9]{1,2}$/", $pocetL))
    {
      $oldfields["pocetL"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    //kontrola poctu volnych miest na izbe
    
    if(!preg_match("/^[0-9]{4}[-]{1}[0-9]{1,2}[-]{1}[0-9]{1,2}/", $datumR))
    {
      $oldfields["datumR"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    $asflds = date_parse($datumR);
    if ( !checkdate ($asflds["month"],$asflds["day"],$asflds["year"]) )
    {
      $oldfields["datumR"] = "ZLA HODNOTA";
      $ok = false; 
    }

    if(!preg_match("/^[0-9]{1,3}$/", $dlzkaR))
    {
      $oldfields["dlzkaR"] = "ZLA HODNOTA";
      $ok = false;
    }
          
    if($ok == true){

      $checkresults = mysql_query("SELECT * FROM Rezervacia WHERE cisloREZ='".$_POST["rid"]."'");
      if($checkresults)
      {
        //zaznam xistuje - updatuj
        $row = mysql_fetch_row ($checkresults);
        mysql_query("DELETE FROM Rezervacia WHERE cisloREZ='".$_POST["rid"]."'") or die(mysql_error());         
                  
                  
      }
      //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR  
      mysql_query("INSERT INTO Rezervacia 
                  (cisloREZ, cisloOP, cisloIZ, pocetL, datumR, dlzkaR ) 
                  VALUES('$rid', '$copz', '$ciz', '$pocetL', '$datumR','$dlzkaR' ) ") 
                  or die('insert failed'.mysql_error()) or die(mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=rezervacia') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=rezervacia_ins&badform=rezervacia');  
    }

} 
else
{

  //zobraz folmular obsahujuci data
  
  $copdisabled = false;
  $txtdata["rid"] = "";
  $txtdata["copz"] = "";
  $txtdata["ciz"] = "";
  $txtdata["pocetL"] = "";
  $txtdata["datumR"] = "";
  $txtdata["dlzkaR"] = ""; 
   
  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Rezervacia WHERE cisloREZ='".$_GET["rid"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    
      $txtdata["rid"] = $row[0];
      $txtdata["copz"] = $row[1];
      $txtdata["ciz"] = $row[2];
      $txtdata["pocetL"] = $row[3];
      $txtdata["datumR"] = $row[4];
      $txtdata["dlzkaR"] = $row[5];

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
    <form action="rezervacia_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if (true)
  {
      echo '<tr><td>Cislo rezervacie[automaticke - nemozno menit]*:</td><td> <input type="text" name="ridronly" disabled=true value="'.$txtdata["rid"].'"/></td></tr>';
      echo '<input type="hidden" name="rid" value="'.$txtdata["rid"].'"/>';
  }
  else
  {
      echo '<tr><td>Cislo rezervacie*:</td><td> <input type="text" name="rid" value="'.$txtdata["rid"].'"/></td></tr>';
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
      <tr><td>Cislo izby*:</td>
      <td>
      <select name="ciz">
      ';
      
      if ( $txtdata["ciz"] == "ZLA HODNOTA" )
      {
        echo '<option selected="selected" value="gdfgf">"ZLA HODNOTA"</option>'  ;
      }
                                        
    $checkresults = mysql_query("SELECT Izba.cisloIZ 
                                  FROM Izba ");
    if($checkresults)
    {
     while($row = mysql_fetch_array($checkresults))
     {  
      if ( $row["cisloIZ"] == $txtdata["ciz"] )
        echo '<option selected="selected" value="'.$row["cisloIZ"].'">"'.$row["cisloIZ"].'"</option>'  ;
      else
        echo '<option value="'.$row["cisloIZ"].'">'.$row["cisloIZ"].'</option>'  ;
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
    <tr><td>Pocet rez. lozok*:</td><td> <input type="text" name="pocetL" value="'.$txtdata["pocetL"].'"/></td></tr>
    <tr><td>Datum rezervacie*:</td><td> <input type="text" name="datumR" value="'.$txtdata["datumR"].'"/></td></tr>
    <tr><td>Na aku dlzku*:</td><td> <input type="text" name="dlzkaR" value="'.$txtdata["dlzkaR"].'"/></td></tr>
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen rezervaciu"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      datum sa zadava v tvare YYYY-MM-DD</br>   
    </form>
    
    ';
  }
    


?>
