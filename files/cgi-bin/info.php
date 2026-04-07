#!/usr/bin/php
<?php

$body = <<<HTML
<!DOCTYPE html>
<html>
<head>
    <title>CGI PHP</title>
</head>
<body>
    <p>Ce contenu est généré par un script PHP CGI.</p>
</body>
</html>
HTML;

$content_length = strlen($body);

echo "Status: 200 OK\r\n"; 
echo "Content-Length: $content_length\r\n";
echo "Content-Type: text/html\r\n";

echo "\r\n";

echo $body;
?>