#include <stdio.h>
#include <stdlib.h>
#include <graph/graph.h>

void prim(GRAPH *g, int initialVertex)
{
	EDGE *aux;
	int maxIndex = 0, currentNode;
	double maxWeight = 1000.00;
	currentNode = initialVertex;
	printf("%d\n", currentNode);
	while(maxIndex != -1)
	{
		g->visited[currentNode] = 1;
		maxWeight = 1000.00;
		maxIndex = -1;
		aux = g->edges[currentNode];
		while(aux)
		{
			if(aux->weight < maxWeight && g->visited[aux->y] == 0)
			{
				maxIndex = aux->y;
				maxWeight = aux->weight;
			}
			aux = aux->next;
		}
		if(maxIndex != -1)
		{
			printf("%d\n", maxIndex);
			currentNode = maxIndex;
		}
	}
}

enum {
	PROGNAME,
	FILENAME,
	NARGS
};

int main(int argc, char *argv[])
{
	if (argc != NARGS) {
		printf("usage: %s graph.info\n", argv[PROGNAME]);
		exit(0);
	}

	FILE *stream = fopen(argv[FILENAME], "r");
	if (!stream) {
		printf("File not found...\n");
		exit(0);
	}

	GRAPH *graph = graph_read_from_file(stream);
	graph_print(graph);
	prim(graph, 0);
	graph_free(graph);
	return 0;
}
