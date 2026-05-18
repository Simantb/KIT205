#include "prototype1.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Prototype1AvlNode {
    int key;
    int height;
    struct Prototype1AvlNode *left;
    struct Prototype1AvlNode *right;
} Prototype1AvlNode;

struct Prototype1BookNode {
    int id;
    Prototype1AvlNode *author_ids;
    struct Prototype1BookNode *next;
};

struct Prototype1AuthorNode {
    int id;
    Prototype1AvlNode *book_ids;
    struct Prototype1AuthorNode *next;
};

static void fatal_allocation(void) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
}

static int avl_height(Prototype1AvlNode *node) {
    return node == NULL ? 0 : node->height;
}

static int avl_max(int a, int b) {
    return a > b ? a : b;
}

static Prototype1AvlNode *avl_create_node(int key) {
    Prototype1AvlNode *node = malloc(sizeof(*node));
    if (node == NULL) {
        fatal_allocation();
    }

    node->key = key;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static Prototype1AvlNode *avl_rotate_right(Prototype1AvlNode *node) {
    Prototype1AvlNode *pivot = node->left;
    Prototype1AvlNode *temp = pivot->right;

    pivot->right = node;
    node->left = temp;

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));
    pivot->height = 1 + avl_max(avl_height(pivot->left), avl_height(pivot->right));

    return pivot;
}

static Prototype1AvlNode *avl_rotate_left(Prototype1AvlNode *node) {
    Prototype1AvlNode *pivot = node->right;
    Prototype1AvlNode *temp = pivot->left;

    pivot->left = node;
    node->right = temp;

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));
    pivot->height = 1 + avl_max(avl_height(pivot->left), avl_height(pivot->right));

    return pivot;
}

static int avl_balance_factor(Prototype1AvlNode *node) {
    return node == NULL ? 0 : avl_height(node->left) - avl_height(node->right);
}

static Prototype1AvlNode *avl_insert(Prototype1AvlNode *node, int key) {
    if (node == NULL) {
        return avl_create_node(key);
    }

    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        return node;
    }

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));

    int balance = avl_balance_factor(node);

    if (balance > 1 && key < node->left->key) {
        return avl_rotate_right(node);
    }
    if (balance < -1 && key > node->right->key) {
        return avl_rotate_left(node);
    }
    if (balance > 1 && key > node->left->key) {
        node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }

    return node;
}

static void avl_collect_in_order(Prototype1AvlNode *node, IntArray *out) {
    if (node == NULL) {
        return;
    }

    avl_collect_in_order(node->left, out);
    if (!int_array_push_back(out, node->key)) {
        fatal_allocation();
    }
    avl_collect_in_order(node->right, out);
}

static void avl_destroy(Prototype1AvlNode *node) {
    if (node == NULL) {
        return;
    }

    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

static Prototype1BookNode *prototype1_get_or_create_book(Prototype1DB *db, int book_id) {
    Prototype1BookNode **current = &db->books;

    while (*current != NULL && (*current)->id < book_id) {
        current = &(*current)->next;
    }

    if (*current != NULL && (*current)->id == book_id) {
        return *current;
    }

    Prototype1BookNode *node = malloc(sizeof(*node));
    if (node == NULL) {
        fatal_allocation();
    }

    node->id = book_id;
    node->author_ids = NULL;
    node->next = *current;
    *current = node;
    db->book_count++;
    return node;
}

static Prototype1AuthorNode *prototype1_get_or_create_author(Prototype1DB *db, int author_id) {
    Prototype1AuthorNode **current = &db->authors;

    while (*current != NULL && (*current)->id < author_id) {
        current = &(*current)->next;
    }

    if (*current != NULL && (*current)->id == author_id) {
        return *current;
    }

    Prototype1AuthorNode *node = malloc(sizeof(*node));
    if (node == NULL) {
        fatal_allocation();
    }

    node->id = author_id;
    node->book_ids = NULL;
    node->next = *current;
    *current = node;
    db->author_count++;
    return node;
}

static const Prototype1BookNode *prototype1_find_book(const Prototype1DB *db, int book_id) {
    const Prototype1BookNode *current = db->books;

    while (current != NULL && current->id < book_id) {
        current = current->next;
    }

    if (current != NULL && current->id == book_id) {
        return current;
    }
    return NULL;
}

static const Prototype1AuthorNode *prototype1_find_author(const Prototype1DB *db, int author_id) {
    const Prototype1AuthorNode *current = db->authors;

    while (current != NULL && current->id < author_id) {
        current = current->next;
    }

    if (current != NULL && current->id == author_id) {
        return current;
    }
    return NULL;
}

void prototype1_init(Prototype1DB *db) {
    db->books = NULL;
    db->authors = NULL;
    db->book_count = 0;
    db->author_count = 0;
}

void prototype1_destroy(Prototype1DB *db) {
    Prototype1BookNode *book = db->books;
    while (book != NULL) {
        Prototype1BookNode *next = book->next;
        avl_destroy(book->author_ids);
        free(book);
        book = next;
    }

    Prototype1AuthorNode *author = db->authors;
    while (author != NULL) {
        Prototype1AuthorNode *next = author->next;
        avl_destroy(author->book_ids);
        free(author);
        author = next;
    }

    prototype1_init(db);
}

void prototype1_link_book_author(Prototype1DB *db, int book_id, int author_id) {
    Prototype1BookNode *book = prototype1_get_or_create_book(db, book_id);
    Prototype1AuthorNode *author = prototype1_get_or_create_author(db, author_id);

    book->author_ids = avl_insert(book->author_ids, author_id);
    author->book_ids = avl_insert(author->book_ids, book_id);
}

void prototype1_get_all_books(const Prototype1DB *db, IntArray *out) {
    int_array_clear(out);

    const Prototype1BookNode *current = db->books;
    while (current != NULL) {
        if (!int_array_push_back(out, current->id)) {
            fatal_allocation();
        }
        current = current->next;
    }
}

int prototype1_get_authors_for_book(const Prototype1DB *db, int book_id, IntArray *out) {
    int_array_clear(out);

    const Prototype1BookNode *book = prototype1_find_book(db, book_id);
    if (book == NULL) {
        return 0;
    }

    avl_collect_in_order(book->author_ids, out);
    return 1;
}

int prototype1_get_books_for_author(const Prototype1DB *db, int author_id, IntArray *out) {
    int_array_clear(out);

    const Prototype1AuthorNode *author = prototype1_find_author(db, author_id);
    if (author == NULL) {
        return 0;
    }

    avl_collect_in_order(author->book_ids, out);
    return 1;
}

void prototype1_print_all_books(const Prototype1DB *db, FILE *out) {
    IntArray values;
    int_array_init(&values);

    prototype1_get_all_books(db, &values);
    fputs("Books: ", out);
    int_array_print(out, &values);
    fputc('\n', out);

    int_array_free(&values);
}

void prototype1_print_authors_for_book(const Prototype1DB *db, int book_id, FILE *out) {
    IntArray values;
    int_array_init(&values);

    if (prototype1_get_authors_for_book(db, book_id, &values)) {
        fprintf(out, "Authors for book %d: ", book_id);
        int_array_print(out, &values);
        fputc('\n', out);
    } else {
        fprintf(out, "Book %d not found.\n", book_id);
    }

    int_array_free(&values);
}

void prototype1_print_books_for_author(const Prototype1DB *db, int author_id, FILE *out) {
    IntArray values;
    int_array_init(&values);

    if (prototype1_get_books_for_author(db, author_id, &values)) {
        fprintf(out, "Books for author %d: ", author_id);
        int_array_print(out, &values);
        fputc('\n', out);
    } else {
        fprintf(out, "Author %d not found.\n", author_id);
    }

    int_array_free(&values);
}
