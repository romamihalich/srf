#include <unistd.h>
#include <pthread.h>

#include "list.h"
#include "semiring.h"
#include "table_check.h"
#include "table_generation.h"
#include "isomorphism.h"
#include "printing.h"

int N;
char* OUTFILENAME;
bool SYMOUT;
bool VERBOSE;

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
void print_usage(char* program_name) {
    printf("Usage: %s N [-s -v -o <file>]\n", program_name);
    printf("       -o <file> - send output to file\n");
    printf("       -s        - symbolic output\n");
    printf("       -v        - print what program is doing\n");
}

void read_argv(int argc, char** argv) {
    SYMOUT = false;
    VERBOSE = false;
    int opt;
    while ((opt = getopt(argc, argv, "vso:")) != -1) {
        switch (opt) {
        case 'o':
            OUTFILENAME = optarg;
            break;
        case 's':
            SYMOUT = true;
            break;
        case 'v':
            VERBOSE = true;
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

void verbose(char* message) {
    if (VERBOSE) {
        fprintf(stderr, "%s", message);
    }
}

void transpose(int* matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            int temp = matrix[i*N + j];
            matrix[i*N + j] = matrix[j*N + i];
            matrix[j*N + i] = temp; 
        }
    }
}

void find_isomorph_dual(List semirings, List arrays, int* count, int* pos) {
    *count = 0;
    *pos = -1;
    int curPos = 1;
    struct Node* temp = semirings.head;
    while (temp != NULL) {
        Semiring* semiring = (Semiring*)temp->value;
        if (semiring->iscommutative == false) {
            Semiring* dual_semiring = (Semiring*)malloc(sizeof(Semiring));
            dual_semiring->mult = copy_matrix(semiring->mult);
            dual_semiring->add = copy_matrix(semiring->add);
            transpose(dual_semiring->mult);

            if (areisomorphic(*semiring, *dual_semiring, arrays)) {
                *count += 1;
                *pos = curPos;
            }
        }
        curPos += 1;
        temp = temp->next;
    }
}

void* generate_idempotent_tables_th(void* arg) {
    List* mult_tables = (List*)arg;
    verbose("Generating mult tables...\n");
    generate_idempotent_tables(mult_tables);
    return NULL;
}

int main(int argc, char** argv) {
    read_argv(argc, argv);

    List semirings = list_new();
    List mult_tables = list_new();
    List add_tables  = list_new();

    pthread_t mult_tables_gen_th;
    pthread_create(&mult_tables_gen_th, NULL, generate_idempotent_tables_th, (void*)&mult_tables);

    verbose("Generating add tables...\n");
    generate_commutative_tables(&add_tables);

    pthread_join(mult_tables_gen_th, NULL);

    verbose("Generating semirings...\n");
    generate_semirings(&semirings, mult_tables, add_tables);

    List arrays = list_new();
    generate_arrays(&arrays);
    verbose("Filtering isomorphisms...\n");
    filter_isomorphism(&semirings, arrays);
    verbose("Computing properties...\n");
    compute_properties(&semirings);

    int count;
    int pos;
    find_isomorph_dual(semirings, arrays, &count, &pos);

    if (OUTFILENAME == NULL) {
        fprint_semiring_list(stdout, semirings);
        fprint_stats(stdout, semirings);
        if (count > 0) {
            fprintf(stdout, "isomorph_dual_count: %d, ", count);
            fprintf(stdout, "pos: %d\n", pos);
        }
    } else {
        FILE* fptr = fopen(OUTFILENAME, "w");
        verbose("Writing output to file...\n");
        fprint_semiring_list(fptr, semirings);
        fprint_stats(fptr, semirings);
        if (count > 0) {
            fprintf(fptr, "isomorph_dual_count: %d, ", count);
            fprintf(fptr, "pos: %d\n", pos);
        }
        fclose(fptr);
    }

    return 0;
}
