#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Alteração: remoção de parâmetro "size" (comentado) */
void printKnapsack(int **vTable, int **keepTable, int I, int W, int *weights/*, int size*/)
{
	int i, j;
	for(i = 0; i < I; i++)
	{
		for(j = 0; j < W; j++)
		{
			printf("%d ", vTable[i][j]);
		}
		printf("\n");
	}
	for(i = 0; i < I; i++)
	{
		for(j = 0; j < W; j++)
		{
			printf("%d ", keepTable[i][j]);
		}
		printf("\n");
	}
	while(I != 0 && W != 0)
	{
		if(keepTable[I-1][W-1] == 1)
		{
			printf("%d ", I-1);
			I = I - 1;
			W = W - weights[I-1];
		}
		else
		{
			I = I - 1;
		}
	}
	/* Alteração: inclusão do '\n' ao final da impressão */
	printf("\n");
}

void knapsack(int *values, int *weights, int **vTable, int **keepTable, int items, int knapsackSize)
{
	int i, j, value;
	for(i = 1; i < items; i++)
	{
		for(j = 0; j < knapsackSize; j++)
		{
			if(weights[i-1] <= j+1)
			{
				if((j+1)-weights[i-1] > 0)
				{
					value = values[i-1] + vTable[i-1][j-weights[i-1]];
				}
				else
				{
					value = values[i-1];
				}
				if(value > vTable[i-1][j])
				{
					vTable[i][j] = value;
					keepTable[i][j] = 1;
				}
				else
				{
					vTable[i][j] = vTable[i-1][j];
					keepTable[i][j] = 0;
				}
			}
		}
	}
	printKnapsack(vTable, keepTable, items, knapsackSize, weights);	
}

int main(void)
{
	/* Inclusao de um ';' no final da declaracao das variaveis */
	int items = 0, i = 0, knapsackSize = 0;
	int *values = NULL, *weights = NULL, **vTable = NULL, **keepTable = NULL;
	scanf("%d", &items);
	values = (int*)calloc(items, sizeof(int));
	weights = (int*)calloc(items, sizeof(int));
	for(i = 0; i < items; i++)
	{
		scanf("%d", &values[i]);
		scanf("%d", &weights[i]);
	}
	scanf("%d", &knapsackSize);
	vTable = (int**)calloc((items+1), sizeof(int*));
	keepTable = (int**)calloc((items+1), sizeof(int*));
	for(i = 0; i < items+1; i++)
	{
		vTable[i] = (int*)calloc(knapsackSize, sizeof(int));
		keepTable[i] = (int*)calloc(knapsackSize, sizeof(int));
	}
	knapsack(values, weights, vTable, keepTable, (items+1), knapsackSize);
	free(values);
	free(weights);
	for(i = 0; i < (items+1); i++)
	{
		free(vTable[i]);
		free(keepTable[i]);
	}
	free(vTable);
	free(keepTable);
	return 0;
}
