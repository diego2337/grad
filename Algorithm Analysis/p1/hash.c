/* 
	Program evaluation of paper implementation of hash.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 30/04/2017	
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct _tHash{
	int M;
	int *contains;
	int *table;
}tHash;

typedef tHash *Hash;

void init(Hash *h, int M)
{
	(*h) = (tHash*)malloc(sizeof(tHash)*1);
	(*h)->contains = (int*)calloc(M, sizeof(int));
	(*h)->table = (int*)malloc(sizeof(int)*1);
	(*h)->M = M;
}

void put(Hash *h, int element, int index)
{
	if((*h)->contains[index%(*h)->M] != 0)
	{
		put(h, element, element+1);
	}
	else
	{
		(*h)->table[index%(*h)->M] = element;
		(*h)->contains[index%(*h)->M] = 1;
	}
}

int main(void)
{
	Hash h;
	init(&h, 10);
	put(&h, 10, 10);
	put(&h, 192, 192);
	put(&h, 5, 5);
}