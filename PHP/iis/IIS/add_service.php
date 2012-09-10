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
if ( isset($_POST["cisloP"]) && isset($_POST["cisloS"]))
{

    $ok = true;
    
      $checkresults = mysql_query("SELECT * FROM Pobyt WHERE cisloP='".$_POST["cisloP"]."'");
      if($checkresults)
      {
          if(mysql_num_rows($checkresults)==0)
          {
            $ok == false;
          }
        }  else
        {
            $ok == false;
        } 
           

    
      $checkresults = mysql_query("SELECT * FROM Sluzby WHERE cisloIZ='".$_POST["cisloS"]."'");
      if($checkresults)
      {
          if(mysql_num_rows($checkresults)==0)
          {
            $ok == false;
          }
        }  else
        {
            $ok == false;
        }
    $cip = mysql_escape_string($_POST["cisloP"]);
    $cis = mysql_escape_string($_POST["cisloS"]); 
       
    if($ok == true){    //cisloZS 	cisloP 	cisloS
      mysql_query("INSERT INTO ZahrnuteSL 
                  (cisloZS, cisloP, cisloS) 
                  VALUES(0, '$cip', '$cis') ") 
                  or die('insert failed'.mysql_error()) or die(mysql_error());
                             
      header( 'Location: index.php?p=ubytovanie_ins&show=&cip='.$cip) ;
    }    
    else
    { 
      header  ('Location: index.php?p=add_service&cisloP='.$cip);  
    }         

}
else
{
   echo '    
    <form action="add_service.php" method="post">
   ';
   echo '<table class="forform">';


    echo '
      <tr><td>Cislo pobytu*:</td>
      <td>
      <select name="cisloP">
      ';
                                        
    $checkresults = mysql_query("SELECT Pobyt.cisloP 
                                  FROM Pobyt ");
    if($checkresults)
    {
     while($row = mysql_fetch_array($checkresults))
     {  
      if ( isset($_GET["cisloP"]) )
      {
        if ( $_GET["cisloP"] == $row["cisloP"]) 
          echo '<option selected="selected" value="'.$row["cisloP"].'">"'.$row["cisloP"].'"</option>'  ;
      }
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

    echo '
      <tr><td>Cislo sluzby*:</td>
      <td>
      <select name="cisloS">
      ';
                                        
    $checkresults = mysql_query("SELECT Sluzby.cisloS 
                                  FROM Sluzby ");
    if($checkresults)
    {
     while($row = mysql_fetch_array($checkresults))
     {  
      if ( isset($_GET["cisloS"]) )
      {
        if ( $_GET["cisloS"] == $row["cisloS"]) 
          echo '<option selected="selected" value="'.$row["cisloS"].'">"'.$row["cisloS"].'"</option>'  ;
      }
      else
        echo '<option value="'.$row["cisloS"].'">'.$row["cisloS"].'</option>'  ;
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
   echo '</table>';
   
   echo '<input type="submit" value="Pridaj sluzbu k pobytu"/>';
   
   echo '</form>';

}



?>