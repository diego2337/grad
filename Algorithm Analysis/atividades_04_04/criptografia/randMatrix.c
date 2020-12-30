/*
    Program that generates a random matrix, of quadratic size.
    Author: Diego Cintra
    USP number: 10094043
    Date: 30/03/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(void)
{
    srand(time(NULL));
    int **matrix;
    int i, j, n, aux;
    n = 9;
    matrix = malloc(sizeof(int*) * (sqrt(n)));
    for(i = 0; i < sqrt(n); i++)
    {
        matrix[i] = malloc(sizeof(int) * sqrt(n));
        for(j = 0; j < sqrt(n); j++)
        {
            aux = rand() % 10;
            matrix[i][j] = aux;
        }
    }
    for(i = 0; i < sqrt(n); i++)
    {
        for(j = 0; j < sqrt(n); j++)
        {
            printf("%d\n", matrix[i][j]);
        }
        // printf("\n");
    }
    for(i = 0; i < sqrt(n); i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}