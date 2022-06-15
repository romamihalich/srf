#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "list.h"
#include "semiring.h"
#include "table_check.h"
#include "table_generation.h"
#include "isomorphism.h"
#include "printing.h"
#include "caching.h"

int N;
char* OUTFILENAME;
bool SYMOUT;
bool VERBOSE;
char* CACHEDIR;
bool MULTCOMM;

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
    printf("Usage: %s N [-s -v -k -c <cachedir> -o <file>]\n", program_name);
    printf("       -o <file>     - send output to file\n");
    printf("       -s            - symbolic output\n");
    printf("       -v            - print what program is doing\n");
    printf("       -c <cachedir> - use cache from (or generate cache into) <cachedir>\n");
    printf("       -k            - generate mult commutative tables\n");
}

void read_argv(int argc, char** argv) {
    SYMOUT = false;
    VERBOSE = false;
    CACHEDIR = NULL;
    MULTCOMM = false;
    int opt;
    while ((opt = getopt(argc, argv, "c:vkso:")) != -1) {
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
        case 'c':
            CACHEDIR = optarg;
            break;
        case 'k':
            MULTCOMM = true;
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

void* generate_mult_tables_th(void* arg) {
    List* mult_tables = (List*)arg;
    verbose("Generating mult tables...\n");
    if (MULTCOMM) {
        generate_commutatuive_and_idempotent_tables(mult_tables);
    } else {
        generate_idempotent_tables(mult_tables);
    }
    verbose("...done generating mult tables\n");
    return NULL;
}

struct Args {
    List* mult_tables;
    char* mult_filepath;
};

void* generate_mult_tables_and_cache_th(void* arg) {
    List* mult_tables = ((struct Args*)arg)->mult_tables;
    char* mult_filepath = ((struct Args*)arg)->mult_filepath;
    verbose("Generating mult tables...\n");
    if (MULTCOMM) {
        generate_commutatuive_and_idempotent_tables(mult_tables);
    } else {
        generate_idempotent_tables(mult_tables);
    }
    verbose("...done generating mult tables\n");
    cache_table_list(mult_filepath, mult_tables, false);
    return NULL;
}

int main(int argc, char** argv) {
    read_argv(argc, argv);

    List semirings = list_new();
    List mult_tables = list_new();
    List add_tables  = list_new();
    List arrays = list_new();

    if (CACHEDIR != NULL) {
        if(mkdir(CACHEDIR, 0777) && errno != EEXIST) {
            printf("ERROR: Can't create '%s'\n%m\n", CACHEDIR);
            exit(1);
        }

        char* mult_filepath = malloc(strlen(CACHEDIR) + strlen("mult") + 2);
        sprintf(mult_filepath, "%s/%s", CACHEDIR, "mult");

        pthread_t mult_tables_gen_th;
        bool mult_tables_gen_th_started = false;
        if(access(mult_filepath, F_OK) != 0) { // not exists
            struct Args* args = (struct Args*)malloc(sizeof(struct Args));
            args->mult_tables = &mult_tables;
            args->mult_filepath = mult_filepath;
            pthread_create(&mult_tables_gen_th, NULL, generate_mult_tables_and_cache_th, (void*)args);
            mult_tables_gen_th_started = true;
        } else {
            verbose("Skipped generating mult tables...\n");
            read_table_list_cache(&mult_tables, mult_filepath);
        }

        char* add_filepath = malloc(strlen(CACHEDIR) + strlen("add") + 2);
        sprintf(add_filepath, "%s/%s", CACHEDIR, "add");

        if(access(add_filepath, F_OK) != 0) { // not exists
            verbose("Generating add tables...\n");
            generate_commutative_tables(&add_tables);
            verbose("...done generating add tables\n");
            cache_table_list(add_filepath, &add_tables, false);
        } else {
            verbose("Skipped generating add tables\n");
            read_table_list_cache(&add_tables, add_filepath);
        }

        if (mult_tables_gen_th_started) {
            pthread_join(mult_tables_gen_th, NULL);
        }

        char* semir_filepath = malloc(strlen(CACHEDIR) + strlen("semir") + 2);
        sprintf(semir_filepath, "%s/%s", CACHEDIR, "semir");

        if(access(semir_filepath, F_OK) != 0) { // not exists
            verbose("Generating semirings...\n");
            generate_semirings(&semirings, mult_tables, add_tables);
            cache_semiring_list(semir_filepath, &semirings);
        } else {
            verbose("Skipped generating semirings...\n");
            read_semiring_list_cache(&semirings, semir_filepath);
        }

        char* semirnoiz_filepath = malloc(strlen(CACHEDIR) + strlen("semirnoiz") + 2);
        sprintf(semirnoiz_filepath, "%s/%s", CACHEDIR, "semirnoiz");

        if(access(semirnoiz_filepath, F_OK) != 0) { // not exists
            generate_arrays(&arrays);
            verbose("Filtering isomorphisms...\n");
            filter_isomorphism(&semirings, arrays);
            cache_semiring_list(semirnoiz_filepath, &semirings);
        } else {
            verbose("Skipped filtering isomorphisms...\n");
            list_clear(&semirings);
            read_semiring_list_cache(&semirings, semirnoiz_filepath);
        }
    } else {
        pthread_t mult_tables_gen_th;
        pthread_create(&mult_tables_gen_th, NULL, generate_mult_tables_th, (void*)&mult_tables);

        verbose("Generating add tables...\n");
        generate_commutative_tables(&add_tables);
        verbose("...done generating add tables\n");

        pthread_join(mult_tables_gen_th, NULL);

        verbose("Generating semirings...\n");
        generate_semirings(&semirings, mult_tables, add_tables);

        generate_arrays(&arrays);
        verbose("Filtering isomorphisms...\n");
        filter_isomorphism(&semirings, arrays);
    }

    verbose("Computing properties...\n");
    compute_properties(&semirings);

    int count;
    int pos;
    find_isomorph_dual(semirings, arrays, &count, &pos);

    if (OUTFILENAME == NULL) {
        fprint_semiring_list(stdout, semirings);
        fprint_stats(stdout, semirings);
        fprintf(stdout, "isomorph_dual_count: %d, ", count);
        fprintf(stdout, "pos: %d\n", pos);
    } else {
        FILE* fptr = fopen(OUTFILENAME, "w");
        verbose("Writing output to file...\n");
        fprint_semiring_list(fptr, semirings);
        fprint_stats(fptr, semirings);
        fprintf(fptr, "isomorph_dual_count: %d, ", count);
        fprintf(fptr, "pos: %d\n", pos);
        fclose(fptr);
    }

    return 0;
}
