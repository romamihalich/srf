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

bool ismono(int mult[N*N], int add[N*N]) {
    for (int i = 0; i < N*N; i++) {
        if (mult[i] != add[i]) {
            return false;
        }
    }
    return true;
}

bool isconst(int table[N*N]) {
    for (int i = 0; i < N*N - 1; i++) {
        if (table[i] != table[i + 1]) {
            return false;
        }
    }
    return true;
}

bool isneutral(int table[N*N], int inx) {
    for (int i = 0; i < N; i++) {
        if (table[inx*N + i] != i || table[i*N + inx] != i) {
            return false;
        }
    }
    return true;
}

// -1 if not exist
int neutral(int table[N*N]) {
    for (int i = 0; i < N; i++) {
        if (isneutral(table, i)) {
            return i;
        }
    }
    return -1;
}

// -1 if not exist
int zero(int mult[N*N], int add[N*N]) {
    int add_neu = neutral(add);
    if (add_neu == -1) {
        return -1;
    }
    for (int i = 0; i < N; i++) {
        if (mult[add_neu*N + i] != add_neu || mult[i*N + add_neu] != add_neu) {
            return -1;
        }
    }
    return add_neu;
}

bool isinfinity(int mult[N*N], int add[N*N], int inx) {
    for (int i = 0; i < N; i++) {
        if (
            mult[i*N + inx] != inx || mult[inx*N + i] != inx
            ||
            add[i*N + inx] != inx || add[inx*N + i] != inx
        ) {
            return false;
        }
    }
    return true;
}

// -1 if not exist
int infinity(int mult[N*N], int add[N*N]) {
    for (int i = 0; i < N; i++) {
        if (isinfinity(mult, add, i)) {
            return i;
        }
    }
    return -1;
}
