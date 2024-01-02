<?php
// Your PHP script

// Check if 'nom' and 'age' are set in the parsed POST data
if (isset($postData['nom']) && isset($postData['age'])) {
    // Output the greeting with sanitized input
    echo "Bonjour, " . htmlspecialchars($postData['nom']) . ". Tu as " . (int)$postData['age'] . " ans.";
} else {
    // Handle the case where 'nom' or 'age' is not set
    echo "Invalid input.";
}
// exit() is not required here unless you have a specific reason to terminate the script explicitly
?>

