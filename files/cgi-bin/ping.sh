#!/bin/sh
printf "Content-Type: text/plain\r\n\r\n"

printf "=== Network Test ===\n"
printf "Target: google.com\n"

# Ping 3 times, wait 1 second max for each
if ping -c 3 -W 1 google.com > /dev/null 2>&1; then
    printf "Status: ONLINE\n"
    printf "Latency: %s\n" "$(ping -c 1 google.com | grep 'time=' | awk -F'time=' '{print $2}')"
else
    printf "Status: OFFLINE\n"
fi
printf "====================\n"