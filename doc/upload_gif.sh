#!/bin/bash

# # Your existing curl command
# response=$(curl -X POST -H "Content-Type: image/gif" -H "Host: tata" --data-binary "@/home/belkarto/Downloads/XOsX.gif" localhost:2001/upload -v 2>&1)
#
# # Extract Content-Location using awk
# content_location=$(echo "$response" | awk '/^< Content-Location:/ {print $3}')
#
# # Print the Content-Location value
# echo "$content_location"
#
# # Use the Content-Location value in the next curl command
#
# url="localhost:2001$content_location"
#
# curl -X GET -H "Host: tata1" $url | echo $?



# Your existing curl command
response=$(curl -X POST -H "Content-Type: image/gif" -H "Host: tata" --data-binary "@$1" localhost:2001/upload -v 2>&1)

# Extract Content-Location using awk
content_location=$(echo "$response" | awk '/^< Content-Location:/ {print $3}')

# Print the Content-Location value
echo "Content-Location: $content_location"
