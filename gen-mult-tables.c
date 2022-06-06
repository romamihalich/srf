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

int* to_Nth(unsigned long long number) {
    int* result = (int*)malloc((N*N - N)*sizeof(int));
    for (int i = (N*N - N) - 1; i >= 0; i--) {
        result[i] = number % N;
        number /= N;
    }
    return result;
}

struct Args {
    int* arr;
    unsigned long long count;
    char* text;
};

void* generate_idempotent_tables_th(void* arg) {
    List* mult_tables = (List*)malloc(sizeof(List));
    mult_tables->count = 0;
    mult_tables->head = NULL;
    mult_tables->tail = NULL;

    int* arr = ((struct Args*)arg)->arr;
    unsigned long long count = ((struct Args*)arg)->count;
    char* text = ((struct Args*)arg)->text;

    int matrix[N*N];
    for (int i = 0; i < N; i++) {
        matrix[i*N + i] = i;
    }
    // int* arr = to_Nth(0);
    generate_idempotent_tables_half(matrix, arr, count, mult_tables, text);
    return mult_tables;
}

int main(void) {
    N = 5;
    // int parts_count = 4;

    unsigned long long all = (unsigned long long)pow(N, N*N-N);

    fprintf(stderr, "all:         %llu\n", all);

    pthread_t generate_idempotent_tables_part1_th;
    struct Args* args = (struct Args*)malloc(sizeof(struct Args));
    args->arr = to_Nth(0);
    args->count = all / 2;
    args->text = "part1_count: %llu\n";
    pthread_create(&generate_idempotent_tables_part1_th, NULL, generate_idempotent_tables_th, args);

    // pthread_t generate_idempotent_tables_part2_th;
    struct Args* args2 = (struct Args*)malloc(sizeof(struct Args));
    args2->arr = to_Nth(all / 2);
    args2->count = all / 2 + all % 2;
    args2->text = "part2_count: %llu\n";
    // pthread_create(&generate_idempotent_tables_part2_th, NULL, generate_idempotent_tables_th, args2);

    List* mult_tables_part2 = generate_idempotent_tables_th(args2);

    List* mult_tables_part1;
    pthread_join(generate_idempotent_tables_part1_th, (void*)&mult_tables_part1);

    // List* mult_tables_part2;
    // pthread_join(generate_idempotent_tables_part2_th, (void*)&mult_tables_part2);

    mult_tables_part1->tail->next = mult_tables_part2->head;

    List mult_tables = list_new();
    mult_tables.count = mult_tables_part1->count + mult_tables_part2->count;
    mult_tables.head = mult_tables_part1->head;
    mult_tables.tail = mult_tables_part2->tail;

    fprintf(stderr, "result_count: %d\n", mult_tables.count);

    cache_table_list("./mult", &mult_tables);

    return 0;
}
