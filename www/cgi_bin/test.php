#!/usr/bin/php-cgi
<?php
// Your PHP script

// Set custom HTTP status code
http_response_code(200); // Change to your desired status code

// Set custom headers
header("Content-type: text/html");
header("X-Custom-Header: Hello from CGI");

// Generate the HTML page
echo "<html>
<head>
    <title>CGI PHP Script</title>
</head>
<body>
    <h1>Hello, CGI World!</h1>
    <p>This is a PHP script running in CGI mode.</p>
</body>
</html>";
?>
