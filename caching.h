#ifndef CACHING_H
#define CACHING_H

#include <stdio.h>

#include "list.h"
#include "semiring.h"

extern int N;

void cache_table_list(char* filename, List* tables, bool append);
void read_table_list_cache(List* list, char* filename);
void cache_semiring_list(char* filename, List* semirings);
void read_semiring_list_cache(List* list, char* filename);

#endif // !CACHING_H
