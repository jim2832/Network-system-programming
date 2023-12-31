#!/bin/bash

test_count=10

# original version
make all
for i in $(seq 1 $test_count); do
    echo Processing origin version $i/$test_count

    # run command and save output to variable
    output=$({ time ./origin 100000; } 2>&1)

    # use awk to extract the seconds of real and append to file
    echo -n "$output" | grep real | awk '{print $2}' | sed 's/^0m//' >> origin_output.txt
done

# new version
make new
for i in $(seq 1 $test_count); do
    echo Processing new version $i/$test_count
    
    # run command and save output to variable
    output=$({ time ./new_version 100000; } 2>&1)

    # use awk to extract the seconds of real and append to file
    echo -n "$output" | grep real | awk '{print $2}' | sed 's/^0m//' >> new_output.txt
done

# merge files
(echo -e "origin   new"; paste origin_output.txt new_output.txt) > output.txt
cat output.txt