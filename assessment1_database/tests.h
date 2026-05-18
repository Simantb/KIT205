#ifndef TESTS_H
#define TESTS_H

typedef struct {
    int passed;
    int failed;
} TestStats;

void test_stats_init(TestStats *stats);
void run_all_tests(TestStats *stats);

#endif
