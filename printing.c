#include "printing.h"

void fprint_semiring(FILE* fptr, Semiring semiring) {
    fprintf(fptr, "add");
    fprintf(fptr, "%*c", 2*N - 2, ' ');
    fprintf(fptr, "mult");
    fprintf(fptr, "%*c", 2*N - 3, ' ');
    fprintf(fptr, " %c        ", semiring.iscommutative ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.isidempotent ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.ismono ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.isconst_add ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.zero != -1 ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.one != -1 ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.infinity != -1 ? '+' : '-');
    fprintf(fptr, "\n");
    char symbols[N];
    if (SYMOUT) {
        char ch = 'a';
        for (int i = 0; i < N; i++) {
            if (semiring.zero == i) {
                symbols[i] = '0';
            } else if (semiring.one == i) {
                symbols[i] = '1';
            } else if (semiring.infinity == i) {
                symbols[i] = 'i';
            } else {
                symbols[i] = ch++;
            }
        }
    } 
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (SYMOUT) {
                fprintf(fptr, "%c ", symbols[semiring.add[i*N + j]]);
            } else fprintf(fptr, "%d ", semiring.add[i*N + j]);
        }
        fprintf(fptr, " ");
        for (int j = 0; j < N; j++) {
            if (SYMOUT) {
                fprintf(fptr, "%c ", symbols[semiring.mult[i*N + j]]);
            } else fprintf(fptr, "%d ", semiring.mult[i*N + j]);
        }
        fprintf(fptr, "\n");
    }
}

void fprint_stats(FILE* fptr, List semirings) {
    int iscommutative_count = 0;
    int isidempotent_count = 0;
    int ismono_count = 0;
    int isconst_add_count = 0;
    int zero_count = 0;
    int one_count = 0;
    int infinity_count = 0;

    int comm_and_idemp = 0;
    int inf_and_one = 0;
    int one_and_zero = 0;

    struct Node* temp = semirings.head;
    while (temp != NULL) {
        Semiring* semiring = temp->value;

        if (semiring->iscommutative && semiring->isidempotent)
            comm_and_idemp++;

        if (semiring->infinity != -1 && semiring->one != -1)
            inf_and_one++;

        if (semiring->one != -1 && semiring->zero != -1)
            one_and_zero++;


        iscommutative_count += semiring->iscommutative;
        isidempotent_count += semiring->isidempotent;
        ismono_count += semiring->ismono;
        isconst_add_count += semiring->isconst_add;
        if (semiring->zero != -1)
            zero_count++;
        if (semiring->one != -1)
            one_count++;
        if (semiring->infinity != -1)
            infinity_count++;
        temp = temp->next;
    }
    fprintf(fptr, "count: %d\n", semirings.count);
    fprintf(fptr, "comm_count: %d\n", iscommutative_count);
    fprintf(fptr, "idem_count: %d\n", isidempotent_count);
    fprintf(fptr, "mono_count: %d\n", ismono_count);
    fprintf(fptr, "const_count: %d\n", isconst_add_count);
    fprintf(fptr, "zero_count: %d\n", zero_count);
    fprintf(fptr, "one_count: %d\n", one_count);
    fprintf(fptr, "inf_count: %d\n", infinity_count);


    fprintf(fptr, "comm_and_idemp: %d\n", comm_and_idemp);
    fprintf(fptr, "inf_and_one: %d\n", inf_and_one);
    fprintf(fptr, "one_and_zero: %d\n", one_and_zero);
}

void fprint_semiring_list(FILE* fptr, List list) {
    fprintf(fptr, "%*c", 4*N + 2, ' ');
    fprintf(fptr, "comm      idem      mono      const     zero      one       inf\n");
    int pos = 1;
    struct Node* temp = list.head;
    while (temp != NULL) {
        Semiring semiring = *((Semiring*)temp->value);
        fprintf(fptr, "pos: %d\n", pos);
        fprint_semiring(fptr, semiring);
        fprintf(fptr, "\n");
        pos++;
        temp = temp->next;
    }
}
