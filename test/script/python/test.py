#!/usr/bin/env python3
import os
import sys

# Standard CGI header
print("Content-Type: text/plain\r\n\r\n", end="")

print("--- Python CGI Test Script ---")
print(f"Request Method: {os.environ.get('REQUEST_METHOD', 'UNKNOWN')}")
print(f"Query String: {os.environ.get('QUERY_STRING', '')}")

# Handle POST data
if os.environ.get('REQUEST_METHOD') == 'POST':
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if content_length > 0:
            post_data = sys.stdin.read(content_length)
            print(f"POST Data: {post_data}")
        else:
            print("POST Data: (empty)")
    except ValueError:
        print("Error: Invalid CONTENT_LENGTH")

print("\n--- Environment Variables ---")
for key, value in sorted(os.environ.items()):
    print(f"{key}={value}")
