#include "evaluation.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "prototype1.h"
#include "prototype2.h"

typedef struct {
    int relationship_count;
    double prototype1_insert_ms;
    double prototype2_insert_ms;
    double prototype1_lookup_ms;
    double prototype2_lookup_ms;
} BenchmarkRow;

static double elapsed_ms(clock_t start, clock_t end) {
    return (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

static void generate_relationship(int index, int book_count, int author_count, int *book_id, int *author_id) {
    *book_id = ((index * 37) + (index / 7) + 101) % book_count + 1000;
    *author_id = ((index * 53) + (index / 5) + 11) % author_count + 5000;
}

static void benchmark_row(BenchmarkRow *row) {
    const int book_count = row->relationship_count / 2 + 50;
    const int author_count = row->relationship_count / 3 + 50;
    const int lookup_count = row->relationship_count / 4;
    Prototype1DB prototype1;
    Prototype2DB prototype2;
    IntArray scratch;

    prototype1_init(&prototype1);
    prototype2_init(&prototype2);
    int_array_init(&scratch);

    clock_t start = clock();
    for (int i = 0; i < row->relationship_count; i++) {
        int book_id;
        int author_id;
        generate_relationship(i, book_count, author_count, &book_id, &author_id);
        prototype1_link_book_author(&prototype1, book_id, author_id);
    }
    clock_t end = clock();
    row->prototype1_insert_ms = elapsed_ms(start, end);

    start = clock();
    for (int i = 0; i < row->relationship_count; i++) {
        int book_id;
        int author_id;
        generate_relationship(i, book_count, author_count, &book_id, &author_id);
        prototype2_link_book_author(&prototype2, book_id, author_id);
    }
    end = clock();
    row->prototype2_insert_ms = elapsed_ms(start, end);

    start = clock();
    for (int i = 0; i < lookup_count; i++) {
        int book_id = (i * 17) % book_count + 1000;
        prototype1_get_authors_for_book(&prototype1, book_id, &scratch);
    }
    end = clock();
    row->prototype1_lookup_ms = elapsed_ms(start, end);

    start = clock();
    for (int i = 0; i < lookup_count; i++) {
        int book_id = (i * 17) % book_count + 1000;
        prototype2_get_authors_for_book(&prototype2, book_id, &scratch);
    }
    end = clock();
    row->prototype2_lookup_ms = elapsed_ms(start, end);

    int_array_free(&scratch);
    prototype1_destroy(&prototype1);
    prototype2_destroy(&prototype2);
}

void run_evaluation(FILE *out) {
    BenchmarkRow rows[] = {
        {1000, 0, 0, 0, 0},
        {3000, 0, 0, 0, 0},
        {6000, 0, 0, 0, 0},
        {12000, 0, 0, 0, 0},
        {24000, 0, 0, 0, 0}
    };
    const size_t row_count = sizeof(rows) / sizeof(rows[0]);
    FILE *csv = fopen("evaluation_results.csv", "w");

    if (csv != NULL) {
        fputs("relationships,prototype1_insert_ms,prototype2_insert_ms,prototype1_lookup_ms,prototype2_lookup_ms\n", csv);
    }

    fprintf(out, "Performance evaluation for the books-authors database\n");
    fprintf(out, "Prototype 1 = sorted linked lists + AVL trees\n");
    fprintf(out, "Prototype 2 = hash tables + sorted dynamic arrays\n\n");
    fprintf(out, "%-14s %-20s %-20s %-20s %-20s\n",
            "Relationships",
            "P1 Insert (ms)",
            "P2 Insert (ms)",
            "P1 Lookup (ms)",
            "P2 Lookup (ms)");

    for (size_t i = 0; i < row_count; i++) {
        benchmark_row(&rows[i]);

        fprintf(out, "%-14d %-20.3f %-20.3f %-20.3f %-20.3f\n",
                rows[i].relationship_count,
                rows[i].prototype1_insert_ms,
                rows[i].prototype2_insert_ms,
                rows[i].prototype1_lookup_ms,
                rows[i].prototype2_lookup_ms);

        if (csv != NULL) {
            fprintf(csv, "%d,%.3f,%.3f,%.3f,%.3f\n",
                    rows[i].relationship_count,
                    rows[i].prototype1_insert_ms,
                    rows[i].prototype2_insert_ms,
                    rows[i].prototype1_lookup_ms,
                    rows[i].prototype2_lookup_ms);
        }
    }

    if (csv != NULL) {
        fclose(csv);
    }
}
