#!/bin/bash 
tee $2.in | ./2310bob $1 $2 $3 $4 | tee $2.out
