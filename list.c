#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static void option_insert(List* my_list) {
    int value;
    printf("Enter value to insert: ");
    scanf("%d", &value);
    insert_in_order(my_list, value);
    printf("Inserted %d\n", value);
}

static void option_delete(List* my_list) {
    int value;
    printf("Enter value to delete: ");
    scanf("%d", &value);

    if (delete_value(my_list, value)) {
        printf("%d deleted successfully.\n", value);
    } else {
        printf("%d not found in list.\n", value);
    }
}

static void option_print(List* my_list) {
    printf("Current list: ");
    print_list(my_list);
}

void init_list(List* list) {
    list->head = NULL;
}

int is_empty(List* list) {
    return list->head == NULL;
}

void insert_at_front(List* list, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
}

void insert_in_order(List* list, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    new_node->data = value;
    new_node->next = NULL;

    if (list->head == NULL || value < list->head->data) {
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    Node* current = list->head;

    while (current->next != NULL && current->next->data < value) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

int delete_value(List* list, int value) {
    if (list->head == NULL) {
        return 0;
    }

    Node* current = list->head;
    Node* previous = NULL;

    while (current != NULL && current->data != value) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return 0;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
    return 1;
}

void print_list(List* list) {
    Node* current = list->head;

    if (current == NULL) {
        printf("EMPTY\n");
        return;
    }

    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("\n");
}

void destroy_list(List* list) {
    Node* current = list->head;

    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    list->head = NULL;
}

List reverse(List* list) {
    List reversed;
    init_list(&reversed);

    Node* current = list->head;
    while (current != NULL) {
        insert_at_front(&reversed, current->data);
        current = current->next;
    }

    return reversed;
}

List merge(List* list1, List* list2) {
    List merged;
    init_list(&merged);

    Node* p1 = list1->head;
    Node* p2 = list2->head;

    while (p1 != NULL && p2 != NULL) {
        if (p1->data <= p2->data) {
            insert_in_order(&merged, p1->data);
            p1 = p1->next;
        } else {
            insert_in_order(&merged, p2->data);
            p2 = p2->next;
        }
    }

    while (p1 != NULL) {
        insert_in_order(&merged, p1->data);
        p1 = p1->next;
    }

    while (p2 != NULL) {
        insert_in_order(&merged, p2->data);
        p2 = p2->next;
    }

    return merged;
}

void list_test(void) {
    List my_list;
    init_list(&my_list);

    printf("Testing insert_at_front...\n");
    insert_at_front(&my_list, 5);
    insert_at_front(&my_list, 3);
    insert_at_front(&my_list, 7);
    insert_at_front(&my_list, 2);
    insert_at_front(&my_list, 0);

    printf("Expected: 0, 2, 7, 3, 5\n");
    printf("Result:   ");
    print_list(&my_list);
    printf("\n");

    destroy_list(&my_list);
    init_list(&my_list);

    printf("Testing insert_in_order...\n");
    insert_in_order(&my_list, 5);
    insert_in_order(&my_list, 3);
    insert_in_order(&my_list, 7);
    insert_in_order(&my_list, 2);
    insert_in_order(&my_list, 0);

    printf("Expected: 0, 2, 3, 5, 7\n");
    printf("Result:   ");
    print_list(&my_list);
    printf("\n");

    printf("Testing delete_value(3)...\n");
    delete_value(&my_list, 3);
    printf("Expected: 0, 2, 5, 7\n");
    printf("Result:   ");
    print_list(&my_list);
    printf("\n");

    printf("Testing reverse...\n");
    List reversed = reverse(&my_list);
    printf("Expected: 7, 5, 2, 0\n");
    printf("Result:   ");
    print_list(&reversed);
    printf("\n");

    destroy_list(&my_list);
    destroy_list(&reversed);

    List list1, list2, merged_list;
    init_list(&list1);
    init_list(&list2);

    insert_in_order(&list1, 1);
    insert_in_order(&list1, 3);
    insert_in_order(&list1, 5);

    insert_in_order(&list2, 2);
    insert_in_order(&list2, 4);
    insert_in_order(&list2, 6);

    printf("Testing merge...\n");
    merged_list = merge(&list1, &list2);
    printf("Expected: 1, 2, 3, 4, 5, 6\n");
    printf("Result:   ");
    print_list(&merged_list);
    printf("\n");

    destroy_list(&list1);
    destroy_list(&list2);
    destroy_list(&merged_list);
}

void list_adhoc_test(void) {
    List my_list;
    init_list(&my_list);

    int quit = 0;

    while (!quit) {
        int option;

        printf("\n--- Linked List Menu ---\n");
        printf("1. Insert\n");
        printf("2. Delete\n");
        printf("3. Print\n");
        printf("0. Quit\n");
        printf("Choose option: ");
        scanf("%d", &option);

        if (option == 0) {
            quit = 1;
        } else if (option == 1) {
            option_insert(&my_list);
        } else if (option == 2) {
            option_delete(&my_list);
        } else if (option == 3) {
            option_print(&my_list);
        } else {
            printf("Invalid option.\n");
        }
    }

    destroy_list(&my_list);
}