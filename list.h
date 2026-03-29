#ifndef LIST_H
#define LIST_H

typedef struct node {
    int data;
    struct node* next;
} Node;

typedef struct list {
    Node* head;
} List;

void init_list(List* list);
int is_empty(List* list);
void insert_at_front(List* list, int value);
void insert_in_order(List* list, int value);
int delete_value(List* list, int value);
void print_list(List* list);
void destroy_list(List* list);

void list_test(void);
void list_adhoc_test(void);

List reverse(List* list);
List merge(List* list1, List* list2);

#endif