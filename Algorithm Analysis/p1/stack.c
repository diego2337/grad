/* 
	Program evaluation of paper implementation of stack. 
	Author: Diego Cintra
	USP Number: 10094043
	Date: 30/04/2017	
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct _tStack{
	int data;
	struct _tStack *next;
}tStack;

typedef tStack* Stack;

void insertStack(Stack *s, int element)
{
	tStack *top = (tStack*)malloc(sizeof(tStack)*1);
	top->data = element;
	if(!(*s))
	{
		top->next = NULL;
		(*s) = top;
	}
	else
	{
		top->next = (*s);
		(*s) = top;
	}
}

void popStack(Stack *s)
{
	Stack top = NULL;
	if((*s) != NULL)
	{
		top = (*s)->next;
		printf("%d\n", (*s)->data);
		(*s) = top;
	}
	else
	{
		printf("pilha vazia!\n");
	}
}

int main(void)
{
	Stack s;
	s = NULL;
	insertStack(&s, 2);
	insertStack(&s, 2);
	insertStack(&s, 2);
	insertStack(&s, 2);
	insertStack(&s, 2);
	popStack(&s);
	popStack(&s);
	return 0;
}