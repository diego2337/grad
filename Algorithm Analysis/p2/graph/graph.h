#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <utils.h>

struct edge {
	int y;
	double weight;
	struct edge *next;
};

typedef struct edge EDGE;

struct graph {
	EDGE **edges;
	int *degree;
	int nvertices;
	int nedges;
	BOOL directed;
};

typedef struct graph GRAPH;

GRAPH *graph_create(BOOL);
int graph_insert_vertex(GRAPH *);
STATUS graph_contains_edge(GRAPH *, int, int);
void graph_insert_edge(GRAPH *, int, int, double);
GRAPH *graph_read_from_file(FILE *);
void graph_free(GRAPH *);
void graph_print(GRAPH *);

#endif
