/*
	Program that reads .dat metadata, describing a markup for .reg files. Also creates .idx index file, which can be used
	with binary search to obtain entries in .reg file.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 19/04/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	Struct of a pseudo vector for register, containing key and offset
	key: register's entry key
	offset: register's entry offset
*/
typedef struct _tVector{
	int key;
	int offset;
}tVector;
/* Typedef annotation for simpler pointer parameter management */
typedef tVector *Vector;

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
	if((*(tVector*)a).key < (*(tVector*)b).key) 
	{
		return -1;
	}
	else if((*(tVector*)a).key == (*(tVector*)b).key) 
	{
		return 0;
	}
	else
	{ 
		return 1;
	}
}

/*
	Simple print method for created struct
	params:
	   - data: struct to be printed
	   - size: struct vector's size
*/
void printStruct(Vector data, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		printf("tVector at position %d\nkey:%d\toffset:%d\n", i, data[i].key, data[i].offset);
	}
}

/*
	Sums a vector
	params:
	   - vector: int vector to be summed;
	   - size: vector size
	returns:
	   - sum of elements
*/
int sumVector(int *vector, int size)
{
	int i, sum;
	sum = 0;
	for(i = 0; i < size; i++)
	{
		sum = sum + vector[i];
	}
	return sum;
}

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
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

/*
	Print a int vector
	param:
	   - vector: array to be printed
	   - size: array size
*/
void printIntVector(int *vector, int size)
{
	int i;
	printf("Vector size: %d\n", size);
	for(i = 0; i < size; i++)
	{
		printf("%d ", vector[i]);
	}
	printf("\n");
}

/*
	Print a string vector
	param:
	   - vector: array to be printed
	   - size: array size
*/
void printStrVector(char *vector, int size)
{
	int i;
	printf("Vector size: %d\n", size);
	for(i = 0; i < size; i++)
	{
		printf("%c\n", vector[i]);
	}
}

/*
	Print a string vector
	param:
	   - vector: array to be printed
	   - size: array size
*/
void printStrMatrix(char **vector, int size)
{
	int i;
	printf("Vector size: %d\n", size);
	for(i = 0; i < size; i++)
	{
		printf("%s ", vector[i]);
	}
	printf("\n");
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
	Read str parameter
	param:
	   - file: file to be read
	returns:
	   - string
*/
char *readParam(FILE *file)
{
	int count;
    char c;
    count = 0;
    char *line = NULL;
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != ',' && c != EOF);
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
	Find line offset
	params:
	   - string: string to be scanned
	   - substr: substring to be found
	returns:
	   - offset from starting substring point
*/
char *findOffset(char *string, char *substr)
{
	char *offset;
	offset = NULL;
	offset = strstr(string, substr);
	return offset;
}

/*
	Read ".reg" extension file name
	param:
	   - file: file to be read
	returns:
	   - ".reg" extension file name
*/
char *readRegFileName(FILE *file)
{
	char *regFileName, *regFileName2, *offset, c;
	int i, j;
	regFileName = NULL;
	regFileName = readLine(file);

	/* Find fileName offset */
	offset = NULL;
	offset = findOffset(regFileName, ":");

	/* Copies string, removing ":" from beggining */
	i = 0;
	j = 2;
	regFileName2 = NULL;
	do{
		c = offset[j];
		regFileName2 = (char*)realloc(regFileName2, sizeof(char)*(i+1));
		regFileName2[i++] = c;
		j = j + 1;
	}while(c != '\0');
	regFileName2[i-1] = '\0';
	// printf("%s\n", regFileName2);
	free(regFileName);
	return regFileName2;
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
		// i = i + 1;
	}
	return num;
}

/*
	Initialize given tVector position
	params:
       - t: type read;
       - k: position to be initialized;
       - types: matrix of types read;
	   - bytes: byte sizes;
*/
void init(char *t, int k, char **types, int *bytes)
{
	char *aux;
	int aux2;
	aux = NULL;
	aux2 = 0;

	/* Initialize type */
	/* If type is a char with a given static size, it must be fixed */
	if(findOffset(t, "[") != NULL)
	{
		char *number, c;
		int i;
		number = findOffset(t, "[");
		i = 1;
		c = number[0];
		while(c != ']')
		{
			aux2 = ((aux2)*10) + (number[i]-48);
			i = i + 1;
			c = number[i];
		}
		aux = (char*)malloc(sizeof(char)*(strlen(t)+1));
		strcpy(aux, "char");	
	}
	else
	{
		aux = (char*)malloc(sizeof(char)*(strlen(t)+1));
		strcpy(aux, t);

		/* Assign bytes' correct size */
		if(strcmp(aux, "int") == 0)
		{
			aux2 = 4;	
		}
		else if(strcmp(aux, "double") == 0)
		{
			aux2 = 8;	
		}
		else if(strcmp(aux, "float") == 0)
		{
			aux2 = 4;	
		}
		else if(strcmp(aux, "char") == 0)
		{
			aux2 = 1;
		}
	}

	/* Assign types and bytes variables with corresponding elements */
	types[k] = (char*)malloc(sizeof(char)*(strlen(aux)+1));
	strcpy(types[k], aux);
	bytes[k] = aux2;
	free(aux);
}

/*
	Read both name and type from ".dat" file
	param:
	   - file: file to be read;
	   - types: matrix of types read;
	   - bytes: byte sizes;
	   - k: position.
	returns:
	   - EOF control signal
*/
int readNameAndType(FILE *file, char **types, int *bytes, int k)
{
	char *fieldType, *offset, *fieldType2, c;
	int i, j, signal = 1;
	fieldType = NULL;

	/* Read field name. This line is redundant so it's skipped */
	fieldType = readLine(file);
	free(fieldType);
	fieldType = NULL;
	fieldType = readLine(file);
	if(strcmp(fieldType, "-1") != 0)
	{
		offset = NULL;
		offset = findOffset(fieldType, ":");

		/* Copies string, removing ":" from beggining */
		i = 0;
		j = 2;
		fieldType2 = NULL;
		do{
			c = offset[j];
			fieldType2 = (char*)realloc(fieldType2, sizeof(char)*(i+1));
			fieldType2[i++] = c;
			j = j + 1;
		}while(c != '\0');
		fieldType2[i-1] = '\0';
		init(fieldType2, k, types, bytes);
		free(fieldType2);
	}
	else
	{
		signal = -1;
	}
	free(fieldType);
	return signal;
}

/*
	Create a file "fileName.reg" (it it doesn't exist) and insert entry
	params:
	   - fileName: file name to be created
	   - params: parameters to be inserted
	   - size: params size
*/
void insertEntry(char *fileName, char **params, int size)
{
	int i;

	/* Opens file for writing - if doesn't exist, create it */
	FILE *reg = fopen(fileName, "a+");
	fseek(reg, 0, SEEK_END);
	for(i = 0; i < size; i++)
	{
		fprintf(reg, "%s\n", params[i]);
	}
	fclose(reg);
}

/*
	Get a file name removing extensions
	param:
	   - name: file name
	returns:
	   - file name without extension
*/
char *getName(char *name)
{
	int i, j;
	char *originalName, c;
	i = 0;
	j = 0;
	originalName = NULL;
	do{
		c = name[j];
		originalName = (char*)realloc(originalName, sizeof(char)*(i+1));
		originalName[i++] = c;
		j = j + 1;
	}while(c != '.' && c != '\0');
	originalName[i-1] = '\0';
	// while(name[i] != '.')
	// {
	// 	originalName = (char*)realloc(originalName, sizeof(char)*(i+1));
	// 	originalName[i++] = name[i];
	// }
	return originalName;
}

/*
	Create a fileName.idx index file, containing inserted entries.
	params:
	   - offset: sum of bytes for each entry;
	   - size: amount of elements for each entry;
	   - entries: amount of elements in ".reg" file;
	   - regFileName: register file name.]
	returns:
	   - sorted index vector of structs
*/
Vector makeIndex(int offset, int size, int entries, char* regFileName)
{
	int i, j;
	FILE *regFile;
	char *trash, *name, **parameters;
	Vector data;
	regFile = fopen(regFileName, "r");
	if(regFile)
	{
		/* Allocate auxiliary vector for storage of indexes */
		parameters = (char**)malloc(sizeof(char*)*(entries+1));

		/* Allocate vector of structs */
		data = (tVector*)malloc(sizeof(tVector)*(entries+1));

		/* for: loops j times size of each entry */
		for(i = 0; i < (entries*size); i++)
		{
			/* Get key and assign offset */
			if(i % size == 0) // FIXME - size or (size+1)?
			{
				data[i/size].offset = offset*(i/size);
				parameters[i/size] = NULL;
				parameters[i/size] = readLine(regFile);
				data[i/size].key = 0;

				/* Converting str to int */
				j = 0;
				while(j < strlen(parameters[i/size]))
				{
					data[i/size].key = ((data[i/size].key)*10) + (parameters[i/size][j]-48);
					j = j + 1;
				}
			}
			else
			{
				/* Do nothing - skip until next key */
				trash = readLine(regFile);
				free(trash);
			}
		}

		/* Get file name without ".reg" extension */
		name = NULL;
		name = getName(regFileName);
		// printf("name: %s\n", name);

		/* Sort struct */
		qsort(data, entries, sizeof(tVector), cmp);

		/* Open ".idx" file and write contents */
		name = (char*)realloc(name, sizeof(char)*(strlen(name)+5));
		strcat(name, ".idx");
		fclose(regFile);
		regFile = fopen(name, "w");
		if(regFile)
		{
			for(i = 0; i < entries; i++)
			{
				fprintf(regFile, "%d\t%d\n", data[i].key, data[i].offset);
			}
			fclose(regFile);
		}
		freeMatrix(parameters, entries);
		free(name);
	}
	return data;
}

/*
	Performs a binary search
	params:
	   - data: vector of struct to be searched;
	   - left: index indicating beggining of vector to be searched;
	   - right: index indicating end of vector to be searched;
	   - element: element to be searched.
	return:
	   - offset of element if found, or -1 if not.
*/
int binarySearch(Vector data, int left, int right, int element)
{
	int middle;

	/* if left > right, unsuccessful search. Return -1 */
	if(left > right)
	{
		return -1;
	}
	else
	{
		middle = (left+right)/2;
		if(data[middle].key < element)
		{
			return binarySearch(data, (middle+1), right, element);
		}
		else if(data[middle].key > element)
		{
			return binarySearch(data, left, (middle-1), element);
		}
		// else if(data[middle].key == element)
		else
		{
			return data[middle].offset;
		}
	}
}

/*
	Find a given entry index for printing
	params:
	   - regFileName: name of ".reg" extension file
	   - entries: number of entries
	   - size: size of each entry
	   - index: offset index of entry to be printed
	   - offset: offset of entries
*/
void findEntry(char *regFileName, int entries, int size, int index, int offset)
{
	int i;
	FILE *regFile;
	char *s;
	regFile = fopen(regFileName, "r");
	if(regFile)
	{
		/* For loop to skip all unecessary lines until index line*/
		for(i = 0; i < ((index/offset)*size); i++)
		{
			s = NULL;
			s = readLine(regFile);
			// printf("skipped s: %s\n", s);
			free(s);
		}

		/* For loop to print entry */
		for(i = 0; i < size; i++)
		{
			s = NULL;
			s = readLine(regFile);
			printf("%s\n", s);
			free(s);
		}
		fclose(regFile);
	}
}

/* Main function */
int main(void)
{
	/* Variable declarations */
	/* File - stream for reading and writing files */
	FILE *file;
	/* data: vector of structs, containing at each cell both "key" and "offset" information, for index creation */
	Vector data;
	/* fileName: name of ".dat" file; regFileName: name of ".reg" file; types: vector of strings storing types from ".dat" definition;
	command: string that takes either "insert", "index", "search" or "exit"; parameters: parameters for "insert" command; 
	indexName: ".idx" file name */
	char *fileName, *regFileName, **types, *command, **parameters, *indexName;
	/* i, j, k: for loop control variables; signal: EOF dispatcher for reading files; bytes: vector of ints containing each types' bytes;
	size: number of elements for each entry; number: number for "search" command */
	int i, j, k, signal = 1, *bytes, size, number;

	/* Step 1: read .dat file name */
	fileName = NULL;
	fileName = readLine(stdin);
	file = fopen(fileName, "rb");
	if(file)
	{
		/* Step 2: count number of elements in .dat file */
		regFileName = "1";
		size = 0;
		while(strcmp(regFileName, "-1") != 0)
		{
			if(strcmp(regFileName, "1") != 0)
			{
				free(regFileName);
			}
			regFileName = readLine(file);
			size = size + 1;
		}
		free(regFileName);

		size = (size-2)/2;

		rewind(file);

		/* Step 3: Allocate variables according to size */
		bytes = (int*)malloc(sizeof(int)*(size+1));
		types = (char**)malloc(sizeof(char*)*(size+1));

		/* Step 4: read .reg file name */
		regFileName = NULL;
		regFileName = readRegFileName(file);
		/* First line was read - file ptr now on second line */

		/* Step 5: read whole .dat file */
		i = 0;
		while(signal == 1)
		{
			// printIntVector(bytes, i);
			// printStrMatrix(types, i);
			signal = readNameAndType(file, types, bytes, i);
			i = i + 1;
		}
		fclose(file);

		/* j variable will keep track of how many entries there are */
		j = 0;

		/* Step 6: read command */
		command = readCommand(stdin);

		/* Step 7: make if/else chain of commands */
		while(strcmp(command, "exit") != 0)
		{
			if(strcmp(command, "insert") == 0)
			{
				parameters = (char**)malloc(sizeof(char*)*(size+1));
				for(i = 0; i < size; i++)
				{
					if(i == (size-1))
					{
						parameters[i] = readLine(stdin);
						/* If true, then parameter is a char - double quotes must be removed */
						
						if(strstr(types[i], "char") != NULL)
						{
							k = 0;
							signal = strlen(parameters[i]);
							while(k < signal)
							{
								parameters[i][k] = parameters[i][k+1];
								k = k + 1;
							}
							parameters[i][k-2] = '\0';
						}
					}
					else
					{
						parameters[i] = readParam(stdin);
						/* If true, then parameter is a char - double quotes must be removed */
						if(strstr(types[i], "char") != NULL)
						{
							k = 0;
							signal = strlen(parameters[i]);
							while(k < signal)
							{
								// printf("parameters[i][k]: %c\tparameters[i][k+1]: %c\n", parameters[i][k], parameters[i][k+1]);
								parameters[i][k] = parameters[i][k+1];
								k = k + 1;
							}
							parameters[i][k-2] = '\0';
							// printf("parameters: %s\n", parameters[i]);
						}
						fgetc(stdin);
					}
					// printf("%s\n", parameters[i]);
				}
				insertEntry(regFileName, parameters, size);
				j = j + 1;
				freeMatrix(parameters, size);
			}
			else if(strcmp(command, "index") == 0)
			{
				data = NULL;
				data = makeIndex(sumVector(bytes, size), size, j, regFileName);
			}
			else if(strcmp(command, "search") == 0)
			{
				/* Create "parameters" vector to store search number */
				parameters = (char**)malloc(sizeof(char*)*1);
				parameters[0] = readLine(stdin);
				// printf("parameters[0]: %s\n", parameters[0]);

				/* Check if ".idx" file was created; if not, create it now */
				indexName = getName(regFileName);
				indexName = (char*)realloc(indexName, sizeof(char)*(strlen(indexName)+5));
				strcat(indexName, ".idx");
				file = NULL;
				file = fopen(indexName, "r");

				/* If ".idx" doesn't exist, create it */
				if(!file)
				{
					data = NULL;
					data = makeIndex(sumVector(bytes, size), size, j, regFileName);
				}

				// FIXME - fclose(file)

				/* Convert string read number to int */
				number = toNum(parameters[0]);

				/* Perform a binary search on "data" structure - "i" will contain index */
				i = binarySearch(data, 0, j, number);

				/* Recovers entry for printing, if any */
				if(i != -1)
				{
					findEntry(regFileName, j, size, i, sumVector(bytes, size));
				}

				/* Find in ".reg" file and print */
				freeMatrix(parameters, 1);
				free(indexName);
			}
			else
			{
				printf("Command not found.\n");
				break;
			}
			free(command);
			command = readCommand(stdin);
		}

		/* Step 8: free variables */
		free(data);
		free(command);
		free(bytes);
		freeMatrix(types, size);
		free(regFileName);
	}
	else
	{
		printf("File not found.\n");
	}
	free(fileName);
	return 0;
}