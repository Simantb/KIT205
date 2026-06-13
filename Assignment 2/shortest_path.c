#define _POSIX_C_SOURCE 199309L
#include "shortest_path.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* High-resolution timing helper                                        */
/* clock() only gives millisecond resolution and returns 0 for small   */
/* graphs. We use clock_gettime(CLOCK_MONOTONIC) for microseconds.     */
/* ------------------------------------------------------------------ */
static long now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)ts.tv_sec * 1000000L + ts.tv_nsec / 1000L;
}

/* ------------------------------------------------------------------ */
/* Dijkstra – array-based min selection, O(V^2 + E)                    */
/*                                                                      */
/* Chosen over a heap because:                                          */
/*   - simpler to implement correctly                                   */
/*   - for dense graphs (E ~ V^2) the heap gives no asymptotic gain    */
/*   - the assignment asks for the trade-off investigation, so keeping  */
/*     both algorithms comparably simple is fair                        */
/*                                                                      */
/* NOTE: only correct for non-negative edge weights.                    */
/* ------------------------------------------------------------------ */
PathResult dijkstra(Graph *g, int source) {
    int v = g->V;
    PathResult r;
    r.V             = v;
    r.has_neg_cycle = 0;
    r.dist          = malloc(v * sizeof(int));
    r.prev          = malloc(v * sizeof(int));
    int *visited    = calloc(v, sizeof(int));

    for (int i = 0; i < v; i++) {
        r.dist[i] = INF;
        r.prev[i] = -1;
    }
    r.dist[source] = 0;

    long t_start = now_us();

    for (int count = 0; count < v; count++) {
        /* find the unvisited vertex with the smallest tentative distance */
        int u       = -1;
        int min_d   = INF;
        for (int i = 0; i < v; i++) {
            if (!visited[i] && r.dist[i] < min_d) {
                min_d = r.dist[i];
                u     = i;
            }
        }
        if (u == -1) break;   /* all remaining vertices are unreachable */
        visited[u] = 1;

        /* relax each outgoing edge from u */
        EdgeNodePtr edge = g->adj[u].head;
        while (edge != NULL) {
            int nb      = edge->to;
            int new_d   = r.dist[u] + edge->weight;
            if (!visited[nb] && r.dist[u] != INF && new_d < r.dist[nb]) {
                r.dist[nb] = new_d;
                r.prev[nb] = u;
            }
            edge = edge->next;
        }
    }

    r.time_us = now_us() - t_start;
    free(visited);
    return r;
}

/* ------------------------------------------------------------------ */
/* Bellman-Ford – relaxes all edges V-1 times, O(V * E)                */
/*                                                                      */
/* Advantages over Dijkstra:                                            */
/*   - handles negative edge weights                                    */
/*   - detects negative cycles                                          */
/*   - on sparse graphs with early termination, converges very fast     */
/*                                                                      */
/* For social networks (positive weights, sparse), the early-           */
/* termination optimisation makes BF competitive with array Dijkstra   */
/* because BF's inner loop touches only reachable edges while          */
/* Dijkstra's min-find always scans all V vertices.                    */
/* ------------------------------------------------------------------ */
PathResult bellman_ford(Graph *g, int source) {
    int v = g->V;
    PathResult r;
    r.V             = v;
    r.has_neg_cycle = 0;
    r.dist          = malloc(v * sizeof(int));
    r.prev          = malloc(v * sizeof(int));

    for (int i = 0; i < v; i++) {
        r.dist[i] = INF;
        r.prev[i] = -1;
    }
    r.dist[source] = 0;

    long t_start = now_us();

    /* V-1 relaxation passes */
    for (int iter = 0; iter < v - 1; iter++) {
        int changed = 0;
        for (int u = 0; u < v; u++) {
            if (r.dist[u] == INF) continue;   /* skip unreachable vertices */
            EdgeNodePtr edge = g->adj[u].head;
            while (edge != NULL) {
                int nb    = edge->to;
                int new_d = r.dist[u] + edge->weight;
                if (new_d < r.dist[nb]) {
                    r.dist[nb] = new_d;
                    r.prev[nb] = u;
                    changed    = 1;
                }
                edge = edge->next;
            }
        }
        if (!changed) break;   /* early termination: already converged */
    }

    /*
     * Negative-cycle detection: perform one more pass.
     * If any distance still decreases, a negative cycle is reachable
     * from the source. (Social networks have positive weights, so this
     * should never trigger – but it makes the implementation complete.)
     */
    for (int u = 0; u < v; u++) {
        if (r.dist[u] == INF) continue;
        EdgeNodePtr edge = g->adj[u].head;
        while (edge != NULL) {
            int nb    = edge->to;
            int new_d = r.dist[u] + edge->weight;
            if (new_d < r.dist[nb]) {
                r.has_neg_cycle = 1;
                break;
            }
            edge = edge->next;
        }
        if (r.has_neg_cycle) break;
    }

    r.time_us = now_us() - t_start;
    return r;
}

/* ------------------------------------------------------------------ */
/* Path reconstruction                                                  */
/* Uses a dynamically allocated stack to avoid the fixed-size buffer.  */
/* ------------------------------------------------------------------ */
void print_path(PathResult *r, int source, int dest) {
    if (r->dist[dest] == INF) {
        printf("  no path from %d to %d\n", source, dest);
        return;
    }

    /* count path length first */
    int len = 0;
    int cur = dest;
    while (cur != -1) { len++; cur = r->prev[cur]; }

    int *path = malloc(len * sizeof(int));
    cur = dest;
    for (int i = len - 1; i >= 0; i--) {
        path[i] = cur;
        cur = r->prev[cur];
    }

    printf("  %d -> %d: distance = %d, path: ", source, dest, r->dist[dest]);
    for (int i = 0; i < len; i++) {
        printf("%d", path[i]);
        if (i < len - 1) printf(" -> ");
    }
    printf("\n");
    free(path);
}

void print_all_distances(PathResult *r, int source) {
    printf("  distances from vertex %d:\n", source);
    for (int i = 0; i < r->V; i++) {
        if (r->dist[i] == INF)
            printf("    to %d: unreachable\n", i);
        else
            printf("    to %d: %d\n", i, r->dist[i]);
    }
}

void destroy_result(PathResult *r) {
    free(r->dist);
    free(r->prev);
    r->dist = NULL;
    r->prev = NULL;
}

/* ------------------------------------------------------------------ */
/* Unit tests for shortest-path algorithms                              */
/* ------------------------------------------------------------------ */
void shortest_path_test(void) {
    printf("\n=== Shortest path algorithm tests ===\n\n");

    /*
     * Test graph (undirected, positive weights):
     *
     *   0 ---4--- 1
     *   |         |
     *   1         5
     *   |         |
     *   2 ---2--- 1
     *             |
     *             3--- 4
     *
     * Expected distances from 0:
     *   0->0 = 0
     *   0->1 = 3  (via 2: 0-2-1, cost 1+2=3)
     *   0->2 = 1
     *   0->3 = 8  (0-2-1-3, cost 1+2+5=8)
     *   0->4 = 11 (0-2-1-3-4, cost 1+2+5+3=11)
     */
    Graph g = create_graph(5);
    add_edge_undirected(&g, 0, 1, 4);
    add_edge_undirected(&g, 0, 2, 1);
    add_edge_undirected(&g, 2, 1, 2);
    add_edge_undirected(&g, 1, 3, 5);
    add_edge_undirected(&g, 3, 4, 3);

    /* --- test: Dijkstra correctness --- */
    printf("[Test 1] Dijkstra from vertex 0:\n");
    PathResult d = dijkstra(&g, 0);
    print_all_distances(&d, 0);
    printf("  Expected: 0=0, 1=3, 2=1, 3=8, 4=11\n");
    int d_ok = (d.dist[0]==0 && d.dist[1]==3 && d.dist[2]==1
                && d.dist[3]==8 && d.dist[4]==11);
    printf("  Dijkstra correctness: %s\n", d_ok ? "PASS" : "FAIL");

    /* --- test: Bellman-Ford correctness --- */
    printf("\n[Test 2] Bellman-Ford from vertex 0:\n");
    PathResult bf = bellman_ford(&g, 0);
    print_all_distances(&bf, 0);
    int bf_ok = (bf.dist[0]==0 && bf.dist[1]==3 && bf.dist[2]==1
                 && bf.dist[3]==8 && bf.dist[4]==11);
    printf("  Bellman-Ford correctness: %s\n", bf_ok ? "PASS" : "FAIL");
    printf("  Negative cycle detected: %s (expected NO)\n",
           bf.has_neg_cycle ? "YES – FAIL" : "NO – PASS");

    /* --- test: both agree --- */
    int agree = 1;
    for (int i = 0; i < g.V; i++)
        if (d.dist[i] != bf.dist[i]) { agree = 0; break; }
    printf("\n[Test 3] Both algorithms produce identical distances: %s\n",
           agree ? "PASS" : "FAIL");

    /* --- test: path reconstruction --- */
    printf("\n[Test 4] Path reconstruction (Dijkstra, 0->4):\n");
    print_path(&d, 0, 4);
    printf("  Expected: 0 -> 2 -> 1 -> 3 -> 4\n");

    /* --- test: unreachable vertex --- */
    printf("\n[Test 5] Unreachable vertex:\n");
    Graph g2 = create_graph(4);
    add_edge_undirected(&g2, 0, 1, 1);
    add_edge_undirected(&g2, 2, 3, 1);   /* disconnected component */
    PathResult d2 = dijkstra(&g2, 0);
    printf("  dist[0->2] = ");
    if (d2.dist[2] == INF) printf("unreachable: PASS\n");
    else printf("%d: FAIL\n", d2.dist[2]);
    destroy_result(&d2);
    destroy_graph(&g2);

    /* --- test: single-vertex graph --- */
    printf("\n[Test 6] Single-vertex graph:\n");
    Graph g3 = create_graph(1);
    PathResult d3 = dijkstra(&g3, 0);
    printf("  dist[0->0] = %d (expected 0): %s\n",
           d3.dist[0], d3.dist[0] == 0 ? "PASS" : "FAIL");
    destroy_result(&d3);
    destroy_graph(&g3);

    destroy_result(&d);
    destroy_result(&bf);
    destroy_graph(&g);

    printf("\nShortest path tests complete.\n");
}
