<!DOCTYPE html>
<html>
<head>
<style>
    table, th, td {
        border: 1px solid black;
        border-collapse: collapse;
    }
    th, td {
        padding: 10px;
    }
</style>
</head>
<body>
<?php

$servername = "localhost";

// REPLACE with your Database name
$dbname = "";
// REPLACE with Database user
$username = "";
// REPLACE with Database user password
$password = "";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, FromDevice, ToDevice, contact_time FROM COVID_19 ORDER BY id DESC";

echo '<table cellspacing="15" cellpadding="15">
      <tr> 
        <td>ID</td> 
        <td>From Device</td> 
        <td>To Device</td> 
        <td>Contact Time</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_sensor = $row["FromDevice"];
        $row_location = $row["ToDevice"];
        $row_value1 = $row["contact_time"];
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_FromDevice . '</td> 
                <td>' . $row_ToDevice . '</td> 
                <td>' . $row_contact_time . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();

$url1=$_SERVER['REQUEST_URI'];
header("Refresh: 1; URL=$url1");
?> 
</table>
</body>
</html>
