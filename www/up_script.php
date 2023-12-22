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
