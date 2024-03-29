<?php

$servername = "localhost";

// REPLACE with your Database name
$dbname = "";
// REPLACE with Database (phpMyAdmin) user
$username = "";
// REPLACE with Database (phpMyAdmin) user password
$password = "";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";

$api_key= $FromDevice = $ToDevice = $contact_time = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $FromDevice = test_input($_POST["FromDevice"]);
        $ToDevice = test_input($_POST["ToDevice"]);
        $contact_time = test_input($_POST["contact_time"]);
        
        $conn = new mysqli($servername, $username, $password, $dbname);
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        $newcontact = 0; //0 mean new, 1 mean old
        //assume checkin day is 1 days before checkout if he/she was the first person using the device
        $now = strtotime("now");
        $checkInTimeFD = strtotime("-1 day", $now);
        $checkInTimeTD = strtotime("-1 day", $now);
        //find the check out time of previous user of FromDevice
        $sql = "SELECT deviceOrder, checkOutTime FROM CustomerInfo WHERE deviceOrder = '$FromDevice' ORDER BY id DESC LIMIT 1";
        $result = $conn->query($sql);		
        if ($result->num_rows > 0) {
            while($row = $result->fetch_assoc()) {
                $checkInTimeFD = strtotime($row["checkOutTime"]);
            }
        }
        $sql2 = "SELECT deviceOrder, checkOutTime FROM CustomerInfo WHERE deviceOrder = '$ToDevice' ORDER BY id DESC LIMIT 1";
        $result2 = $conn->query($sql2);
        if ($result2->num_rows > 0) {
            while($row2 = $result2->fetch_assoc()) {
                $checkInTimeTD = strtotime($row2["checkOutTime"]);
            }
        }
        //check if the same contact has been established while the contact_time is after the above check out time
        //if not add the contact, otherwise, ignore
        $sql3 = "SELECT FromDevice, ToDevice, contact_time FROM COVID_19 WHERE (FromDevice = '$FromDevice' AND ToDevice = '$ToDevice')";
        $result3= $conn->query($sql3);
        if ($result3->num_rows > 0) {
            while($row3 = $result3->fetch_assoc()) {
                if ($checkInTimeFD <= strtotime($row3["contact_time"]) && $checkInTimeTD <= strtotime($row3["contact_time"])){
                    $newcontact = 1;
                }
            }
        }
        
        if ($newcontact == 0){
            $sql4 = "INSERT INTO COVID_19 (FromDevice, ToDevice, contact_time)
            VALUES ('" . $FromDevice . "', '" . $ToDevice . "', '" . $contact_time . "')";
            if ($conn->query($sql4) === TRUE) {
                echo "New record created successfully";
            } 
            else {
                echo "Error: " . $sql4 . "<br>" . $conn->error;
            }
        }      
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
