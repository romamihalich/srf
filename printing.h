#ifndef PRINTING_H
#define PRINTING_H

#include <stdio.h>
#include <stdbool.h>

#include "semiring.h"
#include "list.h"

extern int N;
extern bool SYMOUT;

void fprint_stats(FILE* fptr, List semirings);
void fprint_semiring_list(FILE* fptr, List list);

#endif // !PRINTING_H
