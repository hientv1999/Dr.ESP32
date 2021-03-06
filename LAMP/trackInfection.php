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
// define variables and set to empty values
$emailErr =  "";
$email=  "";


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
}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}
?>	

<h2>Find and Email to Contacted Customers</h2>
<b1> (Reporters must email their proof of infection to validate) </b2>
<p><span class="error">* required field</span></p>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">  
  E-mail: <input type="text" name="email">
  <span class="error">* <?php echo $emailErr;?></span>
  <br><br>
  <input type="submit" name="submit" value="Submit">
</form>


<?php

$returnMessage = "";
echo "<h2>Last Input</h2>";
echo $email;
echo "<br>";
echo "<h2>Contacted Customers</h2>";
echo '<table cellspacing="15" cellpadding="15">
      <tr> 
        <td>Email</td> 
        <td>Gender</td> 
        <td>Contact Time</td> 
      </tr>';
if ( $emailErr == ""){
    	$servername = "localhost";
		$dbname = "";
		$username = "";
		$password = "";
  		$conn = new mysqli($servername, $username, $password, $dbname);
  		if ($conn->connect_error) {
  			die("Connection failed: " . $conn->connect_error);
  		}
        //finding which devices he/she used in the past 14 days
        $last2weeks = strtotime("-14 Days");	//amount of second format
        $inRecord = 0;
        $contacted = 0;
        $sql = "SELECT id, email, gender, deviceOrder, checkOutTime FROM CustomerInfo WHERE email='$email' ";
		$result = $conn->query($sql);
		if ($result->num_rows > 0) {
        	//delete old database *******************************
            $sql6 = "DELETE FROM trackInfection WHERE id > 0";
            if ($conn->query($sql6) === TRUE) {
			} else {
  				echo "Error deleting record: " . $conn->error ."<br>";
			}
  			// output data of each row
  			while($row = $result->fetch_assoc()) {
            	if (strtotime($row["checkOutTime"]) > $last2weeks){
                	$inRecord++;
                	//finding who check out before him/her to know the range of the time he/she was in store
    				$deviceOrderUsed = $row["deviceOrder"];
                    $userID = $row["id"];
                	$checkOutTime = strtotime($row["checkOutTime"]);
                	$checkInTime = strtotime("-1 day", $checkOutTime);			//assume checkin day is 1 days before checkout if he/she was the first person using the device
                	$sql2 = "SELECT id, email, deviceOrder, checkOutTime FROM CustomerInfo WHERE (deviceOrder = '$deviceOrderUsed' AND id < '$userID') ORDER BY id DESC LIMIT 1";
                	$result2 = $conn->query($sql2);
					if ($result2->num_rows > 0) {
                		while ($row2 = $result2->fetch_assoc()) {
                    		$checkInTime = strtotime($row2["checkOutTime"]);
                    	}
                	}
                	//finding who has contacted with him/her
                	$sql3 = "SELECT id, FromDevice, ToDevice, contact_time FROM COVID_19 WHERE FromDevice = '$deviceOrderUsed' ORDER BY id ASC";
                	$result3 = $conn->query($sql3);
					if ($result3->num_rows > 0) {
                    	$returnMessage = "Below table is the list of people have close contacted with this customers<br>";
                		while ($row3 = $result3->fetch_assoc()) {
                        	if (strtotime($row3["contact_time"]) <= $checkOutTime && strtotime($row3["contact_time"]) >= $checkInTime){
                                $deviceOfContactedPeople = $row3["ToDevice"];
                        		$contactTime = $row3["contact_time"];
                        		//finding the emails of the contacted people
                        		$sql4 = "SELECT id, email, gender, deviceOrder, checkOutTime FROM CustomerInfo WHERE (deviceOrder = '$deviceOfContactedPeople' AND checkOutTime >= '$contactTime' ) ORDER BY id ASC LIMIT 1";
                        		$result4 = $conn->query($sql4);
								if ($result4->num_rows > 0) {
                        			while ($row4 = $result4->fetch_assoc()) {
                                    	//check duplication
										$emailContacted = $row4["email"];
										$genderContacted = $row4["gender"];
                                        $sql5 = "SELECT email FROM trackInfection WHERE email = '$emailContacted'";
                                        $result5 = $conn->query($sql5);
                                        if ($result5->num_rows == 0){
                                        	//insert to trackInfection
                                        	$sql6 = "INSERT INTO trackInfection (email, gender, contact_time)
            								VALUES ('" . $row4["email"] . "', '" . $row4["gender"] . "', '" . $contactTime . "')";
            								if ($conn->query($sql6) === TRUE) {
                                        		$contacted++;
                                                echo '<tr> 
        											<td>' . $emailContacted . '</td> 
       												<td>' . $genderContacted . '</td>  
        											<td>' . $contactTime . '</td> 
     											</tr>';
                                            	$output = shell_exec("python newmailing.py $emailContacted $genderContacted $contactTime");
												if ($output != NULL){
                                                	dump($output);
                                                }
                                        	} else {
                								echo "Error: " . $sql6 . "<br>" . $conn->error;
            								}
                            					
                            			}
                                     }                                     
                        		}
                            }
                    	}
                	} else {
                    	if ($contacted == 0 ){
                			$returnMessage = "This customers didn't contact with anyone<br>";                 
                        }
                	}
                }
  			}
            echo "This person has used $inRecord device(s) and has contacted to $contacted people <br>";
		} else {
        	if ($inRecord == 0){
            	$returnMessage = "This customers is NOT in the record over the past 14 days<br>";
            }
  			
		}
  		$conn->close();
 }
?>
<span class="error">* <?php echo $returnMessage;?></span>
</body>
</html>
