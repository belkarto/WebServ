#!/bin/bash

url="http://localhost:2001/index.html"  # Replace with your target URL

output_file="./test.log"  # Replace with your desired output file

for ((i=1; i<=4000; i++)); do
    curl -s "$url" >> "$output_file" &
done

wait
