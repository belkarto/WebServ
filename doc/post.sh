#!/bin/bash

# Number of times to execute the curl command
num_iterations=10000

# Loop to execute the curl command
for ((i=1; i<=$num_iterations; i++)); do
    curl -X POST -H "Content-Type: text/plain" --data "GREAT" localhost:2001/upload
done

