#!/usr/bin/env python3
import os
import json
import time

# Get cookies from environment
http_cookie = os.environ.get('HTTP_COOKIE', '')

# Parse cookies
cookies = {}
if http_cookie:
    for cookie in http_cookie.split(';'):
        cookie = cookie.strip()
        if '=' in cookie:
            name, value = cookie.split('=', 1)
            cookies[name] = value

# Check if session exists
session_id = cookies.get('session_id')
created_at = cookies.get('created_at')

print("Content-Type: application/json")
print()

if session_id:
    current_time = int(time.time())
    created_time = int(created_at) if created_at else current_time
    age = current_time - created_time

    response = {
        "status": "success",
        "valid": True,
        "session_id": session_id,
        "created_at": created_time,
        "age_seconds": age,
        "expires_in": max(0, 3600 - age),
        "cookies_received": list(cookies.keys())
    }
else:
    response = {
        "status": "error",
        "valid": False,
        "message": "No session found",
        "cookies_received": list(cookies.keys())
    }

print(json.dumps(response, indent=2))
