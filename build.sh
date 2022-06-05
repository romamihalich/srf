#!/bin/sh

files="main.c list.c table_check.c table_generation.c isomorphism.c printing.c caching.c"

gcc $files -o main -O3 -lm
gcc list.c table_check.c table_generation.c caching.c gen-mult-tables.c -o gen-mult-tables -O3 -lm
