#ifndef LIST_H
#define LIST_H

struct Node {
    void* value;
    struct Node* next;
    struct Node* prev;
};

typedef struct List {
    struct Node* head;
    struct Node* tail;
    int count;
} List;

List list_new();
bool list_is_empty(List list);
void list_push(List* list, void* item);
void list_delete(List* list, struct Node* elem);

#endif
