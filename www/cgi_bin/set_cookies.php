<?php
// Retrieve form data
//
foreach ($_SERVER as $key => $value) {
        echo $key . ": " . $value . "<br>";
}


// Read the content length from the environment variables
$contentLength = isset($_SERVER['CONTENT_LENGTH']) ? (int)$_SERVER['CONTENT_LENGTH'] : 0;

// Read the POST data from stdin
$postData = file_get_contents('php://stdin');

// Process the POST data
echo "Content Length: $contentLength\n";
echo "POST Data: $postData\n";
//
// $name = $_POST['name'];
// $email = $_POST['email'];
// $age = $_POST['age'];
// $timme = $_POST['time'] + time();
//
// Set cookies
// setcookie('name', $name, $timme, '/');
// setcookie('email', $email, $timme, '/');
// setcookie('age', $age, $timme, '/');
// setcookie('time', $timme, $timme, '/');
//
// header('Location: index.php');
// echo "========================== this is php_script\n";
//
// $contentLength = isset($_SERVER['CONTENT_LENGTH']) ? (int)$_SERVER['CONTENT_LENGTH'] : 0;
// echo $contentLength, "\n";
// // Read the specified amount of data from stdin
// if ($contentLength > 0) {
//     $requestData = file_get_contents('php://input');
//
//     // Print the received data
//     echo "this is the readed data \n\n-----> ";
//     echo $requestData;
// } else {
//     echo "No data received.\r\n\r\n";
// }
// Redirect back to index.php
//
//
// Read data from the input stream (request body)
// $inputData = file_get_contents('php://input');

// Print the received data
// echo $inputData;
exit(0);
?>
