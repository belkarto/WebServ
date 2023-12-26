<!DOCTYPE html>
<html>
<head>
    <title>Set and Expire Cookies</title>
    <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
    <div class="container">
        <h1>Cookies</h1>
        <form method="POST" action="set_cookies.php">
            <div class="form-group">
                <label for="name">Name:</label>
                <input type="text" name="name" id="name" vlaue="webserv" required>
            </div>

            <div class="form-group">
                <label for="email">Email:</label>
                <input type="email" name="email" id="email" vlaue="webserv@1337.ma" required>
            </div>

            <div class="form-group">
                <label for="age">Age:</label>
                <input type="number" name="age" id="age" required>
            </div>

            <div class="form-group">
                <label for="time">Time to expire in seconds:</label>
                <input type="number" name="time" id="time" value="30" required>
            </div>

            <input type="submit" value="Set Cookies" class="btn">
        </form>

        <div class="cookies" id="cookies-container">
            <?php
// Check if cookies are set
if (isset($_COOKIE['name']) && isset($_COOKIE['email']) && isset($_COOKIE['age']) && isset($_COOKIE['time'])) {
    $name = $_COOKIE['name'];
    $email = $_COOKIE['email'];
    $age = $_COOKIE['age'];
    $expiryTime = $_COOKIE['time'] - time();

    echo "<h2>Cookies:</h2>";
    echo "<p><strong>Name:</strong> $name</p>";
    echo "<p><strong>Email:</strong> $email</p>";
    echo "<p><strong>Age:</strong> $age</p>";
    echo "<p><strong>Expire in:</strong> $expiryTime seconds</p>";
} else {
    echo "<p>No cookies set</p>";
}
?>
        </div>

        <form method="POST" action="expire_cookies.php">
            <input type="submit" value="Expire Cookies" class="btn">
        </form>
    </div>
</body>
</html>
