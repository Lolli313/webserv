#!/usr/bin/env python3

import os
import time
import datetime

body = f"""
<!DOCTYPE html>
<html>
<head>
    <title>CGI Python</title>
</head>
<body>
    <p>Ce contenu est genere par un script Python CGI.</p>
    <p>The current server time is: <b>{datetime.datetime.now()}</b></p>
</body>
</html>
"""

content_length = len(body.encode('utf-8'))
current_date = time.strftime("%a, %d %b %Y %H:%M:%S GMT", time.gmtime())
time.sleep(10)

print("HTTP/1.1 200 OK")
print(f"Date: {current_date}")
print(f"Content-Length: {content_length}")
print("Content-Type: text/html; charset=utf-8")
print()
print(body, end='')

