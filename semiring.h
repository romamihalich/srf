#ifndef SEMIRING_H
#define SEMIRING_H

typedef struct {
    int* mult;
    int* add;
    bool iscommutative;
    bool isidempotent;
    bool ismono;
    bool isconst_add;
    int zero;
    int one;
    int infinity;
} Semiring;

#endif // !SEMIRING_H
