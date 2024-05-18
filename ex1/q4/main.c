// C program for Dijkstra's single source shortest path
// algorithm. The program is for adjacency matrix
// representation of the graph
// FROM
// https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void free_graph(int **graph, int V) {
    for (int i = 0; i < V; i++) {
        free(graph[i]);
    }
    free(graph);
}

// A utility function to find the vertex with minimum
// distance value, from the set of vertices not yet included
// in shortest path tree
int minDistance(int dist[], bool sptSet[], int V) {
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min) min = dist[v], min_index = v;

    return min_index;
}

// A utility function to print the constructed distance
// array
void printSolution(int dist[], int V) {
    printf("Vertex \t\t Distance from Source\n");
    for (int i = 0; i < V; i++) printf("%d \t\t\t\t %d\n", i, dist[i]);
}

// Function that implements Dijkstra's single source
// shortest path algorithm for a graph represented using
// adjacency matrix representation
void dijkstra(int **graph, int src, int V) {
    int dist[V];  // The output array. dist[i] will hold the
                  // shortest
    // distance from src to i

    bool sptSet[V];  // sptSet[i] will be true if vertex i is
                     // included in shortest
    // path tree or shortest distance from src to i is
    // finalized

    // Initialize all distances as INFINITE and stpSet[] as
    // false
    for (int i = 0; i < V; i++) dist[i] = INT_MAX, sptSet[i] = false;

    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < V - 1; count++) {
        // Pick the minimum distance vertex from the set of
        // vertices not yet processed. u is always equal to
        // src in the first iteration.
        int u = minDistance(dist, sptSet, V);

        // Mark the picked vertex as processed
        sptSet[u] = true;

        // Update dist value of the adjacent vertices of the
        // picked vertex.
        for (int v = 0; v < V; v++)

            // Update dist[v] only if is not in sptSet,
            // there is an edge from u to v, and total
            // weight of path from src to v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX &&
                dist[u] + graph[u][v] < dist[v])
                dist[v] = dist[u] + graph[u][v];
    }

    // print the constructed distance array
    printSolution(dist, V);
}

// driver's code
int main() {
    // get the number of vertices
    int V;
    if (scanf("%d", &V) != 1) {
        printf("Error while reading the number of vertices\n");
        printf("%d\n", V);
        return 1;
    }
    if (V <= 0) {
        printf("Error: number of vertices should be a positive integer\n");
        return 1;
    }

    // allocate memory for the graph
    int **graph = (int **)malloc(V * sizeof(int *));
    if (graph == NULL) {
        printf("Error while allocating memory for the graph\n");
        return 1;
    }
    for (int i = 0; i < V; i++) {
        graph[i] = (int *)malloc(V * sizeof(int));
        if (graph[i] == NULL) {
            printf("Error while allocating memory for the vertices of the graph\n");
            // free the memory allocated so far
            for (int j = 0; j < i; j++) {
                free(graph[j]);
            }
            free(graph);
            return 1;
        }
    }

    // get the graph
    int input;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (scanf("%d", &input) != 1) {
                printf("Error while reading the weight of the edge between %d and %d\n", i, j);
                free_graph(graph, V);
                return 1;
            }
            if (input < 0) {
                printf("Error: input should be a non-negative integer\n");
                free_graph(graph, V);
                return 1;
            }

            if (i == j && input != 0) {
                printf("Error: the diagonal of the matrix should be 0\n");
                free_graph(graph, V);
                return 1;
            }
            graph[i][j] = input;
        }
        // check if there is more input that its not \n
        char t2;
        if (scanf("%c", &t2) == 1 && t2 != '\n') {
            printf("Error: too many inputs in the row %d\n", i);
            free_graph(graph, V);
            return 1;
        }
    }

    int src;
    if (scanf("%d", &src) != 1) {
        printf("Error while reading source vertex\n");
        free_graph(graph, V);
        return 1;
    }

    if (src < 0 || src >= V) {
        printf("Error: source vertex should be a non-negative integer and less than the number of vertices\n");
        free_graph(graph, V);
        return 1;
    }

    // Function call
    dijkstra(graph, src, V);

    free_graph(graph, V);
    return 0;
}
