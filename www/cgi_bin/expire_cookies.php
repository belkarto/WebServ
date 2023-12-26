<?php
// Check if cookies are set and if so, expire them
if (isset($_COOKIE['name']) && isset($_COOKIE['email']) && isset($_COOKIE['age']) && isset($_COOKIE['time'])) {
    setcookie('name', '', time() - 1, '/');
    setcookie('email', '', time() - 1, '/');
    setcookie('age', '', time() - 1, '/');
    setcookie('time', '', time() - 1, '/');
}

// Redirect back to index.php
header('Location: index.php');
exit();
?>
