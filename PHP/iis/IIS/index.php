<?php
  error_reporting(0);           
  session_start();
  require 'connect.php';
  require 'login.php';
  require 'utils.php';

//foreach ($_SESSION as $key=>$value){
//echo $key . "<=>" . $value . "<br>"; } 
  
  if (!isset($_SESSION["login"]))
  {
    showLoginPage();
  } else
  {
  
    checkInactivity();
  
    if(isset($_GET["p"]))
      $page = $_GET["p"];
    else 
      $page = "none";
  
    echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">';
    echo '<html>';
    
    htmlHead();
    showBody($page);

    echo '</html>';    

  }
  
  function htmlHead()
  {
  
    echo '
      <head>
      <meta http-equiv="content-type" content="text/html; charset=windows-1250">
      <meta name="generator" content="PSPad editor, www.pspad.com">
      <title>Hotel</title>
      <link href="styles.css" rel="stylesheet" type="text/css" />
      <link href="menu.css" rel="stylesheet" type="text/css" />
      
      <SCRIPT LANGUAGE="JavaScript">
      function resizeIframeToFitContent(iframe) {
          // This function resizes an IFrame object
          // to fit its content.
          // The IFrame tag must have a unique ID attribute.
          iframe.height = document.frames[iframe.id]
                          .document.body.scrollHeight;
      }
      </SCRIPT>
       
      <STYLE TYPE=" text/css" >
      
      input
      {
      color: #781351;
      background: #fee3ad;
      border: 1px solid #781351
      }

      textarea
      {
      color: #781351;
      background: #fee3ad;
      border: 1px solid #781351
      }
      
      .submit input
      {
      color: #000;
      background: #ffa20f;
      border: 2px outset #d7b9c9
      } 

      table.sample {
      	border-width: 4px 4px 4px 4px;
      	border-spacing: 4px;
      	border-style: ridge ridge ridge ridge;
      	border-color: black black black black;
      	border-collapse: separate;
      	background-color: white;
      }
      table.sample th {
      	border-width: 1px 1px 1px 1px;
      	padding: 1px 1px 1px 1px;
      	border-style: dashed dashed dashed dashed;
      	border-color: blue blue blue blue;
      	background-color: white;
      	-moz-border-radius: 3px 3px 3px 3px;
      }
      table.sample td {
      	border-width: 1px 1px 1px 1px;
      	padding: 1px 1px 1px 1px;
      	border-style: dashed dashed dashed dashed;
      	border-color: blue blue blue blue;
      	background-color: white;
      	-moz-border-radius: 3px 3px 3px 3px;
      } 
      
      table.forform{
      	border-width: 0px 0px 0px 0px;
      }    

      table.forform td {
      	border-width: 0px 0px 0px 0px;
      } 
          <!--
      A.menul:link {text-decoration: none; color:#FFF}
      A.menul:visited {text-decoration: none; color:#FFF}
      A.menul:hover { text-decoration: none; color:#FFF }
      
      //-->
      </style> 
      
      </head> 
      ';
  
  }
  
  function casifyPage($page)
  {
     	switch($page) {
         case "zakaznik" : include $page.'.php'; break;
         case "zakaznik_ins" : include $page.'.php'; break;
         
         case "rezervacia" : include $page.'.php'; break;
         case "rezervacia_ins" : include $page.'.php'; break;
         
         case "ubytovanie" : include $page.'.php'; break;
         case "ubytovanie_ins" : include $page.'.php'; break;

         case "services" : include $page.'.php'; break;
         case "services_ins" : include $page.'.php'; break;

         case "rooms" : include $page.'.php'; break;
         case "rooms_ins" : include $page.'.php'; break;
         
         case "payment" : include $page.'.php'; break;
         case "payment_ins" : include $page.'.php'; break;

         case "users" : include $page.'.php'; break;
         case "users_ins" : include $page.'.php'; break;
         
         case "vyuctuj_pobyt" : include $page.'.php'; break;
         case "add_service" : include $page.'.php'; break; 
         case "list_service" : include $page.'.php'; break;
         
         case "export" : include $page.'.php'; break; 
         case "import" : include $page.'.php'; break;
                  
         default : showDefaultBody();
       } 
  }
  
  
  function showBody($page)
  {
  
    if ( isset($_GET["nomenu"]) )
    {
      echo '<body>';
       casifyPage($page);
       echo '</body>';
    }
    else
    {
    echo '<body>';
    echo '<div id="maincontainer">';
    showHeader();
         
    echo'  
      <div id="contentwrapper">
      <div id="contentcolumn">
      <div class="innertube">
      ';

    casifyPage($page);
       
    echo '  
      </div>
      </div>
      </div>
    ';
    showMenu();
    showFooter();
      
    echo '</div>';
    echo '</body>';
    }
  }
  
  function showMenu()
  {
    echo'
      <div id="leftcolumn">
      <div class="innertube">

      <div id="navigation">
          <ul>
              <li>Zakaznik
                  <ul>
                      <li><a class="menul" href="index.php?p=zakaznik">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=zakaznik_ins">Pridat</a></li>
                  </ul>
              </li>
              <li>Rezervacia
                  <ul>
                      <li><a class="menul" href="index.php?p=rezervacia">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=rezervacia_ins">Pridat</a></li>
                  </ul>
              </li>
              <li>Ubytovanie
                  <ul>
                      <li><a class="menul" href="index.php?p=ubytovanie">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=ubytovanie_ins">Pridat</a></li>
                      <li><a class="menul" href="index.php?p=vyuctuj_pobyt">Vyuctuj pobyt</a></li>
                      <li><a class="menul" href="index.php?p=list_service">Zobraz sluzby pobytu</a></li>
                      <li><a class="menul" href="index.php?p=add_service">Pridaj sluzbu k pobytu</a></li>
                  </ul>
              </li>
              
              <li>Platba
                  <ul>
                      <li><a class="menul" href="index.php?p=payment">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=payment_ins">Pridat</a></li>
                  </ul>
              </li>
              
              <li>Export a Import
                  <ul>
                      <li><a class="menul" href="index.php?p=export">Export</a></li>
                      <li><a class="menul" href="index.php?p=import">Import</a></li>
                  </ul>
              </li>
     ';         
              
     if ($_SESSION["prava"]==1)
     {
      echo '
      
              <li>Sluzby
                  <ul>
                      <li><a class="menul" href="index.php?p=services">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=services_ins">Pridat</a></li>
                  </ul>
              </li>
                
              <li>Izby
                  <ul>
                      <li><a class="menul" href="index.php?p=rooms">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=rooms_ins">Pridat</a></li>
                  </ul>
              </li> 
              
              <li>Uzivatelia
                  <ul>
                      <li><a class="menul" href="index.php?p=users">Zobrazit</a></li>
                      <li><a class="menul" href="index.php?p=users_ins">Pridat</a></li>
                  </ul>
              </li>
              
              
      ';
     }         
     
     
     echo '
              <li><a class="menul" href="logout.php">Logout</a>
              </li>          
          </ul>
      </div>


      </div>
      </div>
    ';  
  }
  
  function showHeader()
  {
    echo '
      <div id="topsection">
      <div class="innertube">
      <h1>Vytajte v IS Hotela</h1>
      <br/>
      Prihlaseny uzivatel: '.$_SESSION["login"].'
      </div>
      </div>
      ';  
  }
  
  function showFooter()
  {
    echo '
      <div id="footer">
      Projekt iis 2009
      </div>
    ';  
  }
  
  function showDefaultBody()
  {
     echo '<h3>Zacnite pracovat v menu nalavo</h3>';
  }
  
?>