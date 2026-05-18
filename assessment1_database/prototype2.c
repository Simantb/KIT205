#include "prototype2.h"

#include <stdio.h>
#include <stdlib.h>

static void fatal_allocation(void) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
}

static size_t next_power_of_two(size_t value) {
    size_t power = 16;
    while (power < value) {
        power *= 2;
    }
    return power;
}

static size_t hash_int(int key) {
    return ((unsigned int)key * 2654435761u);
}

static void hash_table_init(Prototype2HashTable *table, size_t requested_capacity) {
    table->capacity = next_power_of_two(requested_capacity);
    table->count = 0;
    table->entries = calloc(table->capacity, sizeof(*table->entries));

    if (table->entries == NULL) {
        fatal_allocation();
    }
}

static void hash_table_free(Prototype2HashTable *table) {
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->entries[i].in_use) {
            int_array_free(&table->entries[i].related_ids);
        }
    }

    free(table->entries);
    table->entries = NULL;
    table->capacity = 0;
    table->count = 0;
}

static size_t hash_table_probe(const Prototype2HashTable *table, int key, int *found) {
    size_t index = hash_int(key) & (table->capacity - 1);

    while (table->entries[index].in_use) {
        if (table->entries[index].key == key) {
            *found = 1;
            return index;
        }
        index = (index + 1) & (table->capacity - 1);
    }

    *found = 0;
    return index;
}

static void hash_table_reinsert_entry(Prototype2HashTable *table, Prototype2Entry entry) {
    int found = 0;
    size_t index = hash_table_probe(table, entry.key, &found);
    table->entries[index] = entry;
    table->entries[index].in_use = 1;
    table->count++;
}

static void hash_table_resize(Prototype2HashTable *table, size_t new_capacity) {
    Prototype2Entry *old_entries = table->entries;
    size_t old_capacity = table->capacity;

    table->capacity = next_power_of_two(new_capacity);
    table->count = 0;
    table->entries = calloc(table->capacity, sizeof(*table->entries));
    if (table->entries == NULL) {
        fatal_allocation();
    }

    for (size_t i = 0; i < old_capacity; i++) {
        if (old_entries[i].in_use) {
            hash_table_reinsert_entry(table, old_entries[i]);
        }
    }

    free(old_entries);
}

static Prototype2Entry *hash_table_get_or_create(Prototype2HashTable *table, int key) {
    if ((table->count + 1) * 10 >= table->capacity * 7) {
        hash_table_resize(table, table->capacity * 2);
    }

    int found = 0;
    size_t index = hash_table_probe(table, key, &found);

    if (!found) {
        table->entries[index].in_use = 1;
        table->entries[index].key = key;
        int_array_init(&table->entries[index].related_ids);
        table->count++;
    }

    return &table->entries[index];
}

static const Prototype2Entry *hash_table_find(const Prototype2HashTable *table, int key) {
    int found = 0;
    size_t index = hash_table_probe(table, key, &found);
    return found ? &table->entries[index] : NULL;
}

void prototype2_init(Prototype2DB *db) {
    hash_table_init(&db->books, 32);
    hash_table_init(&db->authors, 32);
}

void prototype2_destroy(Prototype2DB *db) {
    hash_table_free(&db->books);
    hash_table_free(&db->authors);
}

void prototype2_link_book_author(Prototype2DB *db, int book_id, int author_id) {
    Prototype2Entry *book = hash_table_get_or_create(&db->books, book_id);
    Prototype2Entry *author = hash_table_get_or_create(&db->authors, author_id);

    if (!int_array_insert_sorted_unique(&book->related_ids, author_id)) {
        fatal_allocation();
    }
    if (!int_array_insert_sorted_unique(&author->related_ids, book_id)) {
        fatal_allocation();
    }
}

void prototype2_get_all_books(const Prototype2DB *db, IntArray *out) {
    int_array_clear(out);

    for (size_t i = 0; i < db->books.capacity; i++) {
        if (db->books.entries[i].in_use) {
            if (!int_array_push_back(out, db->books.entries[i].key)) {
                fatal_allocation();
            }
        }
    }

    int_array_sort(out);
}

int prototype2_get_authors_for_book(const Prototype2DB *db, int book_id, IntArray *out) {
    int_array_clear(out);

    const Prototype2Entry *book = hash_table_find(&db->books, book_id);
    if (book == NULL) {
        return 0;
    }

    for (size_t i = 0; i < book->related_ids.size; i++) {
        if (!int_array_push_back(out, book->related_ids.data[i])) {
            fatal_allocation();
        }
    }

    return 1;
}

int prototype2_get_books_for_author(const Prototype2DB *db, int author_id, IntArray *out) {
    int_array_clear(out);

    const Prototype2Entry *author = hash_table_find(&db->authors, author_id);
    if (author == NULL) {
        return 0;
    }

    for (size_t i = 0; i < author->related_ids.size; i++) {
        if (!int_array_push_back(out, author->related_ids.data[i])) {
            fatal_allocation();
        }
    }

    return 1;
}

void prototype2_print_all_books(const Prototype2DB *db, FILE *out) {
    IntArray values;
    int_array_init(&values);

    prototype2_get_all_books(db, &values);
    fputs("Books: ", out);
    int_array_print(out, &values);
    fputc('\n', out);

    int_array_free(&values);
}

void prototype2_print_authors_for_book(const Prototype2DB *db, int book_id, FILE *out) {
    IntArray values;
    int_array_init(&values);

    if (prototype2_get_authors_for_book(db, book_id, &values)) {
        fprintf(out, "Authors for book %d: ", book_id);
        int_array_print(out, &values);
        fputc('\n', out);
    } else {
        fprintf(out, "Book %d not found.\n", book_id);
    }

    int_array_free(&values);
}

void prototype2_print_books_for_author(const Prototype2DB *db, int author_id, FILE *out) {
    IntArray values;
    int_array_init(&values);

    if (prototype2_get_books_for_author(db, author_id, &values)) {
        fprintf(out, "Books for author %d: ", author_id);
        int_array_print(out, &values);
        fputc('\n', out);
    } else {
        fprintf(out, "Author %d not found.\n", author_id);
    }

    int_array_free(&values);
}
