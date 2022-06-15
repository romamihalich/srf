#include "caching.h"

void cache_table(FILE* fptr, int* matrix) {
    fwrite(matrix, sizeof(int), N*N, fptr);
}

void cache_table_list(char* filename, List* tables, bool append) {
    FILE* fptr;
    if (append) {
        fptr = fopen(filename, "ab");
    } else {
        fptr = fopen(filename, "wb");
    }
    // fwrite(&tables->count, sizeof(int), 1, fptr);
    struct Node* temp = tables->head;
    while (temp != NULL) {
        cache_table(fptr, temp->value);
        temp = temp->next;
    }
    fclose(fptr);
}

void read_table_list_cache(List* list, char* filename) {
    FILE* fptr = fopen(filename, "rb");
    // int count = 0;
    // fread(&count, sizeof(int), 1, fptr);
    while (true) {
        int* table = malloc(sizeof(int)*N*N);
        if (fread(table, sizeof(int), N*N, fptr) == 0) {
            free(table);
            break;
        }
        list_push(list, table);
    }
    fclose(fptr);
}

void cache_semiring_list(char* filename, List* semirings) {
    FILE* fptr = fopen(filename, "wb");
    // fwrite(&semirings->count, sizeof(int), 1, fptr);
    struct Node* temp = semirings->head;
    while (temp != NULL) {
        Semiring* semiring = temp->value;
        cache_table(fptr, semiring->add);
        cache_table(fptr, semiring->mult);
        temp = temp->next;
    }
    fclose(fptr);
}

void read_semiring_list_cache(List* list, char* filename) {
    FILE* fptr = fopen(filename, "rb");
    // int count = 0;
    // fread(&count, sizeof(int), 1, fptr);
    while (true) {
        Semiring* semiring = malloc(sizeof(Semiring));
        semiring->add = malloc(sizeof(int)*N*N);
        semiring->mult = malloc(sizeof(int)*N*N);
        size_t add_count = fread(semiring->add, sizeof(int), N*N, fptr);
        size_t mult_count = fread(semiring->mult, sizeof(int), N*N, fptr);
        if (add_count == 0 || mult_count == 0) {
            free(semiring->add);
            free(semiring->mult);
            free(semiring);
            break;
        }
        list_push(list, semiring);
    }
    fclose(fptr);
}
