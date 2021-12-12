#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "isomorphism.h"

extern int N;

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
