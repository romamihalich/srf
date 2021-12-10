#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

int N;

typedef struct {
    int* mult;
    int* add;
} Semiring;

bool isassociative(int table[N*N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (table[table[i*N + j]*N + k] != table[i*N + table[j*N + k]])
                    return false;
            }
        }
    }
    return true;
}

bool iscommutative(int table[N*N]) {
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            if (table[i*N + j] != table[j*N + i])
                return false;
        }
    }
    return true;
}

bool isidempotent(int table[N*N]) {
    for (int i = 0; i < N; i++) {
        if (table[i*N + i] != i)
            return false;
    }
    return true;
}

bool isdistributive_left(int mult[N*N], int add[N*N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (mult[i*N + add[j*N + k]] != add[mult[i*N + j]*N + mult[i*N + k]])
                    return false;
            }
        }
    }
    return true;
}

bool isdistributive_right(int mult[N*N], int add[N*N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (mult[add[i*N + j]*N + k] != add[mult[i*N + k]*N + mult[j*N + k]])
                    return false;
            }
        }
    }
    return true;
}

bool isdistributive(int mult[N*N], int add[N*N]) {
    return isdistributive_left(mult, add) && isdistributive_right(mult, add);
}

bool isisomorphism(int f[N], Semiring r1, Semiring r2) {
    for (int a = 0; a < N; a++) {
        for (int b = 0; b < N; b++) {
            if (f[r1.mult[a*N + b]] != r2.mult[f[a]*N + f[b]]
                || f[r1.add[a*N + b]] != r2.add[f[a]*N + f[b]])
                return false;
        }
    }
    return true;
}

int* copy_array(int array[N]) {
    int* array_copy = (int*)malloc(N*sizeof(int));
    if (array_copy == NULL) {
        printf("OS didn't give memory for array. Exiting...");
        exit(1);
    }
    for (int i = 0; i < N; i++) {
        array_copy[i] = array[i];
    }
    return array_copy;
}

void generate_arrays_rec(int arr[N], int pos, List* arrays) {
    if (pos == N) {
        list_push(arrays, copy_array(arr));
    } else {
        for (int i = 0; i < N; i++) {
            bool found = false;
            for (int j = 0; j < pos; j++) {
                if (arr[j] == i) {
                    found = true;
                    break;
                }
            }
            if (found) 
                continue;
            arr[pos] = i;
            generate_arrays_rec(arr, pos + 1, arrays);
        }
    }
}

void generate_arrays(List* arrays) {
    int array[N];
    generate_arrays_rec(array, 0, arrays);
}

bool areisomorphic(Semiring r1, Semiring r2, List arrays) {
    bool result = false;
    struct Node* temp = arrays.head;
    while (temp != NULL) {
        if (isisomorphism(temp->value, r1, r2))
            return true;
        temp = temp->next;
    }
    return false;
}

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

void generate_add_tables_rec(int matrix[N*N], int row_pos, int col_pos, List* add_tables) {
    if (row_pos == N && col_pos == 0) {
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
        if (col_pos == N - 1)
            new_row_pos++;
        int new_col_pos = (col_pos + 1 + new_row_pos) % N;
        generate_add_tables_rec(matrix, new_row_pos, new_col_pos, add_tables);
    }
}

void generate_add_tables(List* mult_tables) {
    int matrix[N*N];
    generate_add_tables_rec(matrix, 0, 0, mult_tables);
}

void generate_mult_tables_rec(int matrix[N*N], int row_pos, int col_pos, List* mult_tables) {
    if (row_pos == N && col_pos == 0) {
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
        generate_mult_tables_rec(matrix, new_row_pos, new_col_pos, mult_tables);
    }
}

void generate_mult_tables(List* mult_tables) {
    for (int i = 0; i < N; i++) {
        int matrix[N*N];
        for (int j = 0; j < N; j++) {
            matrix[j*N + j] = j;
        }
        generate_mult_tables_rec(matrix, 0, 1, mult_tables);
    }
}

void generate_semirings(List* semirings) {
    List mult_tables = { .head = NULL, .tail = NULL, .count = 0 };
    List add_tables  = { .head = NULL, .tail = NULL, .count = 0 };

    generate_mult_tables(&mult_tables);
    generate_add_tables(&add_tables);
    struct Node* mult_temp = mult_tables.head;
    while (mult_temp != NULL) {
        bool iscomm = iscommutative(mult_temp->value);
        struct Node* add_temp = add_tables.head;
        while (add_temp != NULL) {
            if (iscomm && isdistributive_left(mult_temp->value, add_temp->value)
                || !iscomm && isdistributive(mult_temp->value, add_temp->value)) {
                Semiring* semiring = (Semiring*)malloc(sizeof(Semiring));
                semiring->mult = mult_temp->value;
                semiring->add = add_temp->value;
                list_push(semirings, semiring);
            }
            add_temp = add_temp->next;
        }
        mult_temp = mult_temp->next;
    }
}

void filter_isomorphism(List* semirings, List arrays) {
    struct Node* temp = semirings->head;
    while (temp != NULL) {
        struct Node* temp_inner = temp->next;
        while (temp_inner != NULL) {
            if (areisomorphic(*((Semiring*)temp->value), *((Semiring*)temp_inner->value), arrays)) {
                struct Node* temp_next = temp_inner->next;
                list_delete(semirings, temp_inner);
                temp_inner = temp_next;
            } else temp_inner = temp_inner->next;
        }
        temp = temp->next;
    }
}

void print_semiring(Semiring semiring) {
    printf("mult");
    printf("%*c", 2*N - 3, ' ');
    printf("add\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", semiring.mult[i*N + j]);
        }
        printf(" ");
        for (int j = 0; j < N; j++) {
            printf("%d ", semiring.add[i*N + j]);
        }
        printf("\n");
    }
}

void print_semiring_list(List list) {
    struct Node* temp = list.head;
    while (temp != NULL) {
        Semiring semiring = *((Semiring*)temp->value);
        print_semiring(semiring);
        printf("\n");
        temp = temp->next;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong number of arguments. Expected 1, but was: %d\n", argc - 1);
        exit(1);
    }
    char* input = argv[argc - 1];
    N = atoi(input);
    if (N <= 0) {
        printf("Expected positive integer, but was: '%s'\n", input);
        exit(1);
    }

    List semirings = { .head = NULL, .tail = NULL, .count = 0 };
    generate_semirings(&semirings);
    List arrays = { .head = NULL, .tail = NULL, .count = 0 };
    generate_arrays(&arrays);
    filter_isomorphism(&semirings, arrays);

    print_semiring_list(semirings);
    printf("count: %d\n", semirings.count);
    return 0;
}
