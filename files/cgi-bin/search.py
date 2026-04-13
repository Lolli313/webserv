#!/usr/bin/python3
import os, sys

print("Content-Type: text/plain\n")
query = os.environ.get("QUERY_STRING", "")
target = query.split("=")[1] if "=" in query else ""

if not target:
    print("Usage: ?name=filename")
else:
    print(f"Searching for: {target}...")
    # List files in current dir that match
    matches = [f for f in os.listdir('.') if target in f]
    print(f"Found: {matches}" if matches else "No matches found.")