#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int N;

struct Node {
    void* value;
    struct Node* next;
    struct Node* prev;
};

typedef struct {
    struct Node* head;
    struct Node* tail;
    int count;
} List;

bool is_empty(List list) {
    return list.head == NULL;
}

void push(List* list, void* item) {
    struct Node* elem = (struct Node*)malloc(sizeof(struct Node));
    elem->value = item;
    elem->next = NULL;
    elem->prev = NULL;

    if (is_empty(*list)) {
        list->count = 0;
        list->head = list->tail = elem;
    } else {
        list->tail->next = elem;
        elem->prev = list->tail;
        list->tail = list->tail->next;
    }
    list->count++;
}

void delete(List* list, struct Node* elem) {
    if (is_empty(*list)) {
        printf("Nothing to delete. Exiting...");
        exit(1);
    }

    if (elem->prev == NULL) {
        list->head = list->head->next;
        if (list->head == NULL)
            list->tail = NULL;
        else list->head->prev = NULL;
    } else if (elem->next == NULL) {
        list->tail = list->tail->prev;
        if (list->tail == NULL)
            list->head = NULL;
        else list->tail->next = NULL;
    } else {
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
    }
    list->count--;
    free(elem);
}

typedef struct {
    int* mult;
    int* add;
} Semiring;

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

bool isdistributive(int mult[N*N], int add[N*N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (mult[i*N + add[j*N + k]] != add[mult[i*N + j]*N + mult[i*N + k]])
                    return false;
            }
        }
    }
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

void find_isomorphism(int arr[N], int pos, Semiring r1, Semiring r2, bool* result) {
    if (pos == N) {
        if (isisomorphism(arr, r1, r2))
            *result = true;
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
            find_isomorphism(arr, pos + 1, r1, r2, result);
        }
    }
}

bool areisomorphic(Semiring r1, Semiring r2) {
    bool result = false;
    int arr[N];
    find_isomorphism(arr, 0, r1, r2, &result);
    return result;
}

//  0 0  0 0  0 0  0 0  0 1  0 1  0 1  0 1  1 0  1 0  1 0  1 0  1 1  1 1  1 1  1 1 
//  0 0  0 1  1 0  1 1  0 0  0 1  1 0  1 1  0 0  0 1  1 0  1 1  0 0  0 1  1 0  1 1

void generate_tables_rec(int matrix[N*N], int pos, List* mult_tables, List* add_tables) {
    if (pos == N*N) {
        if (isassociative(matrix)) {
            if (isidempotent(matrix)) {
                int* matrix_copy = (int*)malloc(N*N*sizeof(int));
                if (matrix_copy == NULL) {
                    printf("OS didn't give memory for matrix. Exiting...");
                    exit(1);
                }
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        matrix_copy[i*N + j] = matrix[i*N + j];
                    }
                }
                push(mult_tables, matrix_copy);
            }
            if (iscommutative(matrix)) {
                int* matrix_copy = (int*)malloc(N*N*sizeof(int));
                if (matrix_copy == NULL) {
                    printf("OS didn't give memory for matrix. Exiting...");
                    exit(1);
                }
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        matrix_copy[i*N + j] = matrix[i*N + j];
                    }
                }
                push(add_tables, matrix_copy);
            }
        }
        return;
    }
    for (int i = 0; i < N; i++) {
        matrix[pos] = i;
        generate_tables_rec(matrix, pos + 1, mult_tables, add_tables);
    }
}

void generate_tables(List* mult_tables, List* add_tables) {
    int matrix[N*N];
    generate_tables_rec(matrix, 0, mult_tables, add_tables);
}

void generate_semirings(List* semirings) {
    List mult_tables = { .head = NULL, .tail = NULL, .count = 0 };
    List add_tables  = { .head = NULL, .tail = NULL, .count = 0 };

    generate_tables(&mult_tables, &add_tables);
    struct Node* mult_temp = mult_tables.head;
    while (mult_temp != NULL) {
        struct Node* add_temp = add_tables.head;
        while (add_temp != NULL) {
            if (isdistributive(mult_temp->value, add_temp->value)) {
                Semiring* semiring = (Semiring*)malloc(sizeof(Semiring));
                semiring->mult = mult_temp->value;
                semiring->add = add_temp->value;
                push(semirings, semiring);
            }
            add_temp = add_temp->next;
        }
        mult_temp = mult_temp->next;
    }
}

void print_semiring(Semiring semiring) {
    printf("mult");
    printf("%*c", 2*N - 3, ' ');
    printf("add\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", semiring.mult[i*N + j]);
        }
        printf(" ");
        for (int j = 0; j < N; j++) {
            printf("%d ", semiring.add[i*N + j]);
        }
        printf("\n");
    }
}

void print_semiring_list(List list) {
    struct Node* temp = list.head;
    while (temp != NULL) {
        Semiring semiring = *((Semiring*)temp->value);
        print_semiring(semiring);
        printf("\n");
        temp = temp->next;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong number of arguments. Expected 1, but was: %d\n", argc - 1);
        exit(1);
    }
    char* input = argv[argc - 1];
    N = atoi(input);
    if (N == 0) {
        printf("No positive integer provided\n");
        exit(1);
    }

    List semirings = { .head = NULL, .tail = NULL, .count = 0 };
    generate_semirings(&semirings);

    struct Node* temp = semirings.head;
    while (temp != NULL) {
        struct Node* temp_inner = temp->next;
        while (temp_inner != NULL) {
            if (areisomorphic(*((Semiring*)temp->value), *((Semiring*)temp_inner->value))) {
                struct Node* temp_next = temp_inner->next;
                delete(&semirings, temp_inner);
                temp_inner = temp_next;
            } else temp_inner = temp_inner->next;
        }
        temp = temp->next;
    }

    print_semiring_list(semirings);
    printf("semirings.count: %d\n", semirings.count);
    return 0;
}
