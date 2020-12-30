#!/bin/bash
output=1
gcc -Wall -std=c99 -pedantic -g -o criptografia criptografia.c -lm
for run in {1..5}
do
	./criptografia < 1.in > ~/Downloads/cripto/$output.out
	echo "finished running program!"
	let "output++"
done