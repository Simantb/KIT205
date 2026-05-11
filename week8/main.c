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

int main(void) {
    Graph G = {0, NULL};
    FILE *file = fopen("graph.txt", "r");

    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    int vertices;
    if (fscanf(file, "%d", &vertices) != 1) {
        fprintf(stderr, "Error reading number of vertices.\n");
        fclose(file);
        return 1;
    }

    init_graph(&G, vertices);

    int from;
    int to;
    int weight;

    while (fscanf(file, "%d,%d,%d", &from, &to, &weight) == 3) {
        if (from < 0 || from >= G.V || to < 0 || to >= G.V) {
            fprintf(stderr, "Invalid edge: %d -> %d\n", from, to);
            fclose(file);
            free_graph(&G);
            return 1;
        }

        add_edge(&G, from, to, weight);
    }

    fclose(file);

    int *in_degree = calloc((size_t)G.V, sizeof(int));
    if (in_degree == NULL) {
        fprintf(stderr, "Memory allocation failed for in-degree array.\n");
        free_graph(&G);
        return 1;
    }

    for (int v = 0; v < G.V; v++) {
        EdgeNodePtr current = G.edges[v].head;

        while (current != NULL) {
            in_degree[current->edge.to_vertex]++;
            current = current->next;
        }
    }

    printf("In-degrees:\n");
    for (int v = 0; v < G.V; v++) {
        printf("Vertex %d: %d\n", v, in_degree[v]);
    }

    free(in_degree);
    free_graph(&G);
    return 0;
}
