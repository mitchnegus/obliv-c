#!/bin/bash

# Prepare the prefix, if given
if [ $1 ]; then
	prefix="$1_"
else
	prefix=""
fi

# Run the protocol executable
./a.out localhost:1234 1 ${prefix}generator.dat &
./a.out localhost:1234 2 ${prefix}evaluator.dat
