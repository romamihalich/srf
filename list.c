#include "list.h"

List list_new() {
    return (List) { .head = NULL, .tail = NULL, .count = 0 };
}

bool list_is_empty(List list) {
    return list.head == NULL;
}

void list_push(List* list, void* item) {
    struct Node* elem = (struct Node*)malloc(sizeof(struct Node));
    elem->value = item;
    elem->next = NULL;
    elem->prev = NULL;

    if (list_is_empty(*list)) {
        list->count = 0;
        list->head = list->tail = elem;
    } else {
        list->tail->next = elem;
        elem->prev = list->tail;
        list->tail = list->tail->next;
    }
    list->count++;
}

void list_delete(List* list, struct Node* elem) {
    if (list_is_empty(*list)) {
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

void list_clear(List* list) {
    struct Node* temp = list->head;
    while (temp != NULL) {
        free(temp->value);
        temp = temp->next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}
