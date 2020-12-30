#include <stdio.h>
#include <stdlib.h>
#include <graph.h>

GRAPH *graph_create(BOOL directed) {

	GRAPH *graph = (GRAPH *)
		calloc(1, sizeof(GRAPH));
	graph->directed = directed;

	return graph;
}

int graph_insert_vertex(GRAPH *graph) {
	
	graph->nvertices++;
	graph->edges = (EDGE **)
		realloc(graph->edges,
			sizeof(EDGE *) * 
			graph->nvertices);
	graph->edges[graph->nvertices-1] = NULL;
	graph->degree =
		(int *) realloc(graph->degree, 
				sizeof(int)*graph->nvertices);
	graph->degree[graph->nvertices-1] = 0;

	return graph->nvertices-1;
}

STATUS graph_contains_edge(GRAPH *graph, int x, int y) {

	if (x < 0 || x >= graph->nvertices) return STATUS_FAILED;
	if (y < 0 || y >= graph->nvertices) return STATUS_FAILED;

	EDGE *pointer = graph->edges[x];
	while (pointer != NULL) {
		if (pointer->y == y) return STATUS_TRUE;
		pointer = pointer->next;
	}
	return STATUS_FALSE;
}

void graph_insert_edge(GRAPH *graph, 
		int x, int y, 
		double weight) {
	STATUS status;

	status = graph_contains_edge(graph, x, y);
	if (status == STATUS_FALSE) {
		EDGE *myEdge = (EDGE *) malloc(sizeof(EDGE));
		myEdge->y = y;
		myEdge->weight = weight;
		myEdge->next = graph->edges[x];
		graph->edges[x] = myEdge;
		graph->nedges++;
		graph->degree[x]++;

		if (!graph->directed) 
		    graph_insert_edge(graph, y, x, weight);
	}
}

GRAPH *graph_read_from_file(FILE *stream) {
	BOOL directed;
	int i, x, y, nvertices, nedges;
	double w;

	if (!fscanf(stream, "%d", &directed)) return NULL;
	GRAPH *graph = graph_create(directed);
	if (!fscanf(stream, "%d", &nvertices) ||
		!fscanf(stream, "%d", &nedges)) {
		free(graph);
		return NULL;
	}

	for (i = 0; i < nvertices; i++) {
		graph_insert_vertex(graph);
	}

	while (!feof(stream)) {
		fscanf(stream, "%d %d %lf", &x, &y, &w);
		if  (!feof(stream))
			graph_insert_edge(graph, x, y, w);
	}

	return graph;
}

void graph_free(GRAPH *graph) {
	int i;
	EDGE *p, *aux;
	
	free(graph->degree);

	for (i = 0; i < graph->nvertices; i++) {
		p = graph->edges[i];
		while (p != NULL) {
			aux = p->next;
			free(p);
			p = aux;
		}
	}

	free(graph->edges);
	free(graph);
}

void graph_print(GRAPH *graph) {
	int i;

	printf("========== GRAPH INFORMATION ===========\n");
	printf("directed? %d\n", graph->directed);
	printf("number of vertices: %d\n", graph->nvertices);
	printf("number of edges: %d\n", graph->nedges);

	printf("degree information:\n");
	for (i = 0; i < graph->nvertices; i++) {
		printf("\tvertex: %d\tdegree: %d\n", i, graph->degree[i]);
	}

	printf("edges information:\n");
	for (i = 0; i < graph->nvertices; i++) {
		EDGE *p = graph->edges[i];
		while (p != NULL) {
			printf("\tFrom: %d\tTo:\t%d\tWeight:%lf\n",
					i, p->y, p->weight);
			p = p->next;
		}
	}
}














