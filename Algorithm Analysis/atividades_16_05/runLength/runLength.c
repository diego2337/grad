/*
	Program to execute RLE algorithm on .pgm extension files.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 07/05/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMatrix(int **matrix, int n, int m)
{
    int i, j;
    if(m != -1 && m != -2)
    {
    	for(i = 0; i < n; i++)
	    {
    		for(j = 0; j < m; j++)
	        {
	            printf("%d", matrix[i][j]);
	        }
	        printf("\n");
	    }
    }
    else if(m == -1)
    {
    	// for(i = 0; i < n; i++)
    	// {
    	// 	for(j = 0; j < strlen(matrix[i]); j++)
    	// 	{
    	// 		printf("%d", matrix[i][j]);
    	// 	}
    	// 	printf("\n");
    	// }
    }
    else if(m == -2)
    {
    	for(i = 0; i < n; i++)
    	{
    		for(j = 0; matrix[i][j] != -2; j++)
    		{
    			printf("%d", matrix[i][j]);
    		}
    		printf("\n");
    	}	
    }
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMatrix(int **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

/*
	Converts a null terminating string to integer
	param:
	   - number: char type number to be converted
	returns:
	   - int type number
*/
int toNum(char *number)
{
	int i, num = 0;
	for(i = 0; i < strlen(number); i++)
	{
		num = ((num)*10) + (number[i]-48);
	}
	return num;
}

/*
	Read line
	param:
	   - file: file to be read
	returns:
	   - line read
*/
char *readLine(FILE *file)
{
	int count;
    char c;
    count = 0;
    char *line = NULL;
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != '\n' && c != '\r' && c != EOF);
    if(c == EOF && count == 1)
    {
    	free(line);
    	line = NULL;
    	line = (char*)realloc(line, sizeof(char)*3);
    	strcpy(line, "-1");
    }
    else
    {
    	line[count-1] = '\0';
    }
    return line;	
}

/*
	Read command
	param:
	   - file: file to be read
	returns:
	   - command read
*/
char *readCommand(FILE *file)
{
	int count;
    char c = 0;
    count = 0;
    char *line = NULL;
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != ' ' && c != '\n' && c != '\r' && c != EOF);
    line[count-1] = '\0';
    return line;
}

/*
	Read contents from file into matrix
	params:
	   - matrix: matrix to store elements
	   - n: matrix row size
	   - m: matrix column size
	   - file: file to be read
	   - compression: (P2) indicates compression, (P8), decompression
*/
void readIntoMatrix(int **matrix, int n, int m, FILE *file, char *compression)
{
	int i, j, k;
    char c, *number;
    i = 0;
    j = 0;
    if(strcmp(compression, "P8") == 0)
    {
    	for(i = 0; i < n; i++)
    	{
    		c = fgetc(file);
    		j = 0;
    		while(c != '\n' && c != EOF)
    		{
    			if(c != ' ')
	    		{
	    			if(j == 0)
	    			{
	    				matrix[i] = NULL;
	    			}
	    			number = NULL;
	    			k = 0;
	    			while(c != ' ' && c != '\n' && c != '\r' && c != EOF)
	    			{
				        number = (char*)realloc(number, sizeof(char)*(k+1));
				        number[k] = c;
				        k = k + 1;
				        c = fgetc(file);
				    }
				    number = (char*)realloc(number, sizeof(char)*(k+1));
				    number[k] = '\0';
	    			matrix[i] = (int*)realloc(matrix[i], sizeof(int)*(j+1));
	    			/* Special treatment in case if it's '@' symbol */
	    			if(strcmp(number, "@") == 0)
	    			{
	    				matrix[i][j] = -1;
	    			}
	    			else
	    			{
	    				matrix[i][j] = toNum(number);
	    			}
	    			free(number);
	    			j = j + 1;
	    		}
	    		else
	    		{
	    			c = fgetc(file);
	    		}
    		}
    		matrix[i] = (int*)realloc(matrix[i], sizeof(int)*(j+1));
    		/* Indicates end of line in matrix */
    		matrix[i][j] = -2;
    		j = 0;
    	}
    }
    else
    {
    	/* Keep reading while file isn't over */
    	for(i = 0; i < n; i++)
    	{
    		for(j = 0; j < m; j++)
    		{
		    	number = NULL;
		    	number = readCommand(file);
		    	matrix[i][j] = toNum(number);
		    	free(number);
    		}
    	}
    	// printMatrix(matrix, n, m);
    	// exit(0);
    }
}

/*
	Performs RLE compression
	params:
	   - compression: either "P2" or "P8" type file
	   - maxGrayScale: maximum gray scale value
	   - matrix: matrix to be compressed
	   - n: matrix row size
	   - m: matrix column size
*/
void rleCompression(char *compression, char *maxGrayScale, int **matrix, int n, int m)
{
	int i, j, k, repeat = 1;
	printf("%s\n", compression);
	printf("%d %d\n", m, n);
	printf("%s\n", maxGrayScale);
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
		{
			if(j+1 < m && matrix[i][j] == matrix[i][j+1])
			{
				repeat = repeat + 1;
			}
			else if(repeat > 3)
			{
				if(j+1 == m)
				{
					printf("@ %d %d", matrix[i][j], repeat);
				}
				else
				{
					printf("@ %d %d ", matrix[i][j], repeat);
				}
				repeat = 1;
			}
			else
			{
				for(k = 0; k < repeat; k++)
				{
					if(j+1 == m && k+1 == repeat)
					{
						printf("%d", matrix[i][j]);
					}
					else
					{
						printf("%d ", matrix[i][j]);
					}
				}
				repeat = 1;
			}
		}
		printf("\n");
	}
}

/*
	Performs RLE decompression
	params:
	   - compression: either "P2" or "P8" type file
	   - maxGrayScale: maximum gray scale value
	   - matrix: matrix to be decompressed
	   - n: matrix row size
	   - m: matrix column size
*/
void rleDecompression(char *compression, char *maxGrayScale, int **matrix, int n, int m)
{
	int i, j, k;
	printf("%s\n", compression);
	printf("%d %d\n", m, n);
	printf("%s\n", maxGrayScale);
	for(i = 0; i < n; i++)
	{
		for(j = 0; matrix[i][j] != -2; j++)
		{
			/* '@' symbol */
			if(matrix[i][j] == -1)
			{
				for(k = 0; k < matrix[i][j+2]; k++)
				{
					if(matrix[i][j+3] == -2 && (k+1) == matrix[i][j+2])
					{
						printf("%d", matrix[i][j+1]);
					}
					else
					{
						printf("%d ", matrix[i][j+1]);
					}
				}
				j = j + 2;
			}
			else
			{
				if(matrix[i][j+1] == -2)
				{
					printf("%d", matrix[i][j]);
				}
				else
				{
					printf("%d ", matrix[i][j]);
				}
			}
		}
		printf("\n");
	}
}

int main(void)
{
	/* Variable declarations */
	/* param - string to read ; compression - indicates if process is either compression or decompression; maxGrayScale - stores maximum gray scale */
	char *param, *compression, *maxGrayScale;
	/* i: loop variable; n, m: matrix rows and columns size; matrix: matrix to store elements read */
	int i, n, m, **matrix;
	/* Step 1: Store in matriz ".pgm" file */
	/* Check if process is either compression (P2) or decompression (P8) */
	param = NULL;
	param = readLine(stdin);
	compression = (char*)malloc(sizeof(char)*3);
	strcpy(compression, param);
	free(param);
	/* Step 1.1: Read matrix row and column sizes */
	param = NULL;
	param = readCommand(stdin);
	m = toNum(param);
	free(param);
	param = NULL;
	param = readCommand(stdin);
	n = toNum(param);
	/* Step 1.3: Allocate matrix */
	if(strcmp(compression, "P2") == 0)
	{
		matrix = (int**)malloc(sizeof(int*)*n);
		for(i = 0; i < n; i++)
		{
			matrix[i] = (int*)malloc(sizeof(int)*m);
		}
	}
	else
	{
		matrix = (int**)malloc(sizeof(int*)*n);
	}
	/* Read max gray-scale number */
	free(param);
	param = NULL;
	param = readCommand(stdin);
	maxGrayScale = (char*)malloc(sizeof(char)*(strlen(param)+1));
	strcpy(maxGrayScale, param);
	free(param);
	/* Step 1.4: Fill matrix with .pgm file */
	readIntoMatrix(matrix, n, m, stdin, compression);

	/* Step 2: Execute either compression or decompression */
	if(strcmp(compression, "P2") == 0)
	{
		strcpy(compression, "P8");
		rleCompression(compression, maxGrayScale, matrix, n, m);
	}
	else if(strcmp(compression, "P8") == 0)
	{
		strcpy(compression, "P2");
		rleDecompression(compression, maxGrayScale, matrix, n, m);
	}
	free(compression);
	free(maxGrayScale);
	freeMatrix(matrix, n);
	return 0;
}