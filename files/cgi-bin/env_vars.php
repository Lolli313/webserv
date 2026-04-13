<?php
echo "Content-Type: text/plain\r\n\r\n";
echo "--- CGI ENVIRONMENT VARIABLES ---\n";
foreach ($_SERVER as $key => $value) {
    echo "[$key] => $value\n";
}
?>