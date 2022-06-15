#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "list.h"
#include "table_check.h"
#include "table_generation.h"
#include "caching.h"

#define THREADS_NUM 2
#define ELEMENT_NUM 5

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

void generate_idempotent_tables_half(int matrix[N*N], int arr[N*N - N], unsigned long long stop, List* tables, int part_num) {
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
            fprintf(stderr, "part %d: %llu\n", part_num, count);
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
    int part_num;
};

void* generate_idempotent_tables_th(void* arg) {
    List* mult_tables = (List*)malloc(sizeof(List));
    mult_tables->count = 0;
    mult_tables->head = NULL;
    mult_tables->tail = NULL;

    int* arr = ((struct Args*)arg)->arr;
    unsigned long long count = ((struct Args*)arg)->count;
    int part_num = ((struct Args*)arg)->part_num;

    int matrix[N*N];
    for (int i = 0; i < N; i++) {
        matrix[i*N + i] = i;
    }
    generate_idempotent_tables_half(matrix, arr, count, mult_tables, part_num);
    return mult_tables;
}

int main(void) {
    N = ELEMENT_NUM;

    unsigned long long all = (unsigned long long)pow(N, N*N-N);

    fprintf(stderr, "all:         %llu\n", all);

    pthread_t threads[THREADS_NUM];

    for (int i = 0; i < THREADS_NUM; i++) {
        struct Args* args = (struct Args*)malloc(sizeof(struct Args));
        args->arr = to_Nth(i*(all / THREADS_NUM));
        args->count = all / THREADS_NUM;
        if (i == THREADS_NUM - 1) {
            args->count += all % THREADS_NUM;
        }
        args->part_num = i + 1;
        pthread_create(&threads[i], NULL, generate_idempotent_tables_th, args);
    }

    List* mult_tables_arr[THREADS_NUM];

    for (int i = 0; i < THREADS_NUM; i++) {
        pthread_join(threads[i], (void*)&mult_tables_arr[i]);
    }

    int result_count = 0;
    for (int i = 0; i < THREADS_NUM; i++) {
        result_count += mult_tables_arr[i]->count;
    }

    fprintf(stderr, "result_count: %d\n", result_count);

    cache_table_list("./mult", mult_tables_arr[0], false);
    for (int i = 1; i < THREADS_NUM; i++) {
        cache_table_list("./mult", mult_tables_arr[i], true);
    }

    return 0;
}
