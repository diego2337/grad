/*
	Program to identify captchas in a .pgm image
	Author: Diego Cintra
	USP Number: 10094043
	Date: 26/04/2017
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
void printMatrix(char **matrix, int n, int m)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
        {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
}

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
	To reduce some noise, check if at least 4 neighbors are 1
	params:
	   - matrix: matrix to be treated
	   - n: matrix row size
	   - m: matrix column size
	   - i: row position to be looked
	   - j: column position to be looked
*/
char checkNeighbors(char **matrix, int n, int m, int i, int j)
{
	int count = 0;
	/* Check north */
    if((i-1) >= 0)
    {
        if(matrix[i-1][j] == '1')
        {
            count = count + 1;
        }
    }
    /* Check northeast */
    if((i-1) >= 0 && (j+1) < m)
    {
        if(matrix[i-1][j+1] == '1')
        {
            count = count + 1;
        }
    }
    /* Check east */
    if((j+1) < m)
    {
        if(matrix[i][j+1] == '1')
        {
            count = count + 1;
        }
    }
    /* Check southeast */
    if((i+1) < n && (j+1) < m)
    {
        if(matrix[i+1][j+1] == '1')
        {
            count = count + 1;
        }
    }
    /* Check south */
    if((i+1) < n)
    {
        if(matrix[i+1][j] == '1')
        {
            count = count + 1;
        }
    }
    /* Check southwest */
    if((i+1) < n && (j-1) >= 0)
    {
        if(matrix[i+1][j-1] == '1')
        {
            count = count + 1;
        }
    }
    /* Check west */
    if((j-1) >= 0)
    {
        if(matrix[i][j-1] == '1')
        {
            count = count + 1;
        }
    }
    /* Check northwest */
    if((i-1) >= 0 && (j-1) >= 0)
    {
        if(matrix[i-1][j-1] == '1')
        {
            count = count + 1;
        }
    }
    return count > 3 ? '1' : '0';
}

/*
	Filter noise in ".pgm" file
	params:
	   - matrix: matrix to be filtered
	   - n: matrix row size
	   - m: matrix column size
	returns:
	   - a filtered matrix
*/
char **filterNoise(char **matrix, int n, int m)
{
	int i, j;
	char **filter;
	filter = NULL;
	filter = (char**)malloc(sizeof(char*)*n);
	for(i = 0; i < n; i++)
	{
		filter[i] = (char*)malloc(sizeof(char)*m);
	}
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
		{
			filter[i][j] = checkNeighbors(matrix, n, m, i, j);
		}
	}
	printMatrix(filter, n, m);
	return filter;
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
	Identify captcha pattern
	params:
	   - matrix: matrix to be read
	   - n: matrix row size
	   - m: matrix column size
	   - iIndex: row index of first occurrence of color
	   - jIndex: column index of first occurrence of color
*/
void identifyPattern(char **matrix, int n, int m, int *iIndex, int *jIndex)
{
	int i, j, k, nextColumns = 0, colorCount = 0, pattern = -1, wasColored;
	k = (*jIndex);
	wasColored = -1;
	for(i =  (*iIndex) + 25; i < n && nextColumns < 3; nextColumns++)
	{
		/* Step 4: if next 10 colors in a line have high density, check next 10 columns */
		for(j = (*jIndex); j < m && j < ((*jIndex)+10); j++)
		{
			if(matrix[i][j] != '0' && matrix[i][j] >= 48)
			{
				colorCount = colorCount + 1;
			}
		}
		/* Switch to different column possibilities */
		switch(nextColumns)
		{
			case 0:
				if(colorCount < 6) /* Possible value: 7 */
				{
					/* Variable marker for next iteration*/
					wasColored = 0;
				}
				else /* Possible values: 1, 2, 3, 4, 5, 6, 8, 9, 0 */
				{
					wasColored = 1;
				}
			break;

			case 1:
				if(colorCount < 6) /* Possible values: 1, 7, 0 */
				{
					if(wasColored == 0) /* 7 - CHECKED */
					{
						pattern = 7;
						j = k+31;
						nextColumns = 3;
					}
					/* else if(wasColored == 1) Possible values: 1, 0 */
					
				}
				else /* Possible values: 2, 3, 4, 5, 6, 8, 9 */
				{
					wasColored = 2;
					colorCount = 0;
					/* Check first 20 line values; if empty, possible values: 2, 3 */
					for(i = ((*iIndex)+10); i < (*iIndex)+20 && i < n; i++)
					{
						if(matrix[i][k] != '0' && matrix[i][k] >= 48)
						{
							colorCount = colorCount + 1;
						}
					}
					if(colorCount < 4) /* Possible values: 2, 3 */
					{
						colorCount = 0;
						/* Check first 10 values after half; if empty, 3; else, 2 */
						for(i = ((*iIndex)+30); i < (*iIndex)+40 && i < n; i++)
						{
							if(matrix[i][k] != '0' && matrix[i][k] >= 48)
							{
								colorCount = colorCount + 1;
							}
						}
						if(colorCount < 6) /* 3 - CHECKED */
						{
							pattern = 3;
							j = k+31;
							nextColumns = 3;
						}
						else /* 2 - CHECKED */
						{
							pattern = 2;
							j = k+31;
							nextColumns = 3;
						}
					}
					else
					{
						/* Check last 10 line values: if empty, 4 */
						colorCount = 0;
						for(i = ((*iIndex)+40); i < (*iIndex)+50 && i < n; i++)
						{
							// printf("i: %d\tk: %d\tmatrix[i][k]: %c\n", i, k, matrix[i][k]);
							if(matrix[i][k] != '0' && matrix[i][k] >= 48)
							{
								colorCount = colorCount + 1;
							}
						}
						if(colorCount < 6) /* 4 - CHECKED */
						{
							pattern = 4;
							j = k+31;
							nextColumns = 3;
						}
					}
					if(j < (k+31))
					{
						/* Check before last 10 line values: if empty, possible 5 or 9 */
						colorCount = 0;
						for(i = ((*iIndex)+30); i < (*iIndex)+40 && i < n; i++)
						{
							if(matrix[i][k] != '0' && matrix[i][k] >= 48)
							{
								colorCount = colorCount + 1;
							}
						}
						if(colorCount < 6) /* Possible 5 or 9 */
						{
							/* Check last column block second block of 10 line values: if empty, 5; else, 9 */
							colorCount = 0;
							for(i = ((*iIndex)+10); i < (*iIndex)+20 && i < n; i++)
							{
								if(matrix[i][k+20] != '0' && matrix[i][k+20] >= 48)
								{
									colorCount = colorCount + 1;
								}
							}
							if(colorCount < 4) /* 5 - CHECKED */
							{
								pattern = 5;
								j = k+31;
								nextColumns = 3;		
							}
							else /* 9 - CHECKED */
							{
								pattern = 9;
								j = k+31;
								nextColumns = 3;
							}
						}
						else /* Possible 6 or 8 */
						{
							/* Check last column block second block of 10 line values: if empty, 6; else, 8 */
							colorCount = 0;
							for(i = ((*iIndex)+10); i < (*iIndex)+20 && i < n; i++)
							{
								if(matrix[i][k+20] != '0' && matrix[i][k+20] >= 48)
								{
									colorCount = colorCount + 1;
								}
							}	
							if(colorCount < 6) /* 6 - CHECKED */
							{
								pattern = 6;
								j = k+31;
								nextColumns = 3;
							}
							else /* 8 - CHECKED */
							{
								pattern = 8;
								j = k+31;
								nextColumns = 3;
							}
						}	
					}
				}
			break;

			case 2:
				if(colorCount < 6) /* 1 - CHECKED */
				{
					pattern = 1;
					nextColumns = 3;
					/* Check first 10 values at column block; if empty, 1; else, 6 */
					// colorCount = 0;
					// for(i = ((*iIndex)+35); i < (*iIndex)+50 && i < n; i++)
					// {
					// 	if(matrix[i][(*jIndex)] != '0' && matrix[i][(*jIndex)] >= 48)
					// 	{
					// 		colorCount = colorCount + 1;
					// 	}
					// }
					// if(colorCount < 6 && wasColored == 1) /* 1 */
					// {
					// 	pattern = 1;
					// 	nextColumns = 3;
					// }
					// else if(wasColored == 1) /* 6 */
					// {
					// 	pattern = 6;
					// 	nextColumns = 3;
					// }
				}
				else /* 0 - CHECKED */
				{
					if(wasColored == 1)
					{
						pattern = 0;
						j = k+31;
						nextColumns = 3;
					}
				}
			break;

			default:
			break;
		}
		/* else if user hasn't read 30 columns, advance 10 columns. Return to step 4;*/
		colorCount = 0;
		(*jIndex) = j;
	}
	// printf("pattern: %d\n", pattern);
	printf("%d", pattern);
	/* else stop. Return to step 1 */
}

/*
	Seek any association with number captchas
	params:
	   - matrix: matrix to be read
	   - n: matrix row size
	   - m: matrix column size
*/
void findAssociation(char **matrix, int n, int m)
{
	int i, j, iIndex, jIndex, zeroCount, colorCount;
	/*
		GENERAL IDEA
		Step 1: while there are still lines - check lines; if any color is found, store index;
		Step 2: keep checking lines; if 5 or more zeros are found, no pattern. Return to step 1;
		Step 3: if 10 colors are found in a column, possible pattern. Go back to stored index and sum 20 
			(every captcha has 50 columns and 30 rows, approximately)
		Step 4: if next 10 colors in a line have high density, check next 10 columns:
		    		if next 10 have low density, possible "1" or "0";
		    		else, possible captcha between "2-9";
		    	else if user hasn't read 30 columns, advance 10 columns. Return to step 4;
		    	else stop. Return to step 1;
	*/

	/* Check column by column */
	for(j = 0; j < m; j++)
	{
		zeroCount = -1;
		colorCount = 0;
		iIndex = -1;
		jIndex = -1;
		for(i = 0; i < n; i++)
		{
			if(matrix[i][j] != '0' && zeroCount < 4 && iIndex == -1 && jIndex == -1) /* Step 1: store index if any color is found */
			{
				iIndex = i;
				jIndex = j;
				colorCount = colorCount + 1;
				zeroCount = zeroCount + 1;
			}
			else if(matrix[i][j] != '0' && zeroCount < 4 && iIndex != -1 && jIndex != -1)
			{
				colorCount = colorCount + 1;
			}
			else if(matrix[i][j] == '0' && zeroCount >= 4) /* Step 2: keep checking; if 5 or more zeros are found, return to step 1 */
			{
				zeroCount = -1;
				colorCount = 0;
				iIndex = -1;
				jIndex = -1;
			}
			else if(matrix[i][j] == '0' && zeroCount < 4 && zeroCount >= 0)
			{
				zeroCount = zeroCount + 1;
			}
			if(colorCount > 7) /* Step 3: if 10 colors are found in a column, there's a possible pattern. Go back to stored index and sum 20 */
			{
				// printf("BEFORE: iIndex: %d\t jIndex: %d\ti: %d\tj: %d\n", iIndex, jIndex, i, j);
				identifyPattern(matrix, n, m, &iIndex, &jIndex);
				// printf("AFTER: iIndex: %d\t jIndex: %d\ti: %d\tj: %d\n", iIndex, jIndex, i, j);
				/* Finished recognizing a pattern so end column loop */
				i = n+1;
				/* Already seen as far as 30 columns, so skip next 30 columns */
				j = jIndex;
				// printf("i: %d\tj: %d\n", i, j);
			}
		}
	}
	printf("\n");
}

int main(void)
{
	/* Variable declarations */
	/* File - stream for reading and writing files */
	FILE *file;
	/* fileName - name of ".pgm" extension file; param - string to read */
	char *fileName, *param, **matrix, **filter;
	/* i: loop variable; n, m: matrix rows and columns size */
	int i, n, m;

	/* Step 1: Read and open ".pgm" file */
	fileName = NULL;
	fileName = readLine(stdin);
	// printf("fileName: %s\n", fileName);
	file = fopen(fileName, "rb");
	if(file)
	{
		/* Step 2: Store in matriz ".pgm" file */
		/* Skip first line */
		param = NULL;
		param = readLine(file);
		free(param);
		/* Read matrix row and column sizes */
		param = NULL;
		param = readCommand(file);
		m = toNum(param);
		free(param);
		param = NULL;
		param = readCommand(file);
		n = toNum(param);
		/* Allocate matrix */
		matrix = (char**)malloc(sizeof(char*)*n);
		for(i = 0; i < n; i++)
		{
			matrix[i] = (char*)malloc(sizeof(char)*m);
		}
		/* Read max gray-scale number - skipped because not used here */
		free(param);
		param = NULL;
		param = readLine(file);
		/* Fill matrix with elements */
		readIntoMatrix(matrix, n, m, file);
		filter = NULL;
		filter = filterNoise(matrix, n, m);
		// printMatrix(filter, n, m);

		/* Step 3: Find associations */
		findAssociation(filter, n, m);

		/* Step ?: Free variables */
		free(param);
		free(fileName);
		freeMatrix(matrix, n);
		freeMatrix(filter, n);
		fclose(file);
	}
	else
	{
		printf("File not found.\n");
	}
	return 0;
}