# Simple script to run 20 test cases from "chess" problem.
# Author: Diego Cintra
# Date: April 28th 2017
#!/bin/sh
for i in {1..20}
do
	echo "Running chess for case $i"
	eval ./chess < in/$i.in > output/$i.out
	printf "\n"	
done
# eval ./chess < in/1.in
# printf "\n"
# eval ./chess < in/2.in
# printf "\n"
# eval ./chess < in/3.in
# printf "\n"
# eval ./chess < in/4.in
# printf "\n"
# eval ./chess < in/5.in
# printf "\n"
# eval ./chess < in/6.in
# printf "\n"
# eval ./chess < in/7.in
# printf "\n"
# eval ./chess < in/8.in
# printf "\n"
# eval ./chess < in/9.in
# printf "\n"
# eval ./chess < in/10.in
# printf "\n"
