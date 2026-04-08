#!/usr/bin/env python3
import os
import json
import time
import random
import string

def generate_session_id():
    """Generate a random session ID"""
    timestamp = str(int(time.time()))
    random_part = ''.join(random.choices(string.ascii_letters + string.digits, k=32))
    return random_part + timestamp

# Generate session ID
session_id = generate_session_id()

# Set cookie with session ID
print("Content-Type: application/json")
print(f"Set-Cookie: session_id={session_id}; Path=/; HttpOnly; Max-Age=3600")
print(f"Set-Cookie: created_at={int(time.time())}; Path=/; Max-Age=3600")
print()  # Empty line separates headers from body

# Return JSON response
response = {
    "status": "success",
    "message": "Session created",
    "session_id": session_id,
    "created_at": int(time.time()),
    "expires_in": 3600
}

print(json.dumps(response, indent=2))
