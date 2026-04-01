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
$current_date = gmdate("D, d M Y H:i:s") . " GMT";

echo "HTTP/1.1 200 OK\r\n";
echo "Date: $current_date\r\n";
echo "Content-Length: $content_length\r\n";
echo "Content-Type: text/html; charset=utf-8\r\n";
echo "\r\n";

echo $body;
?>