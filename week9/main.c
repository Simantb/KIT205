#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

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

    while (fscanf(file, "%d,%d", &from, &to) == 2) {
        if (from < 0 || from >= G.V || to < 0 || to >= G.V) {
            fprintf(stderr, "Invalid edge: %d -> %d\n", from, to);
            fclose(file);
            free_graph(&G);
            return 1;
        }

        add_edge(&G, from, to, 0);
    }

    fclose(file);

    const int iterations = 20;
    double *page_ranks = page_rank(&G, iterations);

    printf("PageRank after %d iterations:\n", iterations);
    for (int v = 0; v < G.V; v++) {
        printf("Vertex %d: %.6f\n", v, page_ranks[v]);
    }

    free(page_ranks);
    free_graph(&G);
    return 0;
}
