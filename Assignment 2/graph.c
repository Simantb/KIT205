#include "graph.h"
#include <string.h>

Graph create_graph(int v) {
    Graph g;
    g.V = v;
    g.E = 0;
    g.adj = malloc(v * sizeof(EdgeList));
    for (int i = 0; i < v; i++) g.adj[i].head = NULL;
    return g;
}

/* add a single directed arc; does NOT increment E (E tracks undirected edges) */
static void add_arc(Graph *g, int from, int to, int weight) {
    EdgeNodePtr n = malloc(sizeof *n);
    n->to     = to;
    n->weight = weight;
    n->next   = g->adj[from].head;
    g->adj[from].head = n;
}

/* directed edge (use for directed graphs / file loading) */
void add_edge(Graph *g, int from, int to, int weight) {
    add_arc(g, from, to, weight);
    g->E++;
}

/* undirected edge - adds both arcs but counts as ONE edge */
void add_edge_undirected(Graph *g, int u, int v, int weight) {
    add_arc(g, u, v, weight);
    add_arc(g, v, u, weight);
    g->E++;   /* count once for the undirected edge */
}

void print_graph(Graph *g) {
    for (int i = 0; i < g->V; i++) {
        printf("  %d -> ", i);
        EdgeNodePtr c = g->adj[i].head;
        while (c) {
            printf("(%d,w=%d) ", c->to, c->weight);
            c = c->next;
        }
        printf("\n");
    }
}

int out_degree(Graph *g, int v) {
    int count = 0;
    EdgeNodePtr c = g->adj[v].head;
    while (c) { count++; c = c->next; }
    return count;
}

void destroy_graph(Graph *g) {
    for (int i = 0; i < g->V; i++) {
        EdgeNodePtr c = g->adj[i].head;
        while (c) {
            EdgeNodePtr tmp = c;
            c = c->next;
            free(tmp);
        }
    }
    free(g->adj);
    g->V = 0;
    g->E = 0;
}

/*
 * Load a DIRECTED weighted graph.
 * File format: first line = V, then lines of  from,to,weight
 *
 * social_network.txt already lists both directions explicitly
 * (e.g. "0,1,3" AND "1,0,3") so we load as directed arcs.
 * E will count each line as one directed edge.
 */
Graph load_graph_weighted(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("cannot open %s\n", filename);
        Graph e = {0, 0, NULL};
        return e;
    }
    int v = 0;
    if (fscanf(f, "%d", &v) != 1 || v <= 0) {
        fclose(f); Graph e = {0,0,NULL}; return e;
    }
    Graph g = create_graph(v);
    int from, to, w;
    while (fscanf(f, "%d,%d,%d", &from, &to, &w) == 3)
        add_edge(&g, from, to, w);   /* directed */
    fclose(f);
    return g;
}

/*
 * Load an UNDIRECTED unweighted graph.
 * File format: first line = V, then lines of  from,to
 * Each line represents one undirected edge; weight = 1.
 */
Graph load_graph_unweighted(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("cannot open %s\n", filename);
        Graph e = {0, 0, NULL};
        return e;
    }
    int v = 0;
    if (fscanf(f, "%d", &v) != 1 || v <= 0) {
        fclose(f); Graph e = {0,0,NULL}; return e;
    }
    Graph g = create_graph(v);
    int from, to;
    while (fscanf(f, "%d,%d", &from, &to) == 2)
        add_edge_undirected(&g, from, to, 1);
    fclose(f);
    return g;
}

/*
 * Generate a random connected undirected graph for performance testing.
 * Guarantees connectivity by first creating a spanning chain,
 * then adding extra random edges up to the requested total.
 * e must be >= v-1.
 */
Graph generate_random_graph(int v, int e, int max_weight) {
    if (e < v - 1) e = v - 1;   /* need at least a spanning tree */
    Graph g = create_graph(v);

    /* spanning chain ensures connectivity */
    for (int i = 0; i < v - 1; i++) {
        int w = 1 + rand() % max_weight;
        add_edge_undirected(&g, i, i + 1, w);
    }

    /* add remaining random undirected edges */
    int added = v - 1;
    int attempts = 0;
    while (added < e && attempts < e * 10) {
        int u = rand() % v;
        int t = rand() % v;
        if (u != t) {
            int w = 1 + rand() % max_weight;
            add_edge_undirected(&g, u, t, w);
            added++;
        }
        attempts++;
    }
    return g;
}

/* ------------------------------------------------------------------ */
/* Unit tests for the graph data structure                              */
/* ------------------------------------------------------------------ */
void graph_test(void) {
    printf("=== Graph data structure tests ===\n\n");

    /* --- test 1: basic construction and edge counting --- */
    Graph g = create_graph(5);
    add_edge_undirected(&g, 0, 1, 4);
    add_edge_undirected(&g, 0, 2, 1);
    add_edge_undirected(&g, 1, 3, 1);
    add_edge_undirected(&g, 2, 1, 2);
    add_edge_undirected(&g, 2, 3, 5);
    add_edge_undirected(&g, 3, 4, 3);

    printf("[Test 1] 5-vertex undirected graph, 6 edges:\n");
    print_graph(&g);
    printf("  V=%d (expected 5): %s\n", g.V, g.V == 5 ? "PASS" : "FAIL");
    printf("  E=%d (expected 6): %s\n", g.E, g.E == 6 ? "PASS" : "FAIL");

    /* --- test 2: degree check --- */
    printf("\n[Test 2] Out-degrees (adjacency arcs):\n");
    int expected_deg[] = {2, 3, 3, 3, 1};
    int deg_pass = 1;
    for (int i = 0; i < g.V; i++) {
        int d = out_degree(&g, i);
        printf("  vertex %d: degree %d (expected %d): %s\n",
               i, d, expected_deg[i], d == expected_deg[i] ? "PASS" : "FAIL");
        if (d != expected_deg[i]) deg_pass = 0;
    }
    printf("  Degree tests: %s\n", deg_pass ? "PASS" : "FAIL");

    /* --- test 3: directed edge counting --- */
    printf("\n[Test 3] Directed edge (add_edge):\n");
    Graph dg = create_graph(3);
    add_edge(&dg, 0, 1, 5);
    add_edge(&dg, 0, 2, 3);
    printf("  E=%d (expected 2): %s\n", dg.E, dg.E == 2 ? "PASS" : "FAIL");
    printf("  degree of 0 = %d (expected 2): %s\n",
           out_degree(&dg, 0), out_degree(&dg, 0) == 2 ? "PASS" : "FAIL");
    printf("  degree of 1 = %d (expected 0, directed): %s\n",
           out_degree(&dg, 1), out_degree(&dg, 1) == 0 ? "PASS" : "FAIL");
    destroy_graph(&dg);

    /* --- test 4: random graph connectivity --- */
    printf("\n[Test 4] Random graph generation (V=10, E=15):\n");
    Graph rg = generate_random_graph(10, 15, 10);
    printf("  V=%d (expected 10): %s\n", rg.V, rg.V == 10 ? "PASS" : "FAIL");
    printf("  E=%d (expected >=9): %s\n", rg.E, rg.E >= 9 ? "PASS" : "FAIL");
    destroy_graph(&rg);

    destroy_graph(&g);
    printf("\nGraph data structure tests complete.\n");
}
