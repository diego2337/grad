/* 
	Program evaluation of paper implementation of linked list. 
	Author: Diego Cintra
	USP Number: 10094043
	Date: 30/04/2017	
*/
#include <stdio.h>
#include <stdlib.h>

/* Linked list structure */
typedef struct _tList{
	int data;
	struct _tList *next;
}tList;

typedef tList* List;

/* Insertion */
void insert(int element, List *lList)
{
	tList *head = (tList*)malloc(sizeof(tList)*1);
	if((*lList) == NULL)
	{
		head->data = element;
		head->next = NULL;
		(*lList) = head;
	}
	else
	{
		head->data = element;
		head->next = (*lList);
		(*lList) = head;
	}
}

/* Remove */
int listRemove(int element, List *lList)
{
	tList *r = (tList*)malloc(sizeof(tList)*1);
	r = (*lList);
	while(r->next)
	{
		if(r->next->data == element)
		{
			r->next = r->next->next;
			free(r);
			return 1;
		}
		r = r->next;
	}
	r = (*lList);
	if(r->data == element)
	{
		r->next = NULL;
		free(r);
		return 1;
	}
	free(r);
	return -1;
}

/* Search */
int listSearch(int element, List *lList)
{
	tList *r = (tList*)malloc(sizeof(tList)*1);
	r = (*lList);
	while(r->next)
	{
		if(r->data == element)
		{
			return r->data;
		}
		r = r->next;
	}
	r = (*lList);
	if(r->data == element)
	{
		return r->data;
	}
	free(r);
	return -1;
}

int main(void)
{
	List lList;
	lList = NULL;
	insert(3, &lList);
	insert(5, &lList);
	printf("%d\n", listRemove(5, &lList));

	free(lList);
	return 0;
}
