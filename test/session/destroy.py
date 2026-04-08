#!/usr/bin/env python3
import json

# Clear cookies by setting Max-Age=0
print("Content-Type: application/json")
print("Set-Cookie: session_id=; Path=/; Max-Age=0")
print("Set-Cookie: created_at=; Path=/; Max-Age=0")
print()

response = {
    "status": "success",
    "message": "Session destroyed",
    "cookies_cleared": ["session_id", "created_at"]
}

print(json.dumps(response, indent=2))
