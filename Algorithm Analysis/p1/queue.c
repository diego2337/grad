/* 
	Program evaluation of paper implementation of queue. 
	Author: Diego Cintra
	USP Number: 10094043
	Date: 30/04/2017	
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct _tQueue{
	int data;
	struct _tQueue *next;
}tQueue;
typedef tQueue *Queue;

void enqueue(Queue *q, int element)
{
	tQueue *last = (tQueue*)malloc(sizeof(tQueue));
	last->data = element;
	if(!(*q))
	{
		last->next = NULL;
		(*q) = last;
	}
	else
	{
		last->next = (*q);
		(*q) = last;
	}
}

void dequeue(Queue *q)
{
	tQueue *first = (tQueue*)malloc(sizeof(tQueue));
	if(!(*q))
	{
		printf("Fila vazia!\n");
	}
	else
	{
		first = (*q);
		if(!(first->next))
		{
			printf("%d\n", (*q)->data);
			*q = NULL;
		}
		else
		{
			while(first->next->next)
			{
				first = first->next;
			}
			printf("%d\n", first->next->data);
			first->next = NULL;
		}
	}
}

int main(void)
{
	Queue q;
	q = NULL;
	enqueue(&q, 1);
	enqueue(&q, 2);
	enqueue(&q, 3);
	enqueue(&q, 10);
	dequeue(&q);
	dequeue(&q);
	dequeue(&q);
	dequeue(&q);
	dequeue(&q);	
	return 0;
}