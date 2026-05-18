# Poster Notes

## Title

**Comparing Two C Database Prototypes for Large Publishing Networks**  
Simant Bhattarai  
GitHub: https://github.com/Simantb/KIT205/tree/main/assessment1_database

## Introduction

This project investigates how different data-structure choices affect the performance of a database-style system that stores relationships between books and authors. Publishing data is naturally many-to-many because a single book may have multiple authors and an author may contribute to multiple books. In large catalogues, this relationship can become very large and performance-sensitive. The project compares two alternative C prototypes that support ordered output while prioritising efficient insertion and lookup.

## Prototype Summary

### Prototype 1

- sorted linked list of books
- sorted linked list of authors
- AVL tree of authors inside each book
- AVL tree of books inside each author

### Prototype 2

- hash table of books
- hash table of authors
- sorted dynamic array of authors inside each book
- sorted dynamic array of books inside each author

## Most Significant Operations

- Insert book-author relationship
- Find and print authors for a given book

These were chosen because they are central to maintaining and querying a large relationship database.

## Methodology

The prototypes were tested with simulated large datasets. For each test size, the program generated many book-author relationships and measured:

1. total time to insert all relationships
2. total time to perform repeated author lookups by book ID

Testing was done using the same generated data for both prototypes to make the comparison fair.

## Results

Use the data from `evaluation_results.csv` in a graph.

Suggested graph:

- x-axis: number of relationships
- y-axis: time in milliseconds
- lines:
  - Prototype 1 insert
  - Prototype 2 insert
  - Prototype 1 lookup
  - Prototype 2 lookup

Current measured results from the verified run on 18 May 2026:

| Relationships | P1 Insert (ms) | P2 Insert (ms) | P1 Lookup (ms) | P2 Lookup (ms) |
|---|---:|---:|---:|---:|
| 1,000 | 0.627 | 0.216 | 0.182 | 0.014 |
| 3,000 | 3.737 | 0.174 | 0.505 | 0.014 |
| 6,000 | 38.636 | 0.509 | 4.094 | 0.039 |
| 12,000 | 196.220 | 0.766 | 24.358 | 0.131 |
| 24,000 | 462.762 | 2.122 | 119.528 | 0.502 |

## Theoretical Analysis

Prototype 1 performs well for maintaining ordered related data because AVL trees keep associations balanced. However, record lookup is slowed by linked-list traversal.

Prototype 2 benefits from average-case constant-time hash lookup, but keeping related arrays ordered means insertion may require shifting elements.

## Conclusion

The comparison demonstrates that choosing data structures depends on which operation matters most. In the current measured run, Prototype 2 was substantially faster for both inserts and book-to-author lookups because it avoids the linear linked-list search cost that dominates Prototype 1 at larger scales. Prototype 1 still demonstrates a valid ordered tree-based design, but Prototype 2 is the stronger recommendation for very large publishing datasets. The project shows how asymptotic reasoning and measured timing together help select suitable designs for practical problems.

## References / Resources

Add the actual sources you used, for example:

- unit lecture notes
- tutorial materials
- C documentation
- GitHub repository link
- any online AVL/hash-table references
- any AI tools used
