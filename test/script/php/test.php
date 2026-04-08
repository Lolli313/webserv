#!/usr/bin/php
<?php
// Standard CGI header
echo "Content-Type: text/plain\r\n\r\n";

echo "--- PHP CGI Test Script ---
";
echo "Request Method: " . ($_SERVER['REQUEST_METHOD'] ?? 'UNKNOWN') . "\n";
echo "Query String: " . ($_SERVER['QUERY_STRING'] ?? '') . "\n";

if (($_SERVER['REQUEST_METHOD'] ?? '') === 'POST') {
    $input = file_get_contents("php://input");
    echo "POST Data: " . $input . "\n";
}

echo "\n--- Environment Variables ---
";
foreach ($_SERVER as $key => $value) {
    if (is_string($value)) {
        echo "$key=$value\n";
    }
}
?>
