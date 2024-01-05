#!/bin/bash

# Number of times to execute the curl command
num_iterations=500

# Loop to execute the curl command
for ((i=1; i<=$num_iterations; i++)); do
    curl -X POST -H "Content-Type: text/plain" -H "Host: redir" --data "test post request on server and stuff any way thest test test test for ever and ever 1337 42 lol XD stfu ;lkjasdf;lkjasdf;lkjasdf" localhost:2003/upload
    curl -X POST -H "Content-Type: text/plain" -H "Host: tata1" --data "test post request on server and stuff any way thest test test test for ever and ever 1337 42 lol XD stfu ;lkjasdf;lkjasdf;lkjasdf" localhost:2003/upload
    curl -X GET -H "Host: tata" localhost:2003
    curl -X GET -H "Host: tata1" localhost:2003
done

