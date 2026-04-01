#!/usr/bin/python3

import os
import time

body = f"""
<!DOCTYPE html>
<html>
<head>
    <title>CGI Python</title>
</head>
<body>
    <p>Ce contenu est genere par un script Python CGI.</p>
</body>
</html>
"""

content_length = len(body.encode('utf-8'))
current_date = time.strftime("%a, %d %b %Y %H:%M:%S GMT", time.gmtime())

print("HTTP/1.1 200 OK")
print(f"Date: {current_date}")
print(f"Content-Length: {content_length}")
print("Content-Type: text/html; charset=utf-8")
print()
print(body, end='')
