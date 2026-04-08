#!/usr/bin/env python3
import os
import sys

# Standard CGI header
print("Content-Type: text/plain\r\n\r\n", end="")

print("--- Python CGI POST Test ---")
print(f"Request Method: {os.environ.get('REQUEST_METHOD', 'UNKNOWN')}")

# Handle POST data
if os.environ.get('REQUEST_METHOD') == 'POST':
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        print(f"Content-Length: {content_length} bytes")
        
        if content_length > 0:
            post_data = sys.stdin.read(content_length)
            print(f"\nPOST Data Received:\n{post_data}")
        else:
            print("\nPOST Data: (empty)")
    except ValueError:
        print("Error: Invalid CONTENT_LENGTH")
else:
    print("Error: This script requires POST method")
