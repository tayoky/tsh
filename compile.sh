#!/bin/tsh
gcc -c main.c
gcc -c execline.c
gcc -o tsh ./main.o ./execline.o
