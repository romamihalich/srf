#include "table_generation.h"

int* copy_matrix(int matrix[N*N]) {
    int* matrix_copy = (int*)malloc(N*N*sizeof(int));
    if (matrix_copy == NULL) {
        printf("OS didn't give memory for matrix. Exiting...");
        exit(1);
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix_copy[i*N + j] = matrix[i*N + j];
        }
    }
    return matrix_copy;
}

void generate_commutative_tables_rec(int matrix[N*N], int row_pos, int col_pos, List* add_tables) {
    if (row_pos == N && col_pos == N) {
        if (isassociative(matrix)) {
            int* matrix_copy = copy_matrix(matrix);
            list_push(add_tables, matrix_copy);
        }
        return;
    }
    for (int i = 0; i < N; i++) {
        matrix[row_pos*N + col_pos] = i;
        matrix[col_pos*N + row_pos] = i;
        int new_row_pos = row_pos;
        int new_col_pos = col_pos + 1;
        if (new_col_pos == N) {
            new_row_pos++;
            new_col_pos = new_row_pos;
        }
        generate_commutative_tables_rec(matrix, new_row_pos, new_col_pos, add_tables);
    }
}

void generate_commutative_tables(List* mult_tables) {
    int matrix[N*N];
    generate_commutative_tables_rec(matrix, 0, 0, mult_tables);
}

unsigned long long mult_tables_count = 0;

void generate_idempotent_tables_rec(int matrix[N*N], int row_pos, int col_pos, List* mult_tables) {
    if (row_pos == N && col_pos == 0) {
        mult_tables_count++;
        if (mult_tables_count % 1000000000 == 0) {
            fprintf(stderr, "mult_tables_count: %llu\n", mult_tables_count);
        }
        if (isassociative(matrix)) {
            int* matrix_copy = copy_matrix(matrix);
            list_push(mult_tables, matrix_copy);
        }
        return;
    }
    for (int i = 0; i < N; i++) {
        matrix[row_pos*N + col_pos] = i;
        int new_col_pos = (col_pos + 1) % N;
        int new_row_pos = row_pos;
        if (new_col_pos == 0)
            new_row_pos++;
        if (new_col_pos == new_row_pos) {
            new_col_pos = (new_col_pos + 1) % N;
            if (new_col_pos == 0)
                new_row_pos++;
        }
        generate_idempotent_tables_rec(matrix, new_row_pos, new_col_pos, mult_tables);
    }
}

void generate_idempotent_tables(List* mult_tables) {
    int matrix[N*N];
    for (int j = 0; j < N; j++) {
        matrix[j*N + j] = j;
    }
    generate_idempotent_tables_rec(matrix, 0, 1, mult_tables);
}

void generate_commutatuive_and_idempotent_tables_rec(int matrix[N*N], int row_pos, int col_pos, List* mult_tables) {
    if (row_pos == N - 1 && col_pos == N) {
        if (isassociative(matrix)) {
            int* matrix_copy = copy_matrix(matrix);
            list_push(mult_tables, matrix_copy);
        }
        return;
    }
    for (int i = 0; i < N; i++) {
        matrix[row_pos*N + col_pos] = i;
        matrix[col_pos*N + row_pos] = i;
        int new_row_pos = row_pos;
        int new_col_pos = col_pos + 1;
        if (new_col_pos == N) {
            new_row_pos++;
            new_col_pos = new_row_pos + 1;
        }
        generate_commutatuive_and_idempotent_tables_rec(matrix, new_row_pos, new_col_pos, mult_tables);
    }
}

void generate_commutatuive_and_idempotent_tables(List* mult_tables) {
    int matrix[N*N];
    for (int j = 0; j < N; j++) {
        matrix[j*N + j] = j;
    }
    generate_commutatuive_and_idempotent_tables_rec(matrix, 0, 1, mult_tables);
}
