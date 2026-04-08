#!/usr/bin/env python3
import os
import urllib.parse

# Standard CGI header
print("Content-Type: text/plain\r\n\r\n", end="")

print("--- Query String Test ---")
print(f"Request Method: {os.environ.get('REQUEST_METHOD', 'UNKNOWN')}")

query_string = os.environ.get('QUERY_STRING', '')
print(f"Query String: {query_string}")

if query_string:
    # Parse the query string
    params = urllib.parse.parse_qs(query_string)
    print("\nParsed Parameters:")
    for key, values in params.items():
        for value in values:
            print(f"  {key} = {value}")
else:
    print("\nNo query parameters provided")
