# Simple script to run 10 test cases from "captcha" problem.
# Author: Diego Cintra
# Date: April 28th 2017
#!/bin/sh
eval ./captcha < in/1.in
printf "\n"
eval ./captcha < in/2.in
printf "\n"
eval ./captcha < in/3.in
printf "\n"
eval ./captcha < in/4.in
printf "\n"
eval ./captcha < in/5.in
printf "\n"
eval ./captcha < in/6.in
printf "\n"
eval ./captcha < in/7.in
printf "\n"
eval ./captcha < in/8.in
printf "\n"
eval ./captcha < in/9.in
printf "\n"
eval ./captcha < in/10.in
printf "\n"
