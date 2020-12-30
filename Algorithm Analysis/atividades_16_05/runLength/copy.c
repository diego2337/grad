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
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMatrix(char **matrix, int n)
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
    char c;
    count = 0;
    char *line = NULL;
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != ' ' && c != '\n' && c != EOF);
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
*/
void readIntoMatrix(char **matrix, int n, int m, FILE *file)
{
	int i, j;
    char c;
    i = 0;
    j = 0;
    c = fgetc(file);
    /* Keep reading while file isn't over */
    while(c != EOF && i <= n && j <= m)
    {
    	if(c != ' ' && c != '\n')
    	{
    		matrix[i][j] = c;
    		j = j + 1;
    	}
    	else if(c == '\n') 
    	{
			i = i + 1;
			j = 0;
    	}
    	c = fgetc(file);
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
void rleCompression(char *compression, char *maxGrayScale, char **matrix, int n, int m)
{
	int i, j, k, l, repeat = 1;
	char *line;
	printf("%s\n", compression);
	printf("%d %d\n", n, m);
	printf("%s\n", maxGrayScale);
	for(i = 0; i < n; i++)
	{
		// line = (char*)malloc(sizeof(char)*m);
		// l = 0;
		for(j = 0; j < m; j++)
		{
			if(j+1 < m && matrix[i][j] == matrix[i][j+1])
			{
				repeat = repeat + 1;
			}
			else if(repeat > 3)
			{
				// line[l] = '@';
				// l = l + 1;
				// line[l] = ' ';
				// l = l + 1;
				// line[l] = matrix[i][j-1];
				// l = l + 1;
				// line[l] = ' ';
				// l = l + 1;
				// line[l] = repeat+48;
				// l = l + 1;
				// line[l] = ' ';
				// l = l + 1;
				if(j+1 == m)
				{
					printf("@ %c %d", matrix[i][j], repeat);
				}
				else
				{
					printf("@ %c %d ", matrix[i][j], repeat);
				}
				repeat = 1;
			}
			else
			{
				for(k = 0; k < repeat; k++)
				{
					// line[l] = matrix[i][j-1];
					// l = l + 1;
					if(j+1 == m && k+1 == repeat)
					{
						printf("%c", matrix[i][j]);
					}
					else
					{
						printf("%c ", matrix[i][j]);
					}
				}
				repeat = 1;
			}
		}
		// if(repeat > 3)
		// {
		// 	printf("@ %c %d", matrix[i][j], repeat);
		// 	repeat = 1;
		// }
		// else
		// {
		// 	if(repeat == 1)
		// 	{
		// 		printf("%c", matrix[i][j-1]);
		// 	}
		// 	else
		// 	{
		// 		for(k = 0; k < (repeat-1); k++)
		// 		{
		// 			printf("%c ", matrix[i][j-1]);
		// 		}
		// 		printf("%c", matrix[i][j-1]);
		// 	}
		// }
		printf("\n");
	}
}

int main(void)
{
	/* Variable declarations */
	/* param - string to read */
	char *param, **matrix, *compression, *maxGrayScale;
	/* i: loop variable; n, m: matrix rows and columns size */
	int i, n, m;
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
	matrix = (char**)malloc(sizeof(char*)*n);
	for(i = 0; i < n; i++)
	{
		matrix[i] = (char*)malloc(sizeof(char)*m);
	}
	/* Read max gray-scale number */
	free(param);
	param = NULL;
	param = readLine(stdin);
	maxGrayScale = (char*)malloc(sizeof(char)*(strlen(param)+1));
	strcpy(maxGrayScale, param);
	free(param);
	/* Step 1.4: Fill matrix with .pgm file */
	readIntoMatrix(matrix, n, m, stdin);

	/* Step 2: Execute either compression or decompression */
	if(strcmp(compression, "P2") == 0)
	{
		strcpy(compression, "P8");
		rleCompression(compression, maxGrayScale, matrix, n, m);
	}
	free(compression);
	free(maxGrayScale);
	freeMatrix(matrix, n);
	return 0;
}