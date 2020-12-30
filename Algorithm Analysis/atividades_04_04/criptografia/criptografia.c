/*
    Program which decrypts a message. Assuming this message is a matrix Y, and M an identity matrix, finding
    the original message is given by X = Y * M^(-1)
    Author: Diego Cintra
    USP number: 10094043
    Date: 26/03/2017
*/

#include <stdio.h>
#include <stdlib.h>
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
        if(matrix[i][i] == 0) /* switch lines */
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
       - solvable: if assigned with -1, indicates that linear system is unsolvable
    returns:
       - a double type matrix with results
*/
double** gaussJordan(double **matrix, int n, int *solvable)
{
    int i, j, k, isNegative;
    double pivot_value, **finalResults;
    for(i = 0; i < n; i++)
    {
        pivot_value = matrix[i][i];
        if(pivot_value != 0)
        {
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
	        *solvable = 1;
        }
        else
        {
        	/* If pivot value is 0, then linear system is unsolvable */
        	*solvable = -1;
        	i = n;
        }
    }
    /* Returns results */
    finalResults = malloc(sizeof(double*) * n);
    for(i = 0; i < n; i++)
    {
        finalResults[i] = malloc(sizeof(double) * (n));
        for(j = 0; j < (n); j++)
        {
        	if(*solvable == 1)
        	{
        		finalResults[i][j] = matrix[i][j+n];
        	}
        	else
        	{
        		finalResults[i][j] = 0.00000;
        	}
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

int main(int argc, char **argv)
{
	FILE *file;
	int count, i, j, k, solvable = 0, index;
	double **matrix, **invertible, **scaled;
	char *fileName, c, conversion[92] = {' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
        'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6',
        '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
        ',', '-', '.', '/', '[', '\\', ']', '_', '{', '|', '}'};
	/* Step 1: get filename */
    fileName = NULL;
    count = 0;
    do{
        c = fgetc(stdin);
        fileName = realloc(fileName, sizeof(char)*(count+1));
        fileName[count++] = c;

    }while(c != '\n' && c != '\r');
    fileName[count-1] = '\0';
    // printf("%s\n", fileName);
	// fileName = (char*)malloc(sizeof(char) * 1);
	// c = getchar();
	// while(c != '\n' && c != EOF)
	// {
	// 	count = count + 1;
	// 	fileName = realloc(fileName, sizeof(char) * count);
	// 	fileName[count-2] = c;
	// 	c = getchar();
	// }
	// fileName[count-1] = '\0';
	/* Step 2: get message */
	file = fopen(fileName, "r");
	if(file)
	{
		/* Skip whole file to get k size */
		fseek(file, 0, SEEK_END);
		k = ftell(file);
		/* Get k value */
		k = sqrt(k);
		rewind(file);
		/* Step 3: allocate matrix */
		matrix = (double**)malloc(sizeof(double*) * k);
		for(i = 0; i < k; i++)
		{
			matrix[i] = (double*)malloc(sizeof(double) * k);
		}
		/* Step 4: read message */
		for(i = 0; i < k; i++)
		{
			for(j = 0; j < k; j++)
			{
				fscanf(file, "%c", &c);
				matrix[j][i] = findInVector(conversion, c);
			}
		}
		fclose(file);
		/* Step 5: read M matrix */
		invertible = (double**)malloc(sizeof(double*) * k);
		/* scaled matrix will contain both matrix M and its identity */
        scaled = (double**)malloc(sizeof(double*) * k);
        for(i = 0; i < k; i++)
        {
            invertible[i] = (double*)malloc(sizeof(double) * k);
            scaled[i] = (double*)malloc(sizeof(double) * (k*2));
        }
        for(i = 0; i < k; i++)
        {
            for(j = 0; j < k; j++)
            {
                scanf("%lf", &scaled[i][j]);
            }
        }
        /* Step 6: increase matrix to include identity */
        increaseWithIdentity(scaled, k);
        switchLines(scaled, k, (k*2));
        freeMatrix(invertible, k);
        /* Step 7: solve linear system with gauss jordan */
        invertible = gaussJordan(scaled, k, &solvable);
        if(solvable != -1)
        {
        	/* Step 8: multiply matrices */
        	freeMatrix(scaled, k);
        	scaled = multiplyMatrices(invertible, k, k, matrix, k, k);
	        /* Step 9: print results */
	        for(i = 0; i < k; i++)
	        {
	            for(j = 0; j < k; j++)
	            {
	            	index = (int)(fabs(scaled[j][i]));
	            	/* printf("i: %d\t, conversion[index]: %c\t", index, (conversion[index % 92])); */
	                printf("%c", (conversion[index % 92]));
	            }
	        }
            printf("\n");
        }
        else
        {
        	printf("M matrix has no inverse\n");
        }
        /* Step 10: free variables */
        freeMatrix(matrix, k);
        freeMatrix(invertible, k);
        freeMatrix(scaled, k);
	}
	else
	{
		printf("File not found.\n");
	}
	free(fileName);
	return 0;
}