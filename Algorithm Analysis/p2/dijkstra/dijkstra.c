#include <stdio.h>
#include <stdlib.h>
#include <graph/graph.h>

typedef struct _queue{
	int value;
	struct _queue *next;
}queue;

void enqueue(queue **q, int value)
{
	queue *aux = (queue*)calloc(1, sizeof(queue));
	aux->value = value;
	aux->next = NULL;
	if(!(*q))
	{
		(*q) = aux;
	}
	else
	{
		aux->next = (*q);
		(*q) = aux;
	}
}

queue *dequeue(queue **q)
{
	queue *aux = NULL, *aux2 = NULL;
	if(!(*q))
	{
		printf("Empty queue\n");
		return NULL;
	}
	else if(!(*q)->next)
	{
		aux2 = (*q);
		(*q) = NULL;
		return aux2;
	}
	else
	{
		aux = (*q);
		while(aux->next->next)
		{
			aux = aux->next;
		}
		aux2 = aux->next;
		aux->next = NULL;
		return aux2;
	}
}

int isEmpty(queue *q)
{
	if(!q) return 1;
	return 0;
}

void dijkstra(GRAPH *g, int initialVertex)
{
	int currentNode;
	EDGE *aux;
	queue *q = NULL, *aux2 = NULL;
	currentNode = initialVertex;
	g->distances[initialVertex] = 0;
	printf("%d\n", currentNode);
	enqueue(&q, currentNode);
	while(!isEmpty(q))
	{
		aux2 = dequeue(&q);
		printf("%d\n", aux2->value);
		aux = g->edges[aux2->value];
		while(aux)
		{
			if(aux->weight + g->distances[aux2->value] < g->distances[aux->y])
			{
				g->distances[aux->y] = aux->weight + g->distances[aux2->value];
			}
			if(g->visited[aux->y] != 1)
			{
				enqueue(&q, aux->y);
			}
			aux = aux->next;
		}
		g->visited[aux2->value] = 1;		
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

	int i;
	GRAPH *graph = graph_read_from_file(stream);
	graph_print(graph);
	printf("antes\n");
	for(i = 0; i < graph->nvertices; i++)
	{
		printf("%d ", graph->distances[i]);
	}
	printf("\n");
	dijkstra(graph, 0);
	printf("depois\n");
	for(i = 0; i < graph->nvertices; i++)
	{
		printf("%d ", graph->distances[i]);
	}
	printf("\n");
	graph_free(graph);
	return 0;
}
