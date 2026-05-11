#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

void init_graph(Graph *self, int vertices) {
    self->V = vertices;
    self->edges = malloc((size_t)vertices * sizeof(EdgeList));

    if (self->edges == NULL) {
        fprintf(stderr, "Memory allocation failed for graph edges.\n");
        exit(EXIT_FAILURE);
    }

    for (int v = 0; v < vertices; v++) {
        self->edges[v].head = NULL;
    }
}

void add_edge(Graph *self, int from, int to, int w) {
    EdgeNodePtr new_node = malloc(sizeof(*new_node));

    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for new edge.\n");
        free_graph(self);
        exit(EXIT_FAILURE);
    }

    new_node->edge.to_vertex = to;
    new_node->edge.weight = w;
    new_node->next = self->edges[from].head;
    self->edges[from].head = new_node;
}

void free_graph(Graph *self) {
    if (self->edges == NULL) {
        return;
    }

    for (int v = 0; v < self->V; v++) {
        EdgeNodePtr current = self->edges[v].head;

        while (current != NULL) {
            EdgeNodePtr next = current->next;
            free(current);
            current = next;
        }
    }

    free(self->edges);
    self->edges = NULL;
    self->V = 0;
}

double *page_rank(const Graph *self, int iterations) {
    const double damping = 0.85;
    double *page_ranks = malloc((size_t)self->V * sizeof(double));
    double *sums = malloc((size_t)self->V * sizeof(double));
    int *out_degrees = calloc((size_t)self->V, sizeof(int));

    if (page_ranks == NULL || sums == NULL || out_degrees == NULL) {
        fprintf(stderr, "Memory allocation failed for PageRank arrays.\n");
        free(page_ranks);
        free(sums);
        free(out_degrees);
        exit(EXIT_FAILURE);
    }

    for (int v = 0; v < self->V; v++) {
        page_ranks[v] = 1.0;

        for (EdgeNodePtr current = self->edges[v].head; current != NULL; current = current->next) {
            out_degrees[v]++;
        }
    }

    for (int iteration = 0; iteration < iterations; iteration++) {
        for (int v = 0; v < self->V; v++) {
            sums[v] = 0.0;
        }

        for (int from = 0; from < self->V; from++) {
            if (out_degrees[from] == 0) {
                continue;
            }

            for (EdgeNodePtr current = self->edges[from].head; current != NULL; current = current->next) {
                int to = current->edge.to_vertex;
                sums[to] += page_ranks[from] / out_degrees[from];
            }
        }

        for (int v = 0; v < self->V; v++) {
            page_ranks[v] = (1.0 - damping) + damping * sums[v];
        }
    }

    free(sums);
    free(out_degrees);
    return page_ranks;
}
