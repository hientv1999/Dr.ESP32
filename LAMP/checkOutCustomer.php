<!DOCTYPE HTML>  
<html>
<head>
<style>
.error {color: #FF0000;}
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
date_default_timezone_set('America/Vancouver');
// define variables and set to empty values
$emailErr = $genderErr = $deviceOrderErr = "";
$email= $gender = $deviceOrder= $checkOutTime = "";
$returnMessage = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  if (empty($_POST["email"])) {
    $emailErr = "Email is required";
  } else {
    $email = test_input($_POST["email"]);
    // check if e-mail address is well-formed
    if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
      $emailErr = "Invalid email format";
    }
  }

  if (empty($_POST["gender"])) {
    $genderErr = "gender is required";
  } else {
    $gender = test_input($_POST["gender"]);
  }

  if (empty($_POST["deviceOrder"])) {
    $deviceOrderErr = "deviceOrder is required";
  } else {
  	$deviceOrder = test_input($_POST["deviceOrder"]);
  }
  
	if ( $emailErr == "" && $deviceOrderErr == "" && $genderErr == ""){
    	$servername = "localhost";
		$dbname = "";
		$username = "";
		$password = "";
  		$conn = new mysqli($servername, $username, $password, $dbname);
  		if ($conn->connect_error) {
  			die("Connection failed: " . $conn->connect_error);
  		}
        
        $sql = "INSERT INTO CustomerInfo (email, gender, deviceOrder)
        VALUES ('" . $email . "', '" . $gender . "',  '" . $deviceOrder . "' )";
  		if ($conn->query($sql) === TRUE) {
			$returnMessage = "New record created successfully";
            $checkOutTime = new DateTime();
  		} else {
     		$returnMessage = "Error: " + " . $sql . " + " . $conn->error .";
  		}
  		$conn->close();
  	}
}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}
?>	

<h2>Check Out Customer</h2>
<p><span class="error">* required field</span></p>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">  
  E-mail: <input type="text" name="email">
  <span class="error">* <?php echo $emailErr;?></span>
  <br><br>
  Gender:
  <input type="radio" name="gender" value="Female">Female
  <input type="radio" name="gender" value="Male">Male
  <input type="radio" name="gender" value="Other">Other
  <span class="error">* <?php echo $genderErr;?></span>
  <br><br>
  Device Order: <input type="text" name="deviceOrder">
  <span class="error">* <?php echo $deviceOrderErr;?></span>
  <br><br>
  <input type="submit" name="submit" value="Submit">  
  <span class="error">* <?php echo $returnMessage;?></span>
</form>


<?php

echo "<h2>Last Input</h2>";
echo '<table cellspacing="15" cellpadding="15">
      <tr> 
        <td>Email</td> 
        <td>Gender</td> 
        <td>Device Number</td> 
        <td>checkOutTime</td>
      </tr>';
echo '<tr> 
        <td>' . $email . '</td> 
        <td>' . $gender . '</td> 
        <td>' . $deviceOrder . '</td>
        <td>' . $checkOutTime->format('Y-m-d H:i:s') . '</td>
     </tr>';
?>

</body>
</html>
