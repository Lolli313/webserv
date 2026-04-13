#!/usr/bin/python3
import os
import platform
import datetime

print("Content-Type: text/plain\n")
print(f"--- SYSTEM STATUS ---")
print(f"Time: {datetime.datetime.now().strftime('%H:%M:%S')}")
print(f"OS:   {platform.system()} {platform.release()}")
print(f"Load: {os.getloadavg()}")
print(f"Uptime: {os.popen('uptime -p').read().strip()}")