/*
	Author: Diego S. Cintra
	USP Number: 10094043
	Date: 06/06/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum{
	FIRST,
	SECOND
};

/*
	Struct for schema
*/
typedef struct _schema{
	int attrSize, *nearestNeighbors;
	char *table, *tableName, **names, **types;
}schema;

typedef struct _dist{
	int index;
	double distance;
}dist;

void init(dist *d, int i)
{
	d->index = i;
	d->distance = 0.00;
}

/*
	Compare function for qsort
	params:
	   - a: a element to be compared
	   - b: b element to be compared
	returns:
	   - result of comparison between a and b
*/
int cmp(const void *a, const void *b)
{
	if((*(dist*)a).distance < (*(dist*)b).distance) 
	{
		return -1;
	}
	else if((*(dist*)a).distance == (*(dist*)b).distance) 
	{
		return 0;
	}
	else
	{ 
		return 1;
	}
}

/*
	Compare function for qsort
	params:
	   - a: a element to be compared
	   - b: b element to be compared
	returns:
	   - result of comparison between a and b
*/
int cmp2(const void *a, const void *b)
{
	return (strcmp((*(char**)a), (*(char**)b)));
}

/*
	Find first occurrence of a delimiter and return either string until delimiter or string after delimiter
	params:
	   - str: string to be searched;
	   - delimiter: character that delimits reading;
	   - after: (1) will copy string from delimiter, (0) up until delimiter
	returns:
	   - str up to occurrence of first delimiter. If no occurrence, returns NULL
*/
char *split(char *str, char delimiter, int after)
{
	int i, j, strLen = 0;
	char *newStr = NULL;
	strLen = strlen(str);
	for(i = 0; i < strLen; i++)
	{
		if(str[i] == delimiter && !after)
		{
			str[i] = '\0';
			newStr = (char*)malloc(sizeof(char)*((i+1)+1));
			strcpy(newStr, str);
			i = strLen + 1;
		}
		else if(str[i] == delimiter && after)
		{
			newStr = (char*)malloc(sizeof(char)*((strLen-i)+1));
			i = i + 1;
			for(j = 0; i < strLen; j++)
			{
				newStr[j] = str[i];
				i = i + 1;
			}
			newStr[j] = '\0';
			i = strLen + 1;
		}
	}
	return newStr;
}

/*
	Read from a file until a given character occur
	params:
	   - file: file to be read
	   - delimiter: character that delimits reading
	returns:
	   - command read
*/
char *read(FILE *file, char delimiter)
{
	int count;
    char c;
    count = 0;
    char *line = NULL;
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != '\n' && c != delimiter && c != EOF);
    line[count-1] = '\0';
    return line;
}

/*
	Count number of lines from a given file, rewinding at end
	param:
	   - file: file to be read
	returns:
	   - number of lines
*/
int countLines(FILE *file)
{
	int size = 0;
	char *line = NULL;
	line = read(file, '\n');
	while(line[0] > 0)
	{
		free(line);
		line = NULL;
		size = size + 1;
		line = read(file, '\n');
	}
	free(line);
	rewind(file);
	return size;
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMat(unsigned char **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

/* 
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMat(unsigned char **mat, int c, int r)
{
    int i, j;
    for(i = 0; i < r; i++)
    {
		for(j = 0; j < c; j++)
        {
        	if(j+1 == c)
        	{
        		printf("%c", mat[i][j]);	
        	}
        	else
        	{
        		printf("%c ", mat[i][j]);
        	}
        }
        printf("\n");
    }
}

/*
	Dynamically allocates matrix
	params:
	   - c: number of columns;
	   - r: number of rows
	returns:
	   - allocated matrix
*/
unsigned char **allocMat(int c, int r)
{
	int i, j;
	unsigned char **board = NULL;
	board = (unsigned char**)calloc(r, sizeof(unsigned char*));
	for(i = 0; i < r; i++)
	{
		board[i] = (unsigned char*)calloc(c, sizeof(unsigned char));
		for(j = 0; j < c; j++)
		{
			board[i][j] = '0';
		}
	}
	return board;
}

/*
	Calculate hamming distance
	params:
	   - a: first element;
	   - b: second element;
	returns:
	   - the difference between both elements
*/
int hammingDistance(int a, int b)
{
	return abs(a-b);
}

/*
	Calculate hamming distance between two images
*/
void memDist()
{
	int i, j, dist = 0, r, c;
	unsigned char **img1 = NULL, **img2 = NULL;
	char character;
	fscanf(stdin, "%d %d\n", &r, &c);
	img1 = allocMat(c, r);
	img2 = allocMat(c, r);
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++)
		{
			fscanf(stdin, "%c", &character);
			img1[i][j] = character;
		}
	}
	fscanf(stdin, "\n");
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++)
		{
			fscanf(stdin, "%c", &character);
			img2[i][j] = character;
		}
	}
	/* Calculate hamming distance */
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++)
		{
			dist = dist + hammingDistance((int)img1[i][j], (int)img2[i][j]);
		}
	}
	/* Print dist */
	printf("dist = %d\n", dist);
	freeMat(img1, r);
	freeMat(img2, r);
}

/*
	Perform dilate operation
	params:
	   - A: matrix A;
	   - c1: matrix A number of columns;
	   - r1: matrix A number of rows;
	   - B: matrix B;
	   - c2: matrix B number of columns;
	   - r2: matrix B number of rows;
	returns:
	   - image containing dilate operation
*/
unsigned char **dilate(unsigned char **A, int c1, int r1, unsigned char **B, int c2, int r2)
{
	int i, j, k, l, xCenter, yCenter;
	unsigned char **C;
	C = allocMat(c1, r1);
	/* Get center of structuring element */
	xCenter = (r2/2);
	yCenter = (c2/2);
	/* Check every pixel of image A */
	for(i = xCenter; i <= (r1-(xCenter+1)); i++)
	{
		for(j = yCenter; j <= (c1-(yCenter+1)); j++)
		{
			if(A[i][j] == '1')
			{
				/* "Stamp" structuring element over C */
				for(k = 0; k < r2; k++)
				{
					for(l = 0; l < c2; l++)
					{
						if(C[i+(k-(r2/2))][j+(l-(c2/2))] != '1')
						{
							C[i+(k-(r2/2))][j+(l-(c2/2))] = B[k][l];
						}
					}
				}
			}
		}
	}
	return C;
}

/*
	Count number of one pixels of image A
	params:
	   - A: matrix A containing image;
	   - c1: matrix A number of columns;
	   - r1: matrix A number of rows;
	returns:
	   - counter of number of 1 pixels
*/
int countOnePixels(unsigned char **A, int c1, int r1)
{
	int i, j, count = 0;
	for(i = 0; i < r1; i++)
	{
		for(j = 0; j < c1; j++)
		{
			if(A[i][j] == '1')
			{
				count = count + 1;
			}
		}
	}
	return count;
}

/*
	Perform erode operation
	params:
	   - A: matrix A;
	   - c1: matrix A number of columns;
	   - r1: matrix A number of rows;
	   - B: matrix B;
	   - c2: matrix B number of columns;
	   - r2: matrix B number of rows;
	returns:
	   - image containing erode operation
*/
unsigned char **erode(unsigned char **A, int c1, int r1, unsigned char **B, int c2, int r2)
{
	int i, j, k, l, xCenter, yCenter, onePixelsCount = 0, count;
	unsigned char **C;
	C = allocMat(c1, r1);
	/* Get center of structuring element */
	xCenter = (r2/2);
	yCenter = (c2/2);
	/* Get structuring element 1 pixels count */
	onePixelsCount = countOnePixels(B, c2, r2);
	/* Check every pixel of image A */
	for(i = xCenter; i <= (r1-(xCenter+1)); i++)
	{
		for(j = yCenter; j <= (c1-(yCenter+1)); j++)
		{
			count = 0;
			for(k = 0; k < r2; k++)
			{
				for(l = 0; l < c2; l++)
				{
					if(A[i+(k-(r2/2))][j+(l-(c2/2))] == '1' && B[k][l] == '1')
					{
						count = count + 1;
					}
				}
			}
			/* "Stamp" structuring element over C */
			if(count == onePixelsCount)
			{
				C[i][j] = '1';
			}
			else
			{
				C[i][j] = '0';
			}
		}
	}
	return C;
}

/*
	Perform open operation
	params:
	   - A: matrix A;
	   - c1: matrix A number of columns;
	   - r1: matrix A number of rows;
	   - B: matrix B;
	   - c2: matrix B number of columns;
	   - r2: matrix B number of rows;
	returns:
	   - image containing combination of erode and dilate operations
*/
unsigned char **ocrOpen(unsigned char **A, int c1, int r1, unsigned char **B, int c2, int r2)
{
	unsigned char **C = NULL;
	C = erode(A, c1, r1, B, c2, r2);
	return dilate(C, c1, r1, B, c2, r2);
}

/*
	Perform close operation
	params:
	   - A: matrix A;
	   - c1: matrix A number of columns;
	   - r1: matrix A number of rows;
	   - B: matrix B;
	   - c2: matrix B number of columns;
	   - r2: matrix B number of rows;
	returns:
	   - image containing combination of dilate and erode operations
*/
unsigned char **ocrClose(unsigned char **A, int c1, int r1, unsigned char **B, int c2, int r2)
{
	unsigned char **C = NULL;
	C = dilate(A, c1, r1, B, c2, r2);
	return erode(C, c1, r1, B, c2, r2);
}

/*
	Print according to "mem_op" command specification
	params:
	   - img1: image 1;
	   - c1: image 1 number of columns;
	   - r1: image 1 number of rows;
	   - img2: image 2;
	   - c2: image 2 number of columns;
	   - r2: image 2 number of rows;
	   - img3: image 3
*/
void printMemOp(unsigned char **img1, int c1, int r1, unsigned char **img2, int c2, int r2, unsigned char **img3)
{
	printf("im:\n");
	printMat(img1, c1, r1);
	printf("el:\n");
	printMat(img2, c2, r2);
	printf("out:\n");
	printMat(img3, c1, r1);
}

/*
	Allocate matrices, read morphological operation and execute
	params:
	   - img1: matrix containing image;
	   - c1: img1 number of columns;
	   - r1: img1 number of rows;
	   - img2: matrix containing structuring element;
	   - c2: img2 number of columns;
	   - r2: img2 number of rows;
	returns:
	   - image resulting from morphological operation between img1 and img2
*/
unsigned char **loadMemOp(unsigned char ***img1, int *c1, int *r1, unsigned char ***img2, int *c2, int *r2)
{
	int i, j;
	unsigned char **img3 = NULL;
	char character, *operation;

	/* Read image */
	fscanf(stdin, "%d %d\n", &(*r1), &(*c1));
	(*img1) = allocMat((*c1), (*r1));
	for(i = 0; i < (*r1); i++)
	{
		for(j = 0; j < (*c1); j++)
		{
			fscanf(stdin, "%c", &character);
			(*img1)[i][j] = character;
		}
	}
	fscanf(stdin, "\n");
	/* Read structuring element */
	fscanf(stdin, "%d %d\n", &(*r2), &(*c2));
	(*img2) = allocMat((*c2), (*r2));
	for(i = 0; i < (*r2); i++)
	{
		for(j = 0; j < (*c2); j++)
		{
			fscanf(stdin, "%c", &character);
			(*img2)[i][j] = character;
		}
	}
	fscanf(stdin, "\n");
	/* Read morphological operation */
	operation = read(stdin, '\n');
	if(strcmp(operation, "dilate") == 0)
	{
		img3 = dilate((*img1), (*c1), (*r1), (*img2), (*c2), (*r2));
	}
	else if(strcmp(operation, "erode") == 0)
	{
		img3 = erode((*img1), (*c1), (*r1), (*img2), (*c2), (*r2));	
	}
	else if(strcmp(operation, "open") == 0)
	{
		img3 = ocrOpen((*img1), (*c1), (*r1), (*img2), (*c2), (*r2));
	}
	else if(strcmp(operation, "close") == 0)
	{
		img3 = ocrClose((*img1), (*c1), (*r1), (*img2), (*c2), (*r2));
	}
	return img3;
}

/*
	Calculate morphological operation
*/
void memOp()
{
	int c1, r1, c2, r2;
	unsigned char **img1 = NULL, **img2 = NULL, **img3 = NULL;
	img3 = loadMemOp(&img1, &c1, &r1, &img2, &c2, &r2);
	/* Print output */
	printMemOp(img1, c1, r1, img2, c2, r2, img3);
}

void initSchema(schema *s, FILE *fSchema)
{
	char *esquema = NULL, *tableName = NULL;
	/* Count number of attributes; -1 given table is not an attribute */
	s->attrSize = countLines(fSchema) - 1;
	/* Allocate names and types matrices */
	s->names = (char**)malloc(sizeof(char*)*(s->attrSize));
	s->types = (char**)malloc(sizeof(char*)*(s->attrSize));
	/* Read ".schema" first line to store table name */
	esquema = read(fSchema, '\n');
	tableName = split(esquema, ' ', 1);
	s->tableName = (char*)calloc(strlen(tableName)+1, sizeof(char));
	strcat(s->tableName, tableName);
	s->table = (char*)calloc(strlen(tableName)+6, sizeof(char));
	strcat(s->table, tableName);
	strcat(s->table, ".data");
}

/*
	Read both name and type from ".schema" file
	params:
	   - file: file to be read. first line already skipped;
	   - names: matrix of strings which will contain attribute names;
	   - types: matrix of strings which will contain attribute types;
	   - attrSize: number of attributes, consequently being number of matrices rows
*/
void readNameAndType(FILE *file, char **names, char **types, int attrSize)
{
	int i;
	char *str;
	for(i = 0; i < attrSize; i++)
	{
		str = NULL;
		str = read(file, ' ');
		names[i] = NULL;
		names[i] = (char*)realloc(names[i], sizeof(char)*(strlen(str)+1));
		strcpy(names[i], str);
		free(str);
		str = NULL;
		str = read(file, '\n');
		types[i] = NULL;
		types[i] = (char*)realloc(types[i], sizeof(char)*(strlen(str)+1));
		strcpy(types[i], str);
		free(str);
	}
}

/*
	Converts a null terminating string to double
	param:
	   - number: char type number to be converted
	returns:
	   - double type number
*/
double toDouble(char *number)
{
	int i, point = 0, exponent;
	double num = 0.00000;
	for(i = 0; i < strlen(number); i++)
	{
		if(number[i] == '.')
		{
			point = 1;
			exponent = 1;
			i = i + 1;
		}
		if(point == 0)
		{
			num = ((num)*10) + (number[i]-48);
		}
		else
		{
			num = num + ((double)(number[i]-48)/(10*exponent));
			exponent = exponent * 10;
		}
	}
	return num;
}

/*
	Convert null terminating string to pointer to double
	param: 
	   - number: char type number to be converted
	returns:
	   - double * type number
*/
double *writeToDouble(char *number)
{
	double *dValue;
	dValue = (double*)malloc(sizeof(double));
	*dValue = toDouble(number);
	return dValue;
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
	Convert null terminating string to pointer to integer
	param: 
	   - number: char type number to be converted
	returns:
	   - int * type number
*/
int *writeToNum(char *number)
{
	int *integer;
	integer = (int*)malloc(sizeof(int));
	*integer = toNum(number);
	return integer;
}

/*
	Get size from a string containing type
	param:
	   - type: string containing type
	returns:
	   - size from the string type, in bytes
*/
int getSize(char *type)
{
	int num;
	char *number, *number2;
	if(strcmp(type, "char") == 0)
	{
		return 1;
	}
	else if(strcmp(type, "int") == 0)
	{
		return 4;
	}
	else if(strcmp(type, "double") == 0)
	{
		return 8;
	}
	else if(strstr(type, "["))
	{
		number = NULL;
		number2 = NULL;
		number = split(type, '[', 1);
		number2 = split(number, ']', 0);
		num = toNum(number2);
		free(number);
		free(number2);
		return num;
	}
	else
	{
		return 0;
	}
}

/*
	Dump ".data" file
	params:
	   - file: file (already opened) pointer to ".data";
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - names: attributes names;
	   - types: attributes types;
	   - dataSize: number of entries to read
*/
void dumpData(FILE *file, int attrSize, char **names, char **types, int dataSize)
{
	int i, j, size = 0, *integer = NULL;
	double *doubleValue = NULL;
	char *str = NULL;
	// printf("dataSize: %d\n", dataSize);
	for(i = 0; i < dataSize; i++)
	{
		for(j = 0; j < (attrSize+2); j++)
		{
			/* id */
			if(j % (attrSize+2) == 0)
			{
				integer = (int*)malloc(sizeof(int));
				fread(integer, sizeof(int), 1, file);
				printf("id = %d\n", (*integer));
				free(integer);
			}
			/* dist */
			else if(j % (attrSize+1) == 0)
			{
				doubleValue = (double*)malloc(sizeof(double));
				fread(doubleValue, sizeof(double), 1, file);
				printf("dist = %.2lf\n", (*doubleValue));
				free(doubleValue);
			}
			else
			{
				size = getSize(types[j-1]);
				if(strcmp(types[j-1], "int") == 0)
				{
					integer = (int*)malloc(sizeof(int));
					fread(integer, sizeof(int), 1, file);
					printf("%s = %d\n", names[j-1], (*integer));
					free(integer);
				}
				else if(strstr(types[j-1], "["))
				{
					if(strstr(types[j-1], "byte"))
					{
						str = (char*)malloc(sizeof(char)*(size*8));
						fread(str, sizeof(char), (size*8), file);
					}
					else
					{
						str = (char*)malloc(sizeof(char)*(size));
						fread(str, sizeof(char), (size), file);
					}
					printf("%s = %s\n", names[j-1], str);
					free(str);
				}
			}
		}
	}
}

/*
	Read training test input from stdin
	params:
	   - file: file to be written
	   - types: matrix of strings containing attribute types;
	   - attrSize: number of attributes, consequently being number of matrix rows
*/
void readTrainingTest(FILE *file, char **types, int attrSize)
{
	int i = 0, *integer, count = 0, num;
	double *doubleValue;
	char *line = NULL;
	line = read(stdin, ' ');
	while(strcmp(line, "-1") != 0)
	{
		/* (attrSize+1) because of id */
		if(count % (attrSize+2) == 0)
		{
			/* Read id first */
			count = 0;
			i = 0;
			integer = writeToNum(line);
			fwrite(integer, sizeof(int), 1, file);
			free(integer);
			free(line);
			line = NULL;
			line = read(stdin, ' ');
		}
		/* Read dist last */
		else if(count % (attrSize+1) == 0)
		{
			doubleValue = (double*)malloc(sizeof(double));
			*doubleValue = 0.00;
			fwrite(doubleValue, sizeof(double), 1, file);
			free(doubleValue);
		}
		/* Read rest of types */
		else if(strstr(types[i], "["))
		{
			num = getSize(types[i]);
			line = realloc(line, sizeof(char)*((strlen(line)+1)));
			if(strstr(types[i], "byte"))
			{
				fwrite(line, sizeof(char), (num*8), file);
			}
			else
			{
				fwrite(line, sizeof(char), (num), file);
			}
			i = i + 1;
			free(line);
			line = NULL;
			line = read(stdin, ' ');
		}
		else if(strcmp(types[i], "int") == 0)
		{
			integer = NULL;
			integer = writeToNum(line);
			fwrite(integer, sizeof(int), 1, file);
			free(integer);
			i = i + 1;
			free(line);
			line = NULL;
			line = read(stdin, ' ');
		}
		count = count + 1;
	}
	/* Put last entry dist */
	if(count != 0)
	{
		doubleValue = (double*)malloc(sizeof(double));
		*doubleValue = 0.00;
		fwrite(doubleValue, sizeof(double), 1, file);
		free(doubleValue);
	}
	free(line);
}

/*
	Get schema total size
	params:
	   - types: attributes types;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - dumpSchema: (1) if caller was "dumpSchema" function, (0) otherwise
	returns:
	   - sum of sizes, in bytes
*/
int getSchemaSize(char **types, int attrSize, int dumpSchema)
{
	int i, size = 0;
	for(i = 0; i < attrSize; i++)
	{
		if(strstr(types[i], "byte") && dumpSchema == 0)
		{
			size = size + (getSize(types[i])*8);
		}
		else
		{
			size = size + getSize(types[i]);
		}
	}
	/* Summing "id" and "dist" */
	size = size + 4;
	size = size + 8;
	return size;
}

/*
	Dump ".schema" file, with additional "id" and "dist" information
	params:
	   - table: table name;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - names: attributes names;
	   - types: attributes types
*/
void dumpSchema(char *table, int attrSize, char **names, char **types)
{
	int i, byteCount = 0;
	byteCount = getSchemaSize(types, attrSize, 1);
	printf("table %s(%d bytes)\n", table, byteCount);
	/* attrSize+2 because of id and dist */
	for(i = 0; i < (attrSize+2); i++)
	{
		/* id */
		if(i == 0)
		{
			printf("id int(4 bytes)\n");
		}
		/* dist */
		else if(i == (attrSize+1))
		{
			printf("dist double(8 bytes)\n");
		}
		else
		{
			printf("%s %s(%d bytes)\n", names[i-1], types[i-1], getSize(types[i-1]));
		}
	}
}

/*
	Read entry from file
	params:
	   - file: file to be read;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - types: attributes types;
	   - attrInt: vector of type int, to store attributes;
	   - j: attrInt size;
	   - kValue: k-neighbors for "knn"
*/
void readEntry(FILE *file, int attrSize, char **types, int **attrInt, int *j, int *kValue)
{
	int i /*, size*/;
	for(i = 0, (*j) = 0; i < (attrSize); i++)
	{
		if(strcmp(types[i], "int") == 0)
		{
			(*attrInt) = (int*)realloc((*attrInt), sizeof(int)*((*j)+1));
			fscanf(file, "%d", &(*attrInt)[(*j)]);
			// fread((*attrInt), sizeof(int), 1, file);
			(*j) = (*j) + 1;
		}
		else if(strstr(types[i], "["))
		{

		}
	}
	/* k-neighbors */
	fscanf(file, "%d\n", &(*kValue));
}

/*
	Free variables for next entry
	params:
	   - binaryId: id of entry;
	   - binaryInt: int attributes of entry;
	   - binaryIntSize: number of int attributes of entry;
	   - binaryChar: char *attributes of entry;
	   - binaryCharSize: number of char * attributes of entry;
	   - binaryDist: dist of entry
*/
void freeVariables(int *binaryId, int **binaryInt, int *binaryIntSize, char ***binaryChar, int *binaryCharSize, double *binaryDist)
{
	(*binaryId) = 0;
	free((*binaryInt));
	(*binaryInt) = NULL;
	(*binaryIntSize) = 0;
	free((*binaryChar));
	(*binaryChar) = NULL;
	(*binaryCharSize) = 0;
	(*binaryDist) = 0.00;
}	

/*
	Read entry from ".data" file
	params:
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - types: attributes types;
	   - binaryId: id of entry;
	   - binaryInt: int type attributes of entry;
	   - binaryIntSize: number of int entries;
	   - binaryChar: char * type attributes of entry;
	   - binaryCharSize: number of char * entries;
	   - binaryDist: dist of entry
*/
void readBinaryEntry(FILE *file, int attrSize, char **types, int *binaryId, int **binaryInt, int *binaryIntSize,
	char ***binaryChar, int *binaryCharSize, double *binaryDist)
{
	int i, size = 0;
	for(i = 0; i < (attrSize+2); i++)
	{
		/* id */
		if(i == 0)
		{
			fread(binaryId, sizeof(int), 1, file);
		}
		/* dist */
		else if(i == (attrSize+1))
		{
			fread(binaryDist, sizeof(double), 1, file);
		}
		else
		{
			size = getSize(types[i-1]);
			if(strcmp(types[i-1], "int") == 0)
			{
				(*binaryInt) = (int*)realloc((*binaryInt), sizeof(int)*((*binaryIntSize)+1));
				fread(&(*binaryInt)[(*binaryIntSize)], sizeof(int), 1, file);
				(*binaryIntSize) = (*binaryIntSize) + 1;
			}
			else if(strstr(types[i-1], "["))
			{
				(*binaryChar) = (char**)realloc((*binaryChar), sizeof(char*)*((*binaryCharSize)+1));
				(*binaryChar)[(*binaryCharSize)] = NULL;
				if(strstr(types[i-1], "byte"))
				{
					(*binaryChar)[(*binaryCharSize)] = (char*)realloc((*binaryChar)[(*binaryCharSize)], sizeof(char)*(size*8));
					fread((*binaryChar)[(*binaryCharSize)], sizeof(char), (size*8), file);
				}
				else
				{
					(*binaryChar)[(*binaryCharSize)] = (char*)realloc((*binaryChar)[(*binaryCharSize)], sizeof(char)*(size));
					fread((*binaryChar)[(*binaryCharSize)], sizeof(char), (size), file);
				}
				(*binaryCharSize) = (*binaryCharSize) + 1;
			}
		}
	}
}

/*
	Find k-nearest neighbors
	params:
	   - kValue: k;
	   - distances: distances computed;
       - dataSize: number of elements;
	returns:
	   - indexes containing nearest neighbors
*/
int *kNearestNeighbors(int kValue, dist *distances, int dataSize)
{
	int i, *nearestNeighbors;
	nearestNeighbors = (int*)malloc(sizeof(int)*(kValue));
	qsort(distances, dataSize, sizeof(dist), cmp);
	for(i = 0; i < kValue; i++)
	{
		nearestNeighbors[i] = distances[i].index;
	}
	return nearestNeighbors;
}

/*
	Retrieve k closest classes
	params:
	   - file: ".data" file;
	   - kValue: k;
	   - distances: sorted array of closest elements to new element;
	   - types: attributes types;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	returns:
	   - classes
*/
char **retrieveClasses(FILE *file, int kValue, dist *distances, char **types, int attrSize)
{
	int i, binaryId = 0, *binaryInt = NULL, binaryIntSize = 0, binaryCharSize = 0;
	double binaryDist = 0.00;
	char **binaryChar = NULL, **classes = NULL;
	classes = (char**)malloc(sizeof(char*)*kValue);
	for(i = 0; i < kValue; i++)
	{
		rewind(file);
		fseek(file, (distances[i].index*getSchemaSize(types, attrSize, 0)), SEEK_SET);
		readBinaryEntry(file, attrSize, types, &binaryId, &binaryInt, &binaryIntSize, &binaryChar, &binaryCharSize, &binaryDist);
		classes[i] = (char*)malloc(sizeof(char)*(strlen(binaryChar[binaryCharSize-1])+1));
		strcpy(classes[i], binaryChar[binaryCharSize-1]);
		freeVariables(&binaryId, &binaryInt, &binaryIntSize, &binaryChar, &binaryCharSize, &binaryDist);
	}
	return classes;
}

/*
	Check closest indexes classes and assigns according to kValue
	params:
	   - file: ".data" file
	   - kValue: k;
	   - distances: sorted array of closest elements to new element;
	   - types: attributes types;
	   - attrSize: number of attributes, consequently being number of matrices rows;
*/
char *classify(FILE *file, int kValue, dist *distances, char **types, int attrSize)
{
	int i, j, k, *counts, *offsets, max = -1, maxIndex = 0;
	char **classes = NULL, *actualClass = NULL;
	classes = retrieveClasses(file, kValue, distances, types, attrSize);
	/* Count classes */
	counts = (int*)calloc(kValue, sizeof(int));
	offsets = (int*)calloc(kValue, sizeof(int));
	qsort(classes, kValue, sizeof(char*), cmp2);
	for(i = 0, j = -1, k = 0; i < kValue;)
	{
		if(!actualClass)
		{
			actualClass = (char*)malloc(sizeof(char)*(strlen(classes[i])+1));
			strcpy(actualClass, classes[i]);
			j = j + 1;
			offsets[k] = i;
			k = k + 1;
		}
		if(actualClass && strcmp(actualClass, classes[i]) == 0)
		{
			counts[j] = counts[j] + 1;
			i = i + 1;
		}
		else
		{
			free(actualClass);
			actualClass = NULL;
		}
	}
	for(i = 0; i < kValue; i++)
	{
		if(counts[i] > max)
		{
			maxIndex = i;
			max = counts[i];
		}
	}
	// printf("classes[offsets[maxIndex]]: %s\n", classes[offsets[maxIndex]]);
	return classes[offsets[maxIndex]];
}

/*
	Get ".data" file number of entries
	params:
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - types: attributes types
	returns:
	   - size, in number of entries, of ".data" file
*/
int getDataSize(FILE *file, int attrSize, char **types)
{
	int dataSize = 0;
	/* Get file size of bytes */
	fseek(file, 0, SEEK_END);
	dataSize = ftell(file);
	dataSize = (int)ceil((double)dataSize/getSchemaSize(types, attrSize, 0));
	rewind(file);
	return dataSize;
}

/*
	Update distance "dist" in entry
	params:
	   - file: ".data" file;
	   - newDist: new distance to be written
*/
void updateEntry(FILE *file, double *newDist)
{
	fwrite(newDist, sizeof(double), 1, file);
}

/*
	Transform matrix into vector
	params:
	   - img: image matrix;
	   - c1: matrix number of columns;
	   - r1: matrix number of rows
	returns:
	   - vector from matrix
*/
unsigned char *matToVec(unsigned char **img, int c1, int r1)
{
	int i, j, k;
	unsigned char *vec = NULL;
	vec = (unsigned char*)calloc((c1*r1), sizeof(unsigned char));
	for(i = 0, k = 0; i < r1; i++)
	{
		for(j = 0; j < c1; j++, k++)
		{
			vec[k] = img[i][j];
		}
	}
	return vec;
}

/*
	Execute knn classification, using hamming distance
	params:
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - names: attributes names;
	   - types: attributes types;
	   - nearestNeighbors: vector containing k-nearest neighbors;
	   - kValue: k;
	   - print: (1) if memOp type print is required, (0) otherwise
	returns:
	   - classified class
*/
char *ocrKnn(FILE *file, int attrSize, char **names, char **types, int **nearestNeighbors, int *kValue, int print)
{
	int i, j, c1, r1, c2, r2, dataSize, binaryId = 0, *binaryInt = NULL, binaryIntSize = 0, binaryCharSize = 0;
	char **binaryChar = NULL;
	double binaryDist;
	dist *distances = NULL;
	unsigned char **img1 = NULL, **img2 = NULL, **img3 = NULL, *img3Vect = NULL;
	
	/* Get entry */
	img3 = loadMemOp(&img1, &c1, &r1, &img2, &c2, &r2);
	if(print)
	{
		/* Print output */
		printMemOp(img1, c1, r1, img2, c2, r2, img3);
	}
	/* Turn matrix to vector */
	img3Vect = matToVec(img3, c1, r1);
	/* Get neighbors */
	fscanf(stdin, "%d\n", &(*kValue));


	/* Get data number of entries */
	dataSize = getDataSize(file, attrSize, types);

	distances = (dist*)calloc(dataSize, sizeof(dist));
	for(i = 0; i < dataSize; i++)
	{
		readBinaryEntry(file, attrSize, types, &binaryId, &binaryInt, &binaryIntSize, &binaryChar, &binaryCharSize, &binaryDist);
		init(&distances[i], i);
		/* Calculate hamming distance between images */
		for(j = 0; j < (r1*c1); j++)
		{
			distances[i].distance = distances[i].distance + hammingDistance(img3Vect[j], binaryChar[FIRST][j]);
		}
		/*for(j = 0; j < r1; j++)
		{
			for(k = 0; k < c1; k++)
			{
				distances[i].distance = distances[i].distance + hammingDistance(img3[j][k], binaryChar[j][k]);
				printf("distances[i].distance: %lf\n", distances[i].distance);
			}
		}*/
		/* Update dist in binary file */
		fseek(file, -8, SEEK_CUR);
		updateEntry(file, &distances[i].distance);
		freeVariables(&binaryId, &binaryInt, &binaryIntSize, &binaryChar, &binaryCharSize, &binaryDist);
	}
	/* Find k-nearest neighbors */
	(*nearestNeighbors) = realloc((*nearestNeighbors), sizeof(int)*((*kValue)));
	(*nearestNeighbors) = kNearestNeighbors((*kValue), distances, dataSize);
	// attrId, attrInt, intSize, attrDouble, doubleSize, &class
	return classify(file, (*kValue), distances, types, attrSize);
}

/*
	Perform morphological operation froom given image and structuring element, then 
	call knn classification and find matrix containing closest images
	params:
	   - table: ".data" file name, including extension;
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - names: attributes names;
	   - types: attributes types;
	   - nearestNeighbors: vector containing k-nearest neighbors;
*/
void ocrDumpNn(char *table, FILE *file, int attrSize, char **names, char **types, int **nearestNeighbors)
{
	/* Execute knn */
	int i, j, k, kValue = 0, binaryId = 0, *binaryInt = NULL, binaryIntSize = 0, binaryCharSize = 0;
	double binaryDist = 0.00;
	char **binaryChar = NULL;
	ocrKnn(file, attrSize, names, types, nearestNeighbors, &kValue, 1);
	for(i = 0; i < kValue; i++)
	{
		rewind(file);
		fseek(file, ((*nearestNeighbors)[i]*getSchemaSize(types, attrSize, 0)), SEEK_SET);
		readBinaryEntry(file, attrSize, types, &binaryId, &binaryInt, &binaryIntSize, &binaryChar, &binaryCharSize, &binaryDist);
		/* Print nearest neighbors */
		printf("id = %d\n", binaryId);
		for(j = 0, k = 0; j < attrSize; j++)
		{
			if(strcmp(types[j], "int") == 0)
			{
				printf("%s = %d\n", names[j], binaryInt[k]);
				k = k + 1;
			}
			else if(strstr(types[j], "["))
			{
				if(strstr(types[j], "byte"))
				{
					printf("%s = %s\n", names[j], binaryChar[FIRST]);
				}
				else
				{
					printf("%s = %s\n", names[j], binaryChar[binaryCharSize-1]);
				}
			}
		}
		printf("dist = %.2lf\n", binaryDist);
		freeVariables(&binaryId, &binaryInt, &binaryIntSize, &binaryChar, &binaryCharSize, &binaryDist);
		// printf("nearestNeighbors[%d]: %d\n", i, (*nearestNeighbors)[i]);
	}
}

int main(void)
{
	int kValue = 0;
	char *schemaName = NULL, *command = NULL;
	FILE *fSchema, *fData;
	schema *s;
	/* Step 1: Read .schema or "none" */
	schemaName = read(stdin, '\n');
	if(strcmp(schemaName, "none") == 0)
	{
		/* Step 2: Read command */
		command = read(stdin, '\n');
		while(strcmp(command, "exit") != 0)
		{
			/* mem_dist command */
			if(strcmp(command, "mem_dist") == 0)
			{
				memDist();
			}
			/* mem_op command */
			else if(strcmp(command, "mem_op") == 0)
			{
				memOp();
			}
			free(command);
			command = NULL;
			command = read(stdin, '\n');
		}
	}
	else
	{
		fSchema = fopen(schemaName, "r");
		if(fSchema)
		{
			s = (schema*)calloc(1, sizeof(schema));
			/* Step 2: Initialize schema struct */
			initSchema(s, fSchema);
			/* Step 3: Read names and types */
			readNameAndType(fSchema, s->names, s->types, s->attrSize);
			/* Step 4: Create ".data" file */
			fData = fopen(s->table, "wb");
			if(fData)
			{
				readTrainingTest(fData, s->types, s->attrSize);
			}
			else
			{
				printf("Failed to write %s file.\n", s->tableName);
			}
			fclose(fData);
		}
		else
		{
			printf("File not found.\n");
		}
		/* Step 5: Read command */
		command = read(stdin, '\n');
		while(strcmp(command, "exit") != 0)
		{
			/* dump_schema command */
			if(strcmp(command, "dump_schema") == 0)
			{
				dumpSchema(s->tableName, s->attrSize, s->names, s->types);
			}
			/* dump_data command */
			if(strcmp(command, "dump_data") == 0)
			{
				fData = fopen(s->table, "rb+");
				dumpData(fData, s->attrSize, s->names, s->types, getDataSize(fData, s->attrSize, s->types));
				fclose(fData);
			}
			/* ocr_dump_nn command */
			if(strcmp(command, "ocr_dump_nn") == 0)
			{
				fData = fopen(s->table, "rb+");
				ocrDumpNn(s->table, fData, s->attrSize, s->names, s->types, &s->nearestNeighbors);
				fclose(fData);
			}
			/* ocr_knn */
			if(strcmp(command, "ocr_knn") == 0)
			{
				fData = fopen(s->table, "rb+");
				printf("%s\n", ocrKnn(fData, s->attrSize, s->names, s->types, &s->nearestNeighbors, &kValue, 0));
				fclose(fData);
			}
			free(command);
			command = NULL;
			command = read(stdin, '\n');
		}
	}
	free(schemaName);
	free(command);
	return 0;
}