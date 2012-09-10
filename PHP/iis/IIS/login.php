<?php
  session_start();
  require 'connect.php';
  if (!isset($_SESSION["login"]))
  {
    require 'connect.php';
    if ( isset($_POST["fname"]) && isset($_POST["pas"]))
    { 
      $meno = $_POST["fname"];
      $heslo = $_POST["pas"];
      
                 
      $checkresults = mysql_query("SELECT meno,heslo,prava FROM Opravnenia WHERE meno = '".$meno."' AND heslo ='".$heslo."' ");
      if($checkresults)
      {
        $row = mysql_fetch_row ($checkresults);
          if ( $heslo == $row[1] && $meno == $row[0] )
    			{
      			$_SESSION["login"] = $row[0];
      			$_SESSION["prava"] = $row[2];
      			$_SESSION["lastaction"] = time();
  
      			
      		}
      }
      header( 'Location: index.php' ) ;

		}	
  }

  function showLoginPage()
  {
  
  echo'
    <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
    <html>
      <head>
      <meta http-equiv="content-type" content="text/html; charset=windows-1250">
      <meta name="generator" content="PSPad editor, www.pspad.com">
      <title>Hotelik</title>
      </head>                                               
      <body>
      <div align="center">
      
      <h3>Vytajte V IS Hotela</h3><br/>
      <form action="login.php" method="post">
      Name: <input type="text" name="fname" /><br/>
      Password: <input type="text" name="pas" /><br/>
      <input type="submit" value="Overit"/>
      </form>
      
      </div>
      </body>
      </html>    
    ';
  }      

?>
