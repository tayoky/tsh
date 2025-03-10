#!/bin/tsh
gcc -c main.c     -i ./include
gcc -c execline.c -i ./include
gcc -o tsh ./main.o ./execline.o
