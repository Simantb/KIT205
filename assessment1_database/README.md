# Assessment Task 1: Database

## Topic

This project models a large publishing database using a **many-to-many relationship** between:

- `Book` records
- `Author` records

Each record stores only an integer identifier, which matches the assignment requirement that only an identifier is necessary.

The relationship is:

- a book can have many authors
- an author can write many books

This is a realistic large-data problem because publishing platforms, libraries, and research indexes often need to store and query very large networks of books and contributors.

## Required Operations

Both prototypes implement:

1. Insert a book associated with an author
2. Print an ordered list of all books
3. Print an ordered list of all authors associated with a given book
4. Print an ordered list of all books associated with a given author

## Prototype Designs

### Prototype 1

- **Sorted linked list** of books
- **Sorted linked list** of authors
- each book stores related authors in an **AVL tree**
- each author stores related books in an **AVL tree**

This prototype is primarily built from:

- linked lists
- binary search trees (AVL trees)

### Prototype 2

- **Hash table** of books
- **Hash table** of authors
- each book stores related authors in a **sorted dynamic array**
- each author stores related books in a **sorted dynamic array**

This prototype is primarily built from:

- hash tables
- arrays

## Time-Critical Operations Chosen for Evaluation

The project evaluates:

1. **Insert relationship**
   Why: in a large database, repeatedly adding associations is a common and performance-sensitive operation.
2. **Lookup authors for a book**
   Why: this is a typical read-heavy query and is a useful comparison across the two designs.

## Complexity Summary

### Prototype 1

- Insert relationship:
  - finding book/author in sorted linked lists is `O(B + A)` in the worst case
  - inserting related IDs into AVL trees is `O(log r)`
  - overall dominated by linked-list search for large datasets
- Print all books:
  - `O(B)`
- Print authors for a book:
  - find book in linked list: `O(B)`
  - in-order traversal of AVL tree: `O(r)`

### Prototype 2

- Insert relationship:
  - hash lookup is `O(1)` average case
  - sorted dynamic array insertion is `O(r)` because values may need shifting
  - overall usually faster for record lookup, but relation insertion cost still grows with related count
- Print all books:
  - collect from hash table and sort keys: `O(B log B)`
- Print authors for a book:
  - hash lookup is `O(1)` average case
  - printing related array is `O(r)`

## Build Instructions

### GCC / Clang

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c common.c prototype1.c prototype2.c tests.c evaluation.c -o assessment1_database
./assessment1_database
```

### Visual Studio

1. Create an empty C project.
2. Add these files:
   - `main.c`
   - `common.c`
   - `common.h`
   - `prototype1.c`
   - `prototype1.h`
   - `prototype2.c`
   - `prototype2.h`
   - `tests.c`
   - `tests.h`
   - `evaluation.c`
   - `evaluation.h`
3. Build and run.

## Program Output

The program:

1. runs unit tests first
2. prints a small example for both prototypes
3. runs the evaluation
4. writes CSV output to `evaluation_results.csv`

## Verified Run Snapshot

This project was compiled and run successfully in the current environment.

Unit test summary:

- `14 passed`
- `0 failed`

Benchmark snapshot from `evaluation_results.csv`:

| Relationships | P1 Insert (ms) | P2 Insert (ms) | P1 Lookup (ms) | P2 Lookup (ms) |
|---|---:|---:|---:|---:|
| 1,000 | 0.627 | 0.216 | 0.182 | 0.014 |
| 3,000 | 3.737 | 0.174 | 0.505 | 0.014 |
| 6,000 | 38.636 | 0.509 | 4.094 | 0.039 |
| 12,000 | 196.220 | 0.766 | 24.358 | 0.131 |
| 24,000 | 462.762 | 2.122 | 119.528 | 0.502 |

## Files Included

- `main.c`
- `common.c`
- `common.h`
- `prototype1.c`
- `prototype1.h`
- `prototype2.c`
- `prototype2.h`
- `tests.c`
- `tests.h`
- `evaluation.c`
- `evaluation.h`
- `poster_notes.md`
- `devlog_sample.csv`
- `build_artifacts.py`
- `output/assessment1_database_poster_a1.pdf`
- `output/assessment1_database_devlog_draft.xlsx`

## Milestone Guidance

Suggested milestones matching the rubric:

1. Initial commit
2. Prototype 1 data structures complete
3. Prototype 2 data structures complete
4. Database implementation complete
5. Evaluation code complete

## Honest Submission Note

This code can satisfy the **implementation, testing, and evaluation** parts of the assignment.

However, the following still need your own final submission work:

- your real GitHub milestone history over time
- any final adjustments needed to match your unit's exact DevLog template
- any final poster wording changes you personally want before submission

Those pieces must reflect what actually happened, so they should not be fabricated.
