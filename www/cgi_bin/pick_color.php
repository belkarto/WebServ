<html>

<head>
  <title> pick color </title>

  <?php

  if (isset($_COOKIE["back"]))
    echo "<style> body {background-color:" . $_COOKIE["back"] . "} </style>";

  if ($_SERVER["REQUEST_METHOD"] == "GET") {
    if (isset($_GET["bg"])) {
      setcookie("back", $_GET["bg"]);
    }
  }

  ?>
</head>

<body>




  <form method="GET" action="">
    <?php
    echo '<input type="color" name="bg" value=' . $_COOKIE["back"] . '>'
    ?>
    <input type="submit" value="pick color">
  </form>
</body>

</html>
