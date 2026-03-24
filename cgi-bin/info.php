#!/usr/bin/env php-cgi
<?php
// En-tête HTTP obligatoire
header("Content-Type: text/html");

// Récupère les variables d'environnement CGI
$method = $_SERVER['REQUEST_METHOD'];
$name = $_GET['name'] ?? "Invité";

// Génère la page HTML dynamiquement
echo <<<HTML
<!DOCTYPE html>
<html>
<head>
    <title>CGI PHP</title>
</head>
<body>
    <h1>Bonjour, $name !</h1>
    <p>Ce contenu est généré par un script PHP CGI.</p>
    <p>Méthode HTTP : $method</p>
    <p>Version PHP : <?php echo phpversion(); ?></p>
</body>
</html>
HTML;
?>
