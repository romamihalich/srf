#include "table_check.h"

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
