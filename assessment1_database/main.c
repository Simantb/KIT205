#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "evaluation.h"
#include "prototype1.h"
#include "prototype2.h"
#include "tests.h"

static void print_example_database_outputs(void) {
    Prototype1DB prototype1;
    Prototype2DB prototype2;

    prototype1_init(&prototype1);
    prototype2_init(&prototype2);

    prototype1_link_book_author(&prototype1, 201, 10);
    prototype1_link_book_author(&prototype1, 201, 12);
    prototype1_link_book_author(&prototype1, 205, 10);
    prototype1_link_book_author(&prototype1, 203, 14);

    prototype2_link_book_author(&prototype2, 201, 10);
    prototype2_link_book_author(&prototype2, 201, 12);
    prototype2_link_book_author(&prototype2, 205, 10);
    prototype2_link_book_author(&prototype2, 203, 14);

    puts("Example output for Prototype 1:");
    prototype1_print_all_books(&prototype1, stdout);
    prototype1_print_authors_for_book(&prototype1, 201, stdout);
    prototype1_print_books_for_author(&prototype1, 10, stdout);
    putchar('\n');

    puts("Example output for Prototype 2:");
    prototype2_print_all_books(&prototype2, stdout);
    prototype2_print_authors_for_book(&prototype2, 201, stdout);
    prototype2_print_books_for_author(&prototype2, 10, stdout);
    putchar('\n');

    prototype1_destroy(&prototype1);
    prototype2_destroy(&prototype2);
}

int main(void) {
    TestStats stats;
    test_stats_init(&stats);

    run_all_tests(&stats);
    if (stats.failed > 0) {
        fprintf(stderr, "Stopping because one or more unit tests failed.\n");
        return EXIT_FAILURE;
    }

    print_example_database_outputs();
    run_evaluation(stdout);

    puts("\nEvaluation results were also written to evaluation_results.csv");
    return EXIT_SUCCESS;
}
