<?php
 
      $SERVER = 'localhost:/var/run/mysql/mysql.sock';
	    $DATABASE = 'xseben00';
	    $USER = 'xseben00';
	    $PASSWORD ='5ojdahar';
    
      $DB = mysql_connect($SERVER, $USER, $PASSWORD);
		  if (!$DB) die('nelze se pripojit '.mysql_error());
      if (!mysql_select_db($DATABASE, $DB)) die('database neni dostupna '.mysql_error());


?>
