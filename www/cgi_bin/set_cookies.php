<?php
// Retrieve form data
// $f = fopen( 'php://stdin', 'r' );
// while( $line = fgets( $f ) ) {
//   echo $line;
// }
//
// fclose( $f );


// Print all PHP environment variables
// echo "<br>";
// foreach ($_SERVER as $key => $value) {
//     echo $key . " => " . $value . "<br>";
// }

$name = $_POST['name'];
$email = $_POST['email'];
$age = $_POST['age'];
$timme = $_POST['time'] + time();

// Set cookies
setcookie('name', $name, $timme, '/');
setcookie('email', $email, $timme, '/');
setcookie('age', $age, $timme, '/');
setcookie('time', $timme, $timme, '/');

// Redirect back to index.php
header('Location: index.php');
exit();
?>
