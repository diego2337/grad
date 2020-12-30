#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double **createDistances(int n)
{
	srand(time(NULL));
	int i, j; double **matrix;
	matrix = (double**)calloc(n, sizeof(double*));
	for(i = 0; i < n; i++){ matrix[i] = (double*)calloc(n, sizeof(double));}
	for(i = 0; i < n-1; i++)
	{
		for(j = i+1; j < n; j++)
		{
			matrix[i][j] = matrix[j][i] = rand() % 999;
		}
	}
	return matrix;
}

int *generateSolution(int *path, int n, double *minDist, double **distances)
{
	int i, rand1, rand2, aux, *sol = (int*)calloc(n, sizeof(int));
	if(!path) for(i = 0; i < n; i++) sol[i] = i;
	else
	{
		for(i = 0; i < n; i++)
		{
			sol[i] = path[i];
		}
	}
	rand1 = rand() % n;
	rand2 = rand() % n;
	aux = sol[rand1];
	sol[rand1] = sol[rand2];
	sol[rand2] = aux;
	for(i = 1; i < n; i++)
	{
		/*printf("%d\n", sol[i]);*/
		(*minDist) = (*minDist) + distances[sol[i-1]][sol[i]];
	}
	(*minDist) = (*minDist) + distances[sol[i-1]][sol[0]];
	return sol;
}

void print(int *vec, int n, double minDist)
{
	int i;
	for(i = 0; i < n; i++)
	{
		printf("%d " , vec[i]);
	}
	printf("-> %lf\n", minDist);
}

int main(void)
{
	int i, n, *cost = NULL, nGen;
	double **distances = NULL, minDist = 0.00, min = 1000000.00;
	scanf("%d", &n);
	scanf("%d", &nGen);
	distances = createDistances(n);
	for(i = 0; i < nGen; i++)
	{
		minDist = 0.00;
		cost = generateSolution(cost, n, &minDist, distances);
		if(minDist < min)
		{
			min = minDist;
			print(cost, n, minDist);
		}
		/*else
		{
			free(cost);
			cost = NULL;
		}*/
	}
	return 0;
}
