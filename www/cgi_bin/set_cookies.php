<?php
// Retrieve form data
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
