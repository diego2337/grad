/*
    Program which decrypts a message. Assuming this message is a matrix Y, and M an identity matrix, finding
    the original message is given by X = Y * M^(-1)
    Author: Diego Cintra
    USP number: 10094043
    Date: 26/03/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* 
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMatrix(double **matrix, int n, int m)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
        {
            printf("%lf ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMatrix(double **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

/*
    Mulitplies matrices A and B
    params:
       - A: matrix A
       - n1: matrix A's line length
       - m1: matrix A's column length
       - B: matrix B
       - n2: matrix B's line length
       - m2: matrix B's column length
*/
double **multiplyMatrices(double **A, int n1, int m1, double **B, int n2, int m2)
{
    int i, j, k;
    double **results;
    results = malloc(sizeof(double*) * m1);
    for(i = 0; i < m1; i++)
    {
        results[i] = malloc(sizeof(double) * n2);
    }
    for(i = 0; i < m1; i++)
    {
        for(j = 0; j < n2; j++)
        {
            results[i][j] = 0;
            for(k = 0; k < n2; k++)
            {
                results[i][j] = results[i][j] + (A[i][k]*B[k][j]);
            }
        }
    }
    return results;
}

/*
    Swap matrices' lines
    params:
       - matrix: matrix AB (increased)
       - i: line to swap with line j
       - j: line to swap with line i
       - n: matrix line's length
       - m: matrix column's length
*/
void swapLines(double **matrix, int i, int j, int n, int m)
{
    int y;
    double aux;
    for(; i < n; i++)
    {
        for(y = 0; y < m; y++)
        {
            aux = matrix[i][y];
            matrix[i][y] = matrix[j][y];
            matrix[j][y] = aux;
        }
    }
}

/*
    Iteratively checks line arrangement
    params:
       - matrix: matrix AB (increased)
       - n: matrix line's length
       - m: matrix column's length
*/
void switchLines(double **matrix, int n, int m)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        if(matrix[i][i] == 0) // switch lines
        {
            for(j = i+1; j < n; j++)
            {
                if(matrix[j][i] != 0)
                {
                    swapLines(matrix, i, j, n, m);
                }
            }   
        }
    }
}

/*
    Solve linear system with gauss jordan
    params:
       - matrix: matrix A
       - n: matrix's size
    returns:
       - a double type matrix with results
*/
double** gaussJordan(double **matrix, int n)
{
    int i, j, k, isNegative;
    double pivot_value;
    for(i = 0; i < n; i++)
    {
        pivot_value = matrix[i][i];
        /* Simplifies pivot line according to pivot value */
        for(k = 0; k < (n*2); k++)
        {
            if(matrix[i][k] != 0.00000)
            {
                matrix[i][k] = (matrix[i][k]/pivot_value);   
            }
        }
        /* Iterates through lines */
        for(j = 0; j < n; j++)
        {
            /* Defines "pivot" value (element at same column as the pivot to be "zeroed")*/
            pivot_value = matrix[j][i];
            /* Checks to see if line is different from one pivoted */
            if(j != i)
            {
                /* Activates flag if pivot element at line is negative */
                if(pivot_value < 0.00000)
                {
                    isNegative = 1;
                }
                else
                {
                    isNegative = 0;
                }
                /* Iterates through columns */
                for(k = i; k < (n*2); k++)
                {
                    if(isNegative)
                    {
                        matrix[j][k] = matrix[j][k] + (matrix[i][k] * fabs(pivot_value));
                    }
                    else
                    {
                        matrix[j][k] = matrix[j][k] - (matrix[i][k] * pivot_value);
                    }
                }   
            }
        }
    }
    /* Returns results */
    double **finalResults = malloc(sizeof(double*) * n);
    for(i = 0; i < n; i++)
    {
        finalResults[i] = malloc(sizeof(double) * (n));
        for(j = 0; j < (n); j++)
        {
            finalResults[i][j] = matrix[i][j+n];   
        }
    }
    return finalResults;
}

/*
    Increases given matrix, assuming allocation is already correct
    params:
       - matrix: matrix to be increased
       - k: k x k matrix size
*/
void increaseWithIdentity(double **matrix, int k)
{
    int i, j;
    for(i = 0; i < k; i++)
    {
        for(j = 0; j < k; j++)
        {
            if(i == j)
            {
                matrix[i][j+k] = 1;
            }
            else
            {
                matrix[i][j+k] = 0;
            }
        }
    }
}

/*
    Finds character in char vector
    params:
       - conversion: char vector
       - c: char to be found
    returns:
       - index to position in vector
*/
int findInVector(char conversion[92], char c)
{
    int i;
    for(i = 0; i < 92; i++)
    {
        if(conversion[i] == c)
        {
            return i;
        }
    }
    return -1;
}

int main(void)
{
    FILE *file;
    int i, j, k;
    double **matrix, **invertible, **scaled;
    /* Vector for conversion */
    char fileName[512], c, conversion[92] = {' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
        'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6',
        '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
        ',', '-', '.', '/', '[', '\\', ']', '_', '{', '|', '}'};
    /* Step 1: get filename and message */
    scanf("%[^\n]c\n", fileName);
    file = fopen(fileName, "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        k = ftell(file);
        /* Get k value*/
        k = sqrt(k);
        rewind(file);
        /* Step 2: allocate matrix */
        matrix = malloc(sizeof(double*) * k);
        for(i = 0; i < k; i++)
        {
            matrix[i] = malloc(sizeof(double) * k);
        }
        /* Step 3: read message */
        for(i = 0; i < k; i++)
        {
            for(j = 0; j < k; j++)
            {
                fscanf(file, "%c", &c);
                matrix[j][i] = findInVector(conversion, c);
            }
        }
        fclose(file);
        /* Step 4: read M matrix */
        invertible = malloc(sizeof(double*) * k);
        /* scaled matrix will contain both matrix M and its identity */
        scaled = malloc(sizeof(double*) * k);
        for(i = 0; i < k; i++)
        {
            invertible[i] = malloc(sizeof(double) * k);
            scaled[i] = malloc(sizeof(double) * (k*2));
        }
        for(i = 0; i < k; i++)
        {
            for(j = 0; j < k; j++)
            {
                scanf("%lf", &scaled[i][j]);
            }
        }
        /* Step 5: increase matrix to include identity */
        increaseWithIdentity(scaled, k);
        switchLines(scaled, k, (k*2));
        freeMatrix(invertible, k);
        /* Step 6: solve linear system with gauss jordan */
        invertible = gaussJordan(scaled, k);
        /* Step 7: multiply matrices */
        freeMatrix(scaled, k);
        scaled = multiplyMatrices(matrix, k, k, invertible, k, k);
        /* Step 8: print results */
        for(i = 0; i < k; i++)
        {
            for(j = 0; j < k; j++)
            {
                printf("%c", conversion[(int)abs(scaled[j][i])]);
            }
        }
        /* Step 9: free variables */
        freeMatrix(matrix, k);
        freeMatrix(invertible, k);
        freeMatrix(scaled, k);
    }
    else
    {
    	printf("Arquivo nao encontrado\n");
    }
    return 0;
}