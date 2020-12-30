#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEFT(i) 2*i+1
#define RIGHT(i) 2*i+2
#define PARENT(i) i/2

void swap(int **vec, int i, int j)
{
	int aux = (*vec)[i];
	(*vec)[i] = (*vec)[j];
	(*vec)[j] = aux;
}

int *createVector(int size)
{
	int i, *vec = (int*)calloc(size, sizeof(int));
	srand(time(NULL));
	for(i = 0; i < size; i++)
	{
		vec[i] = (rand() % 100) + 0;
	}
	return vec;
}

void maxHeapify(int *vec, int index, int size)
{
	int largestId, leftId, rightId;
	largestId = index;
	leftId = LEFT(largestId);
	rightId = RIGHT(largestId);
	if(leftId < size && vec[leftId] > vec[largestId]) largestId = leftId;
	if(rightId < size && vec[rightId] > vec[largestId]) largestId = rightId;
	if(largestId != index)
	{
		swap(&vec, index, largestId);
		maxHeapify(vec, largestId, size);
	}
}

void buildMaxHeap(int *vec, int size)
{
	int i;
	for(i = size/2; i >= 0; i--)
	{
		maxHeapify(vec, i, size);
	}
}

void decreaseKey(int *vec, int size, int index, int key)
{
	printf("vec[index]: %d\n", vec[index]);
	vec[index] = key;
	maxHeapify(vec, index, size);
}

void increaseKey(int *vec, int size, int index, int key)
{
	int parent;
	printf("vec[index]: %d\n", vec[index]);
	vec[index] = key;
	parent = PARENT(index);
	if(index % 2 == 0) parent = parent - 1;
	while(parent != index)
	{
		if(vec[parent] < vec[index])
		{
			swap(&vec, parent, index);
		}
		index = parent;
		if(parent % 2 == 0) parent = parent - 1;
		parent = PARENT(parent);
	}
}

void heapsort(int *vec, int size)
{
	int i;
	for(i = size-1; i >= 0; i--)
	{
		swap(&vec, 0, i);
		maxHeapify(vec, 0, i);
	}
}

int main(void)
{
	int i, n, vec[15] = {86, 79, 85, 72, 76, 68, 63, 26, 51, 50, 49, 9, 64, 34, 47};
	n = 15;
	//vec = createVector(n);
	buildMaxHeap(vec, n);
	for(i = 0; i < n; i++) printf("%d ", vec[i]);
	printf("\n");
	decreaseKey(vec, n, 3, 1);
	for(i = 0; i < n; i++) printf("%d ", vec[i]);
	printf("\n");
	increaseKey(vec, n, 10, 100);
	for(i = 0; i < n; i++) printf("%d ", vec[i]);
	printf("\n");
	heapsort(vec, n);
	for(i = 0; i < n; i++) printf("%d ", vec[i]);
	printf("\n");
	return 0;
}
