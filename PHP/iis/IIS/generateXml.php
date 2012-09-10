<?php
  require_once 'connect.php';
  if (!isset($_POST["tablename"]) )
    header("Location:index.php");
    
    $rs=mysql_query("SELECT * FROM ".$_POST["tablename"]);
   
    if ($rs){
        $totalrows=mysql_num_rows($rs);
    }else{
        header("Location:index.php");
    }
?>

<?php
  header('Content-Type: text/xml');
  header('Content-Disposition: attachment; filename="'.$_POST["tablename"].'.xml"'); 
?>

<<?php  echo $_POST["tablename"]?>>

<columnnames>

<?php
$i = 0;
while ($i < mysql_num_fields($rs)) {
    $meta = mysql_fetch_field($rs, $i);

    echo "<column>";
    echo $meta->name;
    echo "</column>";

    $i++;
}
?>

</columnnames>


<rows>

<?php
  while($or=mysql_fetch_row($rs)) 
  {
    echo "<row>";
    foreach($or as $val)
    {
       echo "<val>";
       echo $val;
       echo "</val>";
    }
    echo "</row>";
  }
?>


</rows>

</<?php  echo $_POST["tablename"]?>>