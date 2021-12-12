#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "list.h"
#include "semiring.h"
#include "table_check.h"
#include "table_generation.h"

int N;
char* OUTFILENAME;

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

void generate_semirings(List* semirings, List mult_tables, List add_tables) {
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

void fprint_semiring(FILE* fptr, Semiring semiring) {
    fprintf(fptr, "mult");
    fprintf(fptr, "%*c", 2*N - 3, ' ');
    fprintf(fptr, "add\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fptr, "%d ", semiring.mult[i*N + j]);
        }
        fprintf(fptr, " ");
        for (int j = 0; j < N; j++) {
            fprintf(fptr, "%d ", semiring.add[i*N + j]);
        }
        fprintf(fptr, "\n");
    }
}

void fprint_semiring_list(FILE* fptr, List list) {
    struct Node* temp = list.head;
    while (temp != NULL) {
        Semiring semiring = *((Semiring*)temp->value);
        fprint_semiring(fptr, semiring);
        fprintf(fptr, "\n");
        temp = temp->next;
    }
    fprintf(fptr, "count: %d\n", list.count);
}

// TODO: add flag -v (verbose) maybe

void print_usage(char* program_name) {
    printf("Usage: %s N [-o <file>]\n", program_name);
}

void read_argv(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
        case 'o':
            OUTFILENAME = optarg;
            break;
        default:
            print_usage(argv[0]);
            exit(1);
        }
    }
    if (optind >= argc) {
        printf("Provide N\n");
        print_usage(argv[0]);
        exit(1);
    }
    if (optind != argc - 1) {
        printf("Too many arguments\n");
        print_usage(argv[0]);
        exit(1);
    }
    N = atoi(argv[optind]);
    if (N <= 0) {
        printf("Expected positive integer, but was: '%s'\n", argv[optind]);
        print_usage(argv[0]);
        exit(1);
    }
}

int main(int argc, char** argv) {
    read_argv(argc, argv);

    List semirings = list_new();
    List mult_tables = list_new();
    List add_tables  = list_new();

    generate_idempotent_tables(&mult_tables);
    generate_commutative_tables(&add_tables);
    generate_semirings(&semirings, mult_tables, add_tables);

    List arrays = list_new();
    generate_arrays(&arrays);
    filter_isomorphism(&semirings, arrays);

    if (OUTFILENAME == NULL) {
        fprint_semiring_list(stdout, semirings);
    } else {
        FILE* fptr = fopen(OUTFILENAME, "w");
        fprint_semiring_list(fptr, semirings);
        fclose(fptr);
    }
    return 0;
}
