#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "list.h"
#include "table_check.h"
#include "table_generation.h"
#include "caching.h"

int N;

bool plus_one(int* arr, int len) {
    for (int i = len - 1; i >= 0; i--) {
        if (arr[i] != N-1) {
            arr[i]++;
            return true;
        }
        arr[i] = 0;
    }
    return false;
}

void generate_idempotent_tables_half(int matrix[N*N], int arr[N*N - N], unsigned long long stop, List* tables, char* text) {
    unsigned long long count = 0;
    int inx = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i != j) {
                matrix[i*N + j] = arr[inx];
                inx++;
            }
        }
    }
    while (true) {
        if (isassociative(matrix)) {
            int* matrix_copy = copy_matrix(matrix);
            list_push(tables, matrix_copy);
        }

        count++;

        if (count % 1000000000 == 0) {
            fprintf(stderr, text, count);
        }

        if (count == stop) {
            break;
        }

        for (int i = N - 1; i >= 0; i--) {
            for (int j = N - 1; j >= 0; j--) {
                if (i != j) {
                    if (matrix[i*N + j] != N - 1) {
                        matrix[i*N + j]++;
                        goto loop_end;
                    }
                    matrix[i*N + j] = 0;
                }
            }
        }
        loop_end:;
    }
}

int* get_part2_arr(unsigned long long part1_count) {
    int* result = (int*)malloc((N*N - N)*sizeof(int));
    for (int i = (N*N - N) - 1; i >= 0; i--) {
        result[i] = part1_count % N;
        part1_count /= N;
    }
    return result;
}

void* generate_idempotent_tables_part1(void* arg) {
    List* mult_tables_part1 = (List*)malloc(sizeof(List));
    mult_tables_part1->count = 0;
    mult_tables_part1->head = NULL;
    mult_tables_part1->tail = NULL;

    unsigned long long int all = (unsigned long long int)pow(N, N*N-N);
    unsigned long long int part1_count = all / 2; 
    unsigned long long int part2_count = part1_count + all % 2;

    int matrix[N*N];
    for (int i = 0; i < N; i++) {
        matrix[i*N + i] = i;
    }
    // int arr[N*N - N];
    int* arr = (int*)malloc((N*N - N)*sizeof(int));
    for (int i = 0; i < N*N - N; i++) {
        arr[i] = 0;
    }
    generate_idempotent_tables_half(matrix, arr, part1_count, mult_tables_part1, "part1_count: %llu\n");
    return mult_tables_part1;
}

int main(void) {
    N = 5;
    unsigned long long all = (int)pow(N, N*N-N);
    unsigned long long part1_count = all / 2;
    unsigned long long part2_count = part1_count + all % 2;

    pthread_t generate_idempotent_tables_part1_th;
    pthread_create(&generate_idempotent_tables_part1_th, NULL, generate_idempotent_tables_part1, NULL);

    int matrix[N*N];
    for (int i = 0; i < N; i++) {
        matrix[i*N + i] = i;
    }
    int* arr = get_part2_arr(part1_count);
    List mult_tables_part2 = list_new();
    generate_idempotent_tables_half(matrix, arr, part2_count, &mult_tables_part2, "part2_count: %llu\n");

    List* mult_tables_part1;
    pthread_join(generate_idempotent_tables_part1_th, (void*)&mult_tables_part1);

    mult_tables_part1->tail->next = mult_tables_part2.head;

    List mult_tables = list_new();
    mult_tables.count = mult_tables_part1->count + mult_tables_part2.count;
    mult_tables.head = mult_tables_part1->head;
    mult_tables.tail = mult_tables_part2.tail;

    fprintf(stderr, "result_count: %d\n", mult_tables.count);

    cache_table_list("./mult", &mult_tables);

    return 0;
}
