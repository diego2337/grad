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

void bfs(GRAPH *g, int initialVertex)
{
	queue *q = NULL;/*(queue*)calloc(1, sizeof(queue));*/
	EDGE *aux = NULL;
	queue *aux2 = NULL;
	/* Enqueue neighbors vertexes */
	/*aux = g->edges[initialVertex];
	g->visited[initialVertex] = 1;
	while(aux)
	{
		enqueue(&q, aux->y);
		aux = aux->next;
	}*/
	enqueue(&q, initialVertex);
	while(q)
	{
		aux2 = dequeue(&q);
		printf("Vertex found: %d\n", aux2->value);
		g->visited[aux2->value] = 1;
		aux = g->edges[aux2->value];
		while(aux)
		{
			if(g->visited[aux->y] == 0)
			{
				printf("Enqueueing %d\n", aux->y);
				enqueue(&q, aux->y);
				g->visited[aux->y] = 1;
			}
			aux = aux->next;
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
	bfs(graph, 0);
	graph_free(graph);

	return 0;
}
