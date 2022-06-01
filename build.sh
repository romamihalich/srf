#!/bin/sh

files="main.c list.c table_check.c table_generation.c isomorphism.c printing.c caching.c"

gcc $files -o main -O3
