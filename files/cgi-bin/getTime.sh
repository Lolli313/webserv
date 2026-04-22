#!/bin/sh

# Send the header
# \r = \015 (Carriage Return)
# \n = \012 (Line Feed)
printf "Content-Type: text/plain\r\n\r\n"

# Body Section
# We use %s to ensure the hyphens aren't treated as command options
printf "%s\n" "--- Server Status ---"
printf "Date: %s\n" "$(date '+%Y-%m-%d')"
printf "Time: %s\n" "$(date '+%H:%M:%S')"
printf "Timezone: %s\n" "$(date '+%Z')"
printf "%s\n" "---------------------"