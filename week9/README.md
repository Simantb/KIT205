# Week 9 PageRank

Files included:

- `graph.h`
- `graph.c`
- `main.c`
- `graph.txt`

Compile:

```bash
gcc main.c graph.c -o week9
```

Run:

```bash
./week9
```

This solution uses `20` iterations and the un-normalised damping-factor equation from the tutorial.

Expected output:

```text
PageRank after 20 iterations:
Vertex 0: 1.608530
Vertex 1: 1.850712
Vertex 2: 1.260900
Vertex 3: 3.388913
Vertex 4: 1.315152
Vertex 5: 0.353585
Vertex 6: 0.316091
Vertex 7: 0.431018
Vertex 8: 0.977003
Vertex 9: 0.833626
Vertex 10: 1.261476
Vertex 11: 3.377054
Vertex 12: 0.803865
Vertex 13: 0.552155
Vertex 14: 0.150000
Vertex 15: 0.635686
Vertex 16: 0.181875
Vertex 17: 0.263671
Vertex 18: 0.150000
Vertex 19: 0.288690
```
