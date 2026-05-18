#ifndef PROTOTYPE1_H
#define PROTOTYPE1_H

#include <stdio.h>

#include "common.h"

typedef struct Prototype1BookNode Prototype1BookNode;
typedef struct Prototype1AuthorNode Prototype1AuthorNode;

typedef struct {
    Prototype1BookNode *books;
    Prototype1AuthorNode *authors;
    size_t book_count;
    size_t author_count;
} Prototype1DB;

void prototype1_init(Prototype1DB *db);
void prototype1_destroy(Prototype1DB *db);
void prototype1_link_book_author(Prototype1DB *db, int book_id, int author_id);
void prototype1_get_all_books(const Prototype1DB *db, IntArray *out);
int prototype1_get_authors_for_book(const Prototype1DB *db, int book_id, IntArray *out);
int prototype1_get_books_for_author(const Prototype1DB *db, int author_id, IntArray *out);
void prototype1_print_all_books(const Prototype1DB *db, FILE *out);
void prototype1_print_authors_for_book(const Prototype1DB *db, int book_id, FILE *out);
void prototype1_print_books_for_author(const Prototype1DB *db, int author_id, FILE *out);

#endif
