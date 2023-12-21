<?php
// if ($_SERVER["REQUEST_METHOD"] == "POST") {
//     $name = $_POST["name"];
//     $email = $_POST["email"];

//     $uploadDir = "../uploads/";
//     if (!file_exists($uploadDir))
//         mkdir($uploadDir, 0777, true);

//     $uploadedFiles = [];
//     foreach ($_FILES["media"]["tmp_name"] as $key => $tmpName) {
//         $randomString = bin2hex(random_bytes(8));
//         $file_name = $randomString . "_" . basename($_FILES["media"]["name"][$key]);
//         $uploadFile = $uploadDir . $file_name;
//         if (move_uploaded_file($tmpName, $uploadFile))
//             $uploadedFiles[] = $uploadFile;
//     }

//     if (!empty($uploadedFiles)) {
//         echo "<style>     .media-container {         max-width: 100%;         margin-bottom: 20px;         text-align: center;     }      img {         max-width: 100%;         height: auto;     }      video {         max-width: 100%;         height: auto;     } </style>";
//         echo "<h1>Upload Successful</h1>";
//         echo "<p>Name: $name</p>";
//         echo "<p>Email: $email</p>";
//         echo "<p>Media Files:</p>";
        
//         foreach ($uploadedFiles as $file) {
//             $fileType = mime_content_type($file);
//             if (strpos($fileType, 'image/') === 0) {
//                 echo "<div class='media-container'><img src='$file' alt='Uploaded Image'></div>";
//             } else if (strpos($fileType, 'video/') === 0) {
//                 echo "<div class='media-container'><video controls><source src='$file' type='$fileType'>Your browser does not support the video tag.</video></div>";
//             }
//         }
//     } else {
//         echo "<h1>Upload Failed</h1>";
//     }

//     exit(0);
// }
?>

<?php
$uploadDir = "../uploads/";
$allowedExtensions = ['jpg', 'jpeg', 'png', 'gif', 'mp4'];

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['files'])) {
    $fileCount = count($_FILES['files']['name']);

    for ($i = 0; $i < $fileCount; $i++) {
        $file = [
            'name' => $_FILES['files']['name'][$i],
            'type' => $_FILES['files']['type'][$i],
            'tmp_name' => $_FILES['files']['tmp_name'][$i],
            'error' => $_FILES['files']['error'][$i],
            'size' => $_FILES['files']['size'][$i]
        ];

        if ($file['error'] === 0) {
            $fileName = basename($file['name']);
            $fileExtension = strtolower(pathinfo($fileName, PATHINFO_EXTENSION));

            if (in_array($fileExtension, $allowedExtensions)) {
                $uniqueFileName = uniqid() . '.' . $fileExtension;
                $uploadPath = $uploadDir . $uniqueFileName;
                if (move_uploaded_file($file['tmp_name'], $uploadPath)) {
                    echo 'File "' . $fileName . '" uploaded successfully.<br>';
                } else {
                    echo 'Error moving the uploaded file "' . $fileName . '".<br>';
                }
            } else {
                echo 'Invalid file type. Only JPG, JPEG, PNG, GIF, and MP4 files are allowed.<br>';
            }
        } else {
            echo 'File upload error for file "' . $file['name'] . '": ' . $file['error'] . '<br>';
        }
    }
}
?>

