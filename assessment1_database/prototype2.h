#ifndef PROTOTYPE2_H
#define PROTOTYPE2_H

#include <stdio.h>

#include "common.h"

typedef struct {
    int in_use;
    int key;
    IntArray related_ids;
} Prototype2Entry;

typedef struct {
    Prototype2Entry *entries;
    size_t capacity;
    size_t count;
} Prototype2HashTable;

typedef struct {
    Prototype2HashTable books;
    Prototype2HashTable authors;
} Prototype2DB;

void prototype2_init(Prototype2DB *db);
void prototype2_destroy(Prototype2DB *db);
void prototype2_link_book_author(Prototype2DB *db, int book_id, int author_id);
void prototype2_get_all_books(const Prototype2DB *db, IntArray *out);
int prototype2_get_authors_for_book(const Prototype2DB *db, int book_id, IntArray *out);
int prototype2_get_books_for_author(const Prototype2DB *db, int author_id, IntArray *out);
void prototype2_print_all_books(const Prototype2DB *db, FILE *out);
void prototype2_print_authors_for_book(const Prototype2DB *db, int book_id, FILE *out);
void prototype2_print_books_for_author(const Prototype2DB *db, int author_id, FILE *out);

#endif
