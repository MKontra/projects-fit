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
if( (isset($_GET["show"])) && (!isset($_GET["cip"])) )   //zakaznik chce prezerat/editovat
{
  header( 'Location: index.php?p=ubytovanie' ) ;
} 
else
if ( !isset($_GET["show"]) && isset($_POST["cip"]) )  //formular bol submitnuty
{

    $oldfields["a"] = "a";

    $oldfields["cip"] = $cip = mysql_escape_string($_POST["cip"]);
    $oldfields["copz"] = $copz = mysql_escape_string($_POST["copz"]);
    $oldfields["zap"] = $zap = mysql_escape_string($_POST["zap"]);
    $oldfields["trp"] = $trp = mysql_escape_string($_POST["trp"]);    
    $oldfields["ciz"] = $ciz = mysql_escape_string($_POST["ciz"]);
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
    
    //kontrola poctu volnych miest na izbe
    
    if(!preg_match("/^[0-9]{4}[-]{1}[0-9]{1,2}[-]{1}[0-9]{1,2}/", $zap))
    {
      $oldfields["datumR"] = "ZLA HODNOTA";
      $ok = false;
    }
    
    $asflds = date_parse($zap);
    if ( !checkdate ($asflds["month"],$asflds["day"],$asflds["year"]) )
    {
      $oldfields["datumR"] = "ZLA HODNOTA";
      $ok = false; 
    }

    if(!preg_match("/^[0-9]{1,3}$/", $trp))
    {
      $oldfields["dlzkaR"] = "ZLA HODNOTA";
      $ok = false;
    }
          
    if($ok == true){
      //cisloP 	cisloOP 	zaciatokP 	trvanieP 	cisloIZ
      $checkresults = mysql_query("SELECT * FROM Pobyt WHERE cisloP='".$cip."'");
      if($checkresults)
      {
        //zaznam xistuje - updatuj
        $row = mysql_fetch_row ($checkresults);
        mysql_query("DELETE FROM Pobyt WHERE cisloP='".$cip."'") or die(mysql_error());         
                  
                  
      }
   
      //cisloREZ 	cisloOP 	cisloIZ 	pocetL 	datumR 	dlzkaR  
      mysql_query("INSERT INTO Pobyt 
                  (cisloP, cisloOP, zaciatokP, trvanieP, cisloIZ ) 
                  VALUES('$cip', '$copz', '$zap', '$trp', '$ciz') ") 
                  or die('insert failed'.mysql_error()) or die(mysql_error());
                  
    if ( isset($_SESSION['olddata']) )
    {
      unset($_SESSION['olddata']);
    }            
      header( 'Location: index.php?p=ubytovanie') ;
    }    
    else
    { 
      $_SESSION['olddata'] = $oldfields;
      header  ('Location: index.php?p=ubytovanie_ins&badform=pobyt');  
    }

} 
else
{

  //zobraz folmular obsahujuci data
  
  $copdisabled = false;
  $txtdata["cip"] = "";
  $txtdata["copz"] = "";
  $txtdata["zap"] = "";
  $txtdata["trp"] = "";
  $txtdata["ciz"] = "";
   
  if ( isset($_GET["show"]) )    //vytiahni data z databazy
  {
    $copdisabled = TRUE;
    $checkresults = mysql_query("SELECT * FROM Pobyt WHERE cisloP='".$_GET["cip"]."'");
    if($checkresults)
    {
      $row = mysql_fetch_row ($checkresults);
    }
    
    $txtdata["cip"] = $row[0];
    $txtdata["copz"] = $row[1];
    $txtdata["zap"] = $row[2];
    $txtdata["trp"] = $row[3];
    $txtdata["ciz"] = $row[4];

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
    <form action="ubytovanie_ins.php" method="post">
  ';
  echo '<table class="forform">';
  if (true)
  {
      echo '<tr><td>Cislo pobytu[automaticke - nemozno menit]*:</td><td> <input type="text" name="cipronly" disabled=true value="'.$txtdata["cip"].'"/></td></tr>';
      echo '<input type="hidden" name="cip" value="'.$txtdata["cip"].'"/>';
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



  echo '
    <tr><td>Zaciatok pobytu*:</td><td> <input type="text" name="zap" value="'.$txtdata["zap"].'"/></td></tr>
    <tr><td>Dlzka pobytu*:</td><td> <input type="text" name="trp" value="'.$txtdata["trp"].'"/></td></tr>
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
    echo '
    <tr><td></td><td> </td></tr>
    <tr><td></td><td><input type="submit" value="Pridaj/Zmen rezervaciu"/> </td></tr>
    </table>
    <br/></br>
    * Oznaceny udaj je povinny<br/>
      datum sa zadava v tvare YYYY-MM-DD<br/>   
    </form>
    ';
    
      $checkresults = mysql_query("SELECT * FROM Pobyt WHERE cisloP='".$txtdata["cip"]."'");
      if($checkresults)
      {
        //zaznam xistuje - updatuj
        echo 'Pobyt existuje:</br> ';
        echo '<a href="index.php?p=add_service&cisloP='.$txtdata["cip"].'">Kliknite sem na pridanie sluzby k pobytu</a><br/>'; 
        echo '<a href="index.php?p=list_service&cisloP='.$txtdata["cip"].'">Kliknite sem na odobranie sluzby k pobytu</a><br/>'; 
        echo '<a href="index.php?p=vyuctuj_pobyt&cisloP='.$txtdata["cip"].'">Kliknite sem pre vyuctovanie pobytu</a><br/>';                   
      }

      $checkresults = mysql_query("SELECT Sluzby.cisloS,Sluzby.cena 
                                   FROM ZahrnuteSL NATURAL JOIN Sluzby
                                   WHERE ZahrnuteSL.cisloS = Sluzby.cisloS
                                   AND ZahrnuteSL.cisloP='".$txtdata["cip"]."'");
      if($checkresults)
      {
         echo 'Asociovane sluzby:</br>';
         echo '<table class="sample">';
      
         echo '<tr>';
         echo '<td>Cislo sluzby</td>';
         echo '<td>Cena</td>';
         echo '</tr>'; 
         while($row = mysql_fetch_array($checkresults)) 
         {
           echo '<tr>';
           echo '<td><a href="index.php?p=services&show=&cisloS='.$row['cisloS'].'">'.$row['cisloS'].'</a></td><td>'.$row['cena'].'</td>';
           echo '</tr>'; 
         }  
         echo '</table>';                
      } 
      
        
}


?>
