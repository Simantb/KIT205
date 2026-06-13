
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"
#include "shortest_path.h"

/* ------------------------------------------------------------------ */
/* Part 2: Performance comparison                                       */
/* ------------------------------------------------------------------ */

#define REPEATS 20

static long avg_dijkstra_us(Graph *g, int source) {
    long total = 0;
    for (int r = 0; r < REPEATS; r++) {
        PathResult p = dijkstra(g, source);
        total += p.time_us;
        destroy_result(&p);
    }
    return total / REPEATS;
}

static long avg_bellman_ford_us(Graph *g, int source) {
    long total = 0;
    for (int r = 0; r < REPEATS; r++) {
        PathResult p = bellman_ford(g, source);
        total += p.time_us;
        destroy_result(&p);
    }
    return total / REPEATS;
}

/* Verify both algorithms give the same distances (sanity check). */
static int algorithms_agree(Graph *g, int source) {
    PathResult d  = dijkstra(g, source);
    PathResult bf = bellman_ford(g, source);
    int agree = 1;
    for (int i = 0; i < g->V; i++)
        if (d.dist[i] != bf.dist[i]) { agree = 0; break; }
    destroy_result(&d);
    destroy_result(&bf);
    return agree;
}

void run_comparison(void) {
    printf("\n====== Part 2: Performance Comparison – Dijkstra vs Bellman-Ford ======\n\n");
    printf("Context: social networks are sparse (avg degree ~10).\n");
    printf("         Dijkstra's array min-scan costs O(V^2) regardless of E.\n");
    printf("         Bellman-Ford with early termination converges fast on sparse graphs.\n");
    printf("Each result is the average of %d runs (microseconds).\n\n", REPEATS);

    srand(42);   /* fixed seed for reproducibility */

    /* --- Experiment 1: vary graph size, fixed avg degree ~10 --- */
    printf("--- Experiment 1: varying V, fixed avg degree ~10 ---\n\n");
    printf("%-10s %-10s %-16s %-16s %-10s\n",
           "Vertices", "Edges", "Dijkstra (us)", "B-Ford (us)", "Speedup");
    printf("------------------------------------------------------------------\n");

    int test_v[] = {100, 500, 1000, 2000, 5000};
    int n_tests  = 5;

    for (int t = 0; t < n_tests; t++) {
        int v = test_v[t];
        int e = v * 5;   /* 5 undirected edges per vertex -> avg degree 10 */

        Graph g = generate_random_graph(v, e, 100);

        if (!algorithms_agree(&g, 0))
            printf("  WARNING: algorithms disagree for V=%d!\n", v);

        long d_us  = avg_dijkstra_us(&g, 0);
        long bf_us = avg_bellman_ford_us(&g, 0);

        if (d_us == 0 && bf_us == 0)
            printf("%-10d %-10d %-16ld %-16ld  <1us each\n", v, g.E, d_us, bf_us);
        else if (d_us == 0)
            printf("%-10d %-10d %-16ld %-16ld  Dijkstra <1us\n", v, g.E, d_us, bf_us);
        else {
            double speedup = (double)bf_us / (double)d_us;
            printf("%-10d %-10d %-16ld %-16ld  %.2fx\n", v, g.E, d_us, bf_us, speedup);
        }

        destroy_graph(&g);
    }

    /* --- Experiment 2: vary edge density, fixed V = 1000 --- */
    printf("\n--- Experiment 2: varying E, fixed V=1000 ---\n\n");
    printf("%-10s %-10s %-16s %-16s\n",
           "Edges", "Avg deg", "Dijkstra (us)", "B-Ford (us)");
    printf("------------------------------------------------------\n");

    int edge_counts[] = {2000, 5000, 10000, 20000, 50000};
    for (int t = 0; t < 5; t++) {
        int e     = edge_counts[t];
        Graph g   = generate_random_graph(1000, e, 100);
        long d_us  = avg_dijkstra_us(&g, 0);
        long bf_us = avg_bellman_ford_us(&g, 0);
        printf("%-10d %-10d %-16ld %-16ld\n", g.E, g.E / 1000, d_us, bf_us);
        destroy_graph(&g);
    }

    printf("\n--- Theoretical complexity summary ---\n");
    printf("  Dijkstra (array min-scan): O(V^2 + E)\n");
    printf("  Bellman-Ford (early term): O(V * E) worst case, often much less\n");
    printf("\n  For sparse graphs (E ~ V, avg degree constant):\n");
    printf("    Dijkstra : O(V^2)  -- min-scan dominates\n");
    printf("    BF       : O(V * V) = O(V^2) worst, but early term makes it O(E * iters)\n");
    printf("    -> BF can outperform Dijkstra on sparse social networks\n");
    printf("\n  For dense graphs (E ~ V^2):\n");
    printf("    Dijkstra : O(V^2)\n");
    printf("    BF       : O(V * V^2) = O(V^3)\n");
    printf("    -> Dijkstra wins clearly on dense graphs\n");
}

/* ------------------------------------------------------------------ */
/* Demo: named 8-person social network                                  */
/* ------------------------------------------------------------------ */
void run_demo(void) {
    printf("\n====== Demo: 8-Person Social Network ======\n\n");
    printf("Vertices = people, edge weight = interaction distance\n");
    printf("(lower weight = closer relationship)\n\n");
    printf("  Alice(0) --2-- Bob(1) --6-- Eve(4)\n");
    printf("     |              |              |\n");
    printf("     4              1              3\n");
    printf("     |              |              |\n");
    printf("  Charlie(2) --3-- Dave(3) --1-- Frank(5)\n");
    printf("     |                               |\n");
    printf("     7                               2\n");
    printf("     |                               |\n");
    printf("  Grace(6) ----------5---------- Helen(7)\n\n");

    Graph g = create_graph(8);
    add_edge_undirected(&g, 0, 1, 2);  /* Alice  - Bob    */
    add_edge_undirected(&g, 0, 2, 4);  /* Alice  - Charlie*/
    add_edge_undirected(&g, 1, 3, 1);  /* Bob    - Dave   */
    add_edge_undirected(&g, 1, 4, 6);  /* Bob    - Eve    */
    add_edge_undirected(&g, 2, 3, 3);  /* Charlie- Dave   */
    add_edge_undirected(&g, 2, 6, 7);  /* Charlie- Grace  */
    add_edge_undirected(&g, 3, 5, 1);  /* Dave   - Frank  */
    add_edge_undirected(&g, 4, 5, 3);  /* Eve    - Frank  */
    add_edge_undirected(&g, 5, 7, 2);  /* Frank  - Helen  */
    add_edge_undirected(&g, 6, 7, 5);  /* Grace  - Helen  */

    const char *names[] = {"Alice","Bob","Charlie","Dave","Eve","Frank","Grace","Helen"};

    printf("Dijkstra shortest paths from Alice:\n");
    PathResult d = dijkstra(&g, 0);

    for (int i = 1; i < 8; i++) {
        printf("  Alice -> %-8s (dist=%2d)  ", names[i], d.dist[i]);
        /* inline path print without newline break */
        int path[8]; int len = 0; int cur = i;
        while (cur != -1) { path[len++] = cur; cur = d.prev[cur]; }
        for (int j = len - 1; j >= 0; j--) {
            printf("%s", names[path[j]]);
            if (j > 0) printf(" -> ");
        }
        printf("\n");
    }

    printf("\nBellman-Ford result matches Dijkstra: ");
    PathResult bf = bellman_ford(&g, 0);
    int agree = 1;
    for (int i = 0; i < 8; i++) if (d.dist[i] != bf.dist[i]) { agree = 0; break; }
    printf("%s\n", agree ? "YES (PASS)" : "NO (FAIL)");
    printf("Negative cycle detected: %s\n", bf.has_neg_cycle ? "YES" : "NO (expected)");
    printf("Dijkstra: %ld us  |  Bellman-Ford: %ld us\n", d.time_us, bf.time_us);

    destroy_result(&d);
    destroy_result(&bf);
    destroy_graph(&g);
}

/* ------------------------------------------------------------------ */
/* File-based test using social_network.txt                             */
/*                                                                      */
/* social_network.txt is a DIRECTED graph: both directions are listed  */
/* explicitly (e.g. "0,1,3" and "1,0,3" are separate lines).          */
/* We load it with load_graph_weighted which calls add_edge (directed) */
/* so each line becomes one directed arc – no double-counting.         */
/* ------------------------------------------------------------------ */
void run_file_test(void) {
    printf("\n====== File-based graph test (social_network.txt) ======\n\n");

    Graph g = load_graph_weighted("social_network.txt");
    if (g.V == 0) {
        printf("Skipping: social_network.txt not found.\n");
        return;
    }

    printf("Loaded directed graph: %d vertices, %d directed edges\n\n", g.V, g.E);

    PathResult d  = dijkstra(&g, 0);
    PathResult bf = bellman_ford(&g, 0);

    printf("Dijkstra from vertex 0:\n");
    print_all_distances(&d, 0);

    printf("\nSample paths from vertex 0:\n");
    for (int i = 1; i < g.V && i <= 5; i++)
        print_path(&d, 0, i);

    int agree = 1;
    for (int i = 0; i < g.V; i++) if (d.dist[i] != bf.dist[i]) { agree = 0; break; }
    printf("\nBoth algorithms agree: %s\n", agree ? "YES (PASS)" : "NO (FAIL)");
    printf("Negative cycle detected: %s\n", bf.has_neg_cycle ? "YES" : "NO");
    printf("Dijkstra: %ld us  |  Bellman-Ford: %ld us\n", d.time_us, bf.time_us);

    destroy_result(&d);
    destroy_result(&bf);
    destroy_graph(&g);
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int main(void) {
    printf("KIT205 Assignment 2 – Social Network Shortest Path\n");
    printf("Student : Samsul Alam (759423)\n");
    printf("Problem : Comparing Dijkstra vs Bellman-Ford on sparse social network graphs\n");
    printf("=======================================================\n\n");

    srand(42);

    /* Part 1: unit tests */
    graph_test();
    shortest_path_test();

    /* Demo with named social network */
    run_demo();

    /* File-based test */
    run_file_test();

    /* Part 2: performance comparison */
    run_comparison();

    printf("\n=======================================================\n");
    printf("All tests and evaluation complete.\n");
    return 0;
}
