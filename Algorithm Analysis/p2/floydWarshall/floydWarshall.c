#include <stdio.h>
#include <stdlib.h>

void floydWarshall(int adjMatrix[4][4])
{
	int i, j, k;
	for(k = 0; k < 4; k++)
	{
		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 4; j++)
			{
				if(adjMatrix[i][j] > adjMatrix[i][k] + adjMatrix[k][j])
				{
					adjMatrix[i][j] = adjMatrix[i][k] + adjMatrix[k][j];
				}
			}
		}
	}
}

void printMat(int matrix[4][4], int rows, int cols)
{
	int i, j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(void)
{
	int adjMatrix[4][4] = { {0,3,6,15}, {100,0,-2,100}, {100,100,0,2}, {1,100,100,0} };
	printf("antes\n");
	printMat(adjMatrix, 4, 4);
	floydWarshall(adjMatrix);
	printf("depois\n");
	printMat(adjMatrix, 4, 4);
	return 0;
}
