#!/bin/bash 
tee $2.in | ./2310alice $1 $2 $3 $4 | tee $2.out
