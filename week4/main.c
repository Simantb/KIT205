#include <stdio.h>
#include <time.h>

/*
Paste the tutorial functions here, above main().
Example format only:

void function1(long n) {
    ...
}

void function2(long n) {
    ...
}
*/

/* Example placeholder functions so the program compiles.
Replace these with the real functions from your tutorial page. */
void function1(long n) {
    for (long i = 0; i < n; i++) {
        volatile long x = i;
    }
}

void function2(long n) {
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            volatile long x = i + j;
        }
    }
}

void function3(long n) {
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            for (long k = 0; k < n; k++) {
                volatile long x = i + j + k;
            }
        }
    }
}

int main(void) {
    long n;

    printf("Enter a value for n\n");
    scanf("%ld", &n);

    clock_t start, diff;
    long msec;

    start = clock();
    function1(n);
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("function1 took %ld milliseconds\n\n", msec);

    start = clock();
    function2(n);
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("function2 took %ld milliseconds\n\n", msec);

    start = clock();
    function3(n);
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("function3 took %ld milliseconds\n\n", msec);

    return 0;
}