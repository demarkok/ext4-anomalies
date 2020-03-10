#!/bin/bash

TIMEOUT="3m"
DIR="."
TESTS=("na_pwrite" "na_write")

exit_code=0

for test in ${TESTS[*]}; do
	rm -f test.txt
	echo "Test $test..."
	timeout --foreground $TIMEOUT $DIR"/"$test

	if [ $? -eq 0 ] 
	then
		echo -e "\e[92mTest passed: anomaly found\e[0m"
	else
		echo -e "\e[91mTest failed: time limit $TIMEOUT exceeded\e[0m"
		exit_code=124
	fi
	echo
done

exit $exit_code

