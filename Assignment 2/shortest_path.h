#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "graph.h"

/* Stores the result of a single-source shortest path computation */
typedef struct pathResult {
    int  *dist;       /* dist[i] = shortest distance from source to i  */
    int  *prev;       /* prev[i] = predecessor of i on shortest path    */
    int   V;          /* number of vertices                             */
    long  time_us;    /* wall-clock microseconds the algorithm took     */
    int   has_neg_cycle; /* 1 if Bellman-Ford detected a negative cycle */
} PathResult;

PathResult dijkstra(Graph *g, int source);
PathResult bellman_ford(Graph *g, int source);
void       print_path(PathResult *r, int source, int dest);
void       print_all_distances(PathResult *r, int source);
void       destroy_result(PathResult *r);
void       shortest_path_test(void);

#endif
