#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/plain\r\n\r\n", end="")

cwd = os.getcwd()
print(f"Current Working Directory: {cwd}")

try:
    # Try to read the file using a relative path
    with open("cwd_test_data", "r") as f:
        content = f.read().strip()
        print(f"SUCCESS: Read local file 'cwd_test_data'. Content: '{content}'")
except FileNotFoundError:
    print(f"FAILURE: Could not find 'cwd_test_data' in {cwd}")
    print("Files in this directory:", os.listdir(cwd))
