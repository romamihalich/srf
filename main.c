#include <unistd.h>

#include "list.h"
#include "semiring.h"
#include "table_check.h"
#include "table_generation.h"
#include "isomorphism.h"

int N;
char* OUTFILENAME;

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

void compute_properties(List* semirings) {
    struct Node* temp = semirings->head;
    while (temp != NULL) {
        Semiring* semiring = temp->value;
        semiring->iscommutative = iscommutative(semiring->mult);
        semiring->isidempotent = isidempotent(semiring->add);
        semiring->ismono = ismono(semiring->mult, semiring->add);
        semiring->isconst_add = isconst(semiring->add);
        semiring->zero = zero(semiring->mult, semiring->add);
        semiring->one = neutral(semiring->mult);
        semiring->infinity = infinity(semiring->mult, semiring->add);
        temp = temp->next;
    }
}

void fprint_semiring(FILE* fptr, Semiring semiring) {
    fprintf(fptr, "mult");
    fprintf(fptr, "%*c", 2*N - 3, ' ');
    fprintf(fptr, "add");
    fprintf(fptr, "%*c", 2*N - 2, ' ');
    fprintf(fptr, " %c        ", semiring.iscommutative ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.isidempotent ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.ismono ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.isconst_add ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.zero != -1 ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.one != -1 ? '+' : '-');
    fprintf(fptr, " %c        ", semiring.infinity != -1 ? '+' : '-');
    fprintf(fptr, "\n");
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

void fprint_stats(FILE* fptr, List semirings) {
    int iscommutative_count = 0;
    int isidempotent_count = 0;
    int ismono_count = 0;
    int isconst_add_count = 0;
    int zero_count = 0;
    int one_count = 0;
    int infinity_count = 0;
    struct Node* temp = semirings.head;
    while (temp != NULL) {
        Semiring* semiring = temp->value;
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
    fprintf(fptr, "iscommutative_count: %d\n", iscommutative_count);
    fprintf(fptr, "isidempotent_count: %d\n", isidempotent_count);
    fprintf(fptr, "ismono_count: %d\n", ismono_count);
    fprintf(fptr, "isconst_add_count: %d\n", isconst_add_count);
    fprintf(fptr, "zero_count: %d\n", zero_count);
    fprintf(fptr, "one_count: %d\n", one_count);
    fprintf(fptr, "infinity_count: %d\n", infinity_count);
}

void fprint_semiring_list(FILE* fptr, List list) {
    fprintf(fptr, "%*c", 4*N + 2, ' ');
    fprintf(fptr, "comm      idem      mono      const     zero      one       inf\n");
    struct Node* temp = list.head;
    while (temp != NULL) {
        Semiring semiring = *((Semiring*)temp->value);
        fprint_semiring(fptr, semiring);
        fprintf(fptr, "\n");
        temp = temp->next;
    }
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
    compute_properties(&semirings);

    if (OUTFILENAME == NULL) {
        fprint_semiring_list(stdout, semirings);
        fprint_stats(stdout, semirings);
    } else {
        FILE* fptr = fopen(OUTFILENAME, "w");
        fprint_semiring_list(fptr, semirings);
        fprint_stats(fptr, semirings);
        fclose(fptr);
    }
    return 0;
}
