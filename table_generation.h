#ifndef TABLE_GENERATION_C
#define TABLE_GENERATION_C

#include "list.h"
#include "table_check.h"

extern int N;

int* copy_matrix(int matrix[N*N]);
void generate_commutative_tables(List* mult_tables);
void generate_idempotent_tables(List* mult_tables);
void generate_commutatuive_and_idempotent_tables(List* mult_tables);

#endif // !TABLE_GENERATION_C
