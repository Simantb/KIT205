#include "tests.h"

#include <stdio.h>

#include "common.h"
#include "prototype1.h"
#include "prototype2.h"

static void test_check(TestStats *stats, int condition, const char *message) {
    if (condition) {
        stats->passed++;
        printf("[PASS] %s\n", message);
    } else {
        stats->failed++;
        printf("[FAIL] %s\n", message);
    }
}

void test_stats_init(TestStats *stats) {
    stats->passed = 0;
    stats->failed = 0;
}

static void load_sample_data_prototype1(Prototype1DB *db) {
    prototype1_link_book_author(db, 103, 2);
    prototype1_link_book_author(db, 101, 4);
    prototype1_link_book_author(db, 101, 1);
    prototype1_link_book_author(db, 102, 1);
    prototype1_link_book_author(db, 101, 1);
    prototype1_link_book_author(db, 102, 5);
    prototype1_link_book_author(db, 103, 1);
}

static void load_sample_data_prototype2(Prototype2DB *db) {
    prototype2_link_book_author(db, 103, 2);
    prototype2_link_book_author(db, 101, 4);
    prototype2_link_book_author(db, 101, 1);
    prototype2_link_book_author(db, 102, 1);
    prototype2_link_book_author(db, 101, 1);
    prototype2_link_book_author(db, 102, 5);
    prototype2_link_book_author(db, 103, 1);
}

static void run_prototype1_tests(TestStats *stats) {
    Prototype1DB db;
    IntArray values;
    int expected_books[] = {101, 102, 103};
    int expected_book101_authors[] = {1, 4};
    int expected_author1_books[] = {101, 102, 103};

    prototype1_init(&db);
    int_array_init(&values);

    load_sample_data_prototype1(&db);

    prototype1_get_all_books(&db, &values);
    test_check(stats, int_array_equals(&values, expected_books, 3), "Prototype 1 prints books in ascending order");

    test_check(stats, prototype1_get_authors_for_book(&db, 101, &values), "Prototype 1 finds an existing book");
    test_check(stats, int_array_equals(&values, expected_book101_authors, 2), "Prototype 1 keeps related authors ordered and unique");

    test_check(stats, prototype1_get_books_for_author(&db, 1, &values), "Prototype 1 finds an existing author");
    test_check(stats, int_array_equals(&values, expected_author1_books, 3), "Prototype 1 returns ordered books for an author");

    test_check(stats, !prototype1_get_authors_for_book(&db, 999, &values), "Prototype 1 handles missing books");
    test_check(stats, !prototype1_get_books_for_author(&db, 999, &values), "Prototype 1 handles missing authors");

    int_array_free(&values);
    prototype1_destroy(&db);
}

static void run_prototype2_tests(TestStats *stats) {
    Prototype2DB db;
    IntArray values;
    int expected_books[] = {101, 102, 103};
    int expected_book101_authors[] = {1, 4};
    int expected_author1_books[] = {101, 102, 103};

    prototype2_init(&db);
    int_array_init(&values);

    load_sample_data_prototype2(&db);

    prototype2_get_all_books(&db, &values);
    test_check(stats, int_array_equals(&values, expected_books, 3), "Prototype 2 prints books in ascending order");

    test_check(stats, prototype2_get_authors_for_book(&db, 101, &values), "Prototype 2 finds an existing book");
    test_check(stats, int_array_equals(&values, expected_book101_authors, 2), "Prototype 2 keeps related authors ordered and unique");

    test_check(stats, prototype2_get_books_for_author(&db, 1, &values), "Prototype 2 finds an existing author");
    test_check(stats, int_array_equals(&values, expected_author1_books, 3), "Prototype 2 returns ordered books for an author");

    test_check(stats, !prototype2_get_authors_for_book(&db, 999, &values), "Prototype 2 handles missing books");
    test_check(stats, !prototype2_get_books_for_author(&db, 999, &values), "Prototype 2 handles missing authors");

    int_array_free(&values);
    prototype2_destroy(&db);
}

void run_all_tests(TestStats *stats) {
    printf("Running unit tests...\n");
    run_prototype1_tests(stats);
    run_prototype2_tests(stats);
    printf("Tests complete: %d passed, %d failed\n\n", stats->passed, stats->failed);
}
