#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INF INT_MAX

typedef struct edgeNode {
    int to;
    int weight;
    struct edgeNode *next;
} *EdgeNodePtr;

typedef struct edgeList {
    EdgeNodePtr head;
} EdgeList;

typedef struct graph {
    int V;          /* number of vertices */
    int E;          /* number of undirected edges */
    EdgeList *adj;
} Graph;

Graph  create_graph(int v);
void   add_edge(Graph *g, int from, int to, int weight);
void   add_edge_undirected(Graph *g, int u, int v, int weight);
void   print_graph(Graph *g);
void   destroy_graph(Graph *g);
int    out_degree(Graph *g, int v);
Graph  load_graph_weighted(const char *filename);
Graph  load_graph_unweighted(const char *filename);
Graph  generate_random_graph(int v, int e, int max_weight);
void   graph_test(void);

#endif
