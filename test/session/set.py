#!/usr/bin/env python3
import os
import json
import urllib.parse

# Get query string
query_string = os.environ.get('QUERY_STRING', '')
params = urllib.parse.parse_qs(query_string)

# Get name and value from query parameters
name = params.get('name', ['test_cookie'])[0]
value = params.get('value', ['test_value'])[0]

# Set the cookie
print("Content-Type: application/json")
print(f"Set-Cookie: {name}={value}; Path=/; Max-Age=3600")
print()

response = {
    "status": "success",
    "message": "Cookie set",
    "cookie_name": name,
    "cookie_value": value,
    "max_age": 3600
}

print(json.dumps(response, indent=2))
