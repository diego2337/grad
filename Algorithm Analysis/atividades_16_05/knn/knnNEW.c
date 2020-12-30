/*
	Program that reads FEN notation chess input, sequence of moves and check possible win or draw situations to a given player.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 10/05/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMatrix(char **matrix, int n, int m)
{
    int i, j;
    if(m != -1)
    {
    	for(i = 0; i < n; i++)
	    {
    		for(j = 0; j < m; j++)
	        {
	            printf("%c", matrix[i][j]);
	        }
	        printf("\n");
	    }
    }
    else
    {
    	for(i = 0; i < n; i++)
    	{
    		for(j = 0; j < strlen(matrix[i]); j++)
    		{
    			printf("%c", matrix[i][j]);
    		}
    		printf("\n");
    	}
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
    }while(c != delimiter && c != EOF);
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
	Read both name and type from ".schema" file
	params:
	   - file: file to be read. first line already skipped;
	   - names: matrix of strings which will contain attribute names;
	   - types: matrix of strings which will contain attribute types;
	   - originalClassType: attribute to be used in "dump_schema" command
	   - attrSize: number of attributes, consequently being number of matrices rows
*/
void readNameAndType(FILE *file, char **names, char **types, char **originalClassType, int attrSize)
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
		/* Last attribute must be class; forcing char* */
		if((i+1) == attrSize)
		{
			if(!strstr(str, "char"))
			{
				/* Store original class type */
				(*originalClassType) = (char*)realloc((*originalClassType), sizeof(char)*(strlen(str)+1));
				strcpy((*originalClassType), str);
				types[i] = (char*)realloc(types[i], sizeof(char)*(9));
				strcpy(types[i], "char[30]");
				types[i][strlen(types[i])] = '\0';
			}
			else
			{
				/* Store original class type */
				(*originalClassType) = (char*)realloc((*originalClassType), sizeof(char)*(strlen(str)+1));
				strcpy((*originalClassType), str);
				types[i] = (char*)realloc(types[i], sizeof(char)*(strlen(str)+1));		
				strcpy(types[i], str);
			}
		}
		else
		{
			types[i] = (char*)realloc(types[i], sizeof(char)*(strlen(str)+1));
			strcpy(types[i], str);
		}
		free(str);
	}
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
	line = read(stdin, '\n');
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
			line = read(stdin, '\n');
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
			// printf("line: %s\ttype: %s\n", line, type);
			num = getSize(types[i]);
			line = realloc(line, sizeof(char)*((strlen(line)+1)+num));
			fwrite(line, sizeof(char), num, file);
			i = i + 1;
			free(line);
			line = NULL;
			line = read(stdin, '\n');
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
			line = read(stdin, '\n');
		}
		else if(strcmp(types[i], "double") == 0)
		{
			doubleValue = NULL;
			doubleValue = writeToDouble(line);
			fwrite(doubleValue, sizeof(double), 1, file);
			free(doubleValue);
			i = i + 1;
			free(line);
			line = NULL;
			line = read(stdin, '\n');
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
	   - attrSize: number of attributes, consequently being number of matrices rows
	returns:
	   - sum of sizes, in bytes
*/
int getSchemaSize(char **types, char *originalClassType, int attrSize)
{
	int i, size = 0;
	for(i = 0; i < attrSize; i++)
	{
		if(originalClassType && (i+1) == attrSize)
		{
			size = size + getSize(originalClassType);
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
void dumpSchema(char *table, int attrSize, char **names, char **types, char *originalClassType)
{
	int i, byteCount = 0;
	byteCount = getSchemaSize(types, originalClassType, attrSize);
	printf("table %s(%d bytes)\n", table, byteCount);
	/* attrSize+2 because of id and dist */
	for(i = 0; i < (attrSize+2); i++)
	{
		/* id */
		if(i == 0)
		{
			printf("id int(4 bytes)\n");
		}
		/* class */
		else if(i == attrSize)
		{
			printf("%s %s(%d bytes)\n", names[i-1], originalClassType, getSize(originalClassType));
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
	dataSize = (int)ceil((double)dataSize/getSchemaSize(types, NULL, attrSize));
	rewind(file);
	return dataSize;
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
				else if(strcmp(types[j-1], "double") == 0)
				{
					doubleValue = (double*)malloc(sizeof(double));
					fread(doubleValue, sizeof(double), 1, file);
					// printf("o que eh double value %.5lf\n", (*doubleValue));
					printf("%s = %.2lf\n", names[j-1], (*doubleValue));
					free(doubleValue);
				}
				else if(strstr(types[j-1], "["))
				{
					str = (char*)malloc(sizeof(char)*size);
					// printf("o que eh str value %s\n", str);
					fread(str, sizeof(char), size, file);
					printf("%s = %s\n", names[j-1], str);
					free(str);
				}
			}
		}
	}
}

/*
	Read entry from file
	params:
	   - file: file to be read;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - types: attributes types;
	   - attrId: store new entry id;
	   - attrInt: vector of type int, to store attributes;
	   - j: attrInt size;
	   - attrDouble: vector of type double, to store attributes;
	   - k: attrDouble size;
	   // - class: vector of type int, to store class;
	   - kValue: k-neighbors for "knn"
*/
void readEntry(FILE *file, int attrSize, char **types, int *attrId, int **attrInt, int *j, double **attrDouble, int *k, int *kValue)
{
	int i /*, size*/;
	for(i = 0, (*j) = 0, (*k) = 0; i < (attrSize+1); i++)
	{
		/*if((i-2) >= 0)
		{
			size = getSize(types[i-2]);
		}*/
		/* k-neighbors */
		if(i == 0)
		{
			fscanf(file, "%d\n", &(*kValue));
			// (*attrInt) = (int*)realloc((*attrInt), sizeof(int)*((*j)+1));
			// fscanf(file, "%d", &(*attrInt)[(*j)]);
			// // fread((*attrInt), sizeof(int), 1, file);
			// (*j) = (*j) + 1;
		}
		/* id */
		else if(i == 1)
		{
			// (*attrInt) = (int*)realloc((*attrInt), sizeof(int)*((*j)+1));
			fscanf(file, "%d\n", &(*attrId));
			// fread((*attrInt), sizeof(int), 1, file);
			// (*j) = (*j) + 1;
		}
		else if(strcmp(types[i-2], "int") == 0)
		{
			(*attrInt) = (int*)realloc((*attrInt), sizeof(int)*((*j)+1));
			fscanf(file, "%d\n", &(*attrInt)[(*j)]);
			// fread((*attrInt), sizeof(int), 1, file);
			(*j) = (*j) + 1;
		}
		else if(strcmp(types[i-2], "double") == 0)
		{
			(*attrDouble) = (double*)realloc((*attrDouble), sizeof(double)*((*k)+1));
			fscanf(file, "%lf\n", &(*attrDouble)[(*k)]);
			// fread((*attrDouble), sizeof(double), 1, file);
			(*k) = (*k) + 1;
		}
	}
}

/*
	Read entry from ".data" file
	params:
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - types: attributes types;
	   - binaryId: id of entry;
	   - binaryInt: int type attributes of entry;
	   - binaryDouble: double type attributes of entry;
	   - binaryClass: char * type attribute of entry;
	   - binaryDist: dist of entry
*/
void readBinaryEntry(FILE *file, int attrSize, char **types, int *binaryId, int **binaryInt, int *binaryIntSize,
	double **binaryDouble, int *binaryDoubleSize, char **binaryClass, double *binaryDist)
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
			else if(strcmp(types[i-1], "double") == 0)
			{
				(*binaryDouble) = (double*)realloc((*binaryDouble), sizeof(double)*((*binaryDoubleSize)+1));
				fread(&(*binaryDouble)[(*binaryDoubleSize)], sizeof(double), 1, file);
				(*binaryDoubleSize) = (*binaryDoubleSize) + 1;
			}
			else if(strstr(types[i-1], "["))
			{
				(*binaryClass) = (char*)realloc((*binaryClass), sizeof(char)*size);
				fread((*binaryClass), sizeof(char), size, file);
			}
		}
	}
}

/*
	Print entry
	params:
	   - binaryId: id of entry;
	   - binaryInt: int attributes of entry;
	   - binaryIntSize: number of int attributes of entry;
	   - binaryDouble: double attributes of entry;
	   - binaryDoubleSize: number of double attributes of entry;
	   - binaryClass: char * attribute of entry;
	   - binary dist: dist of entry
*/
void printEntry(int binaryId, int* binaryInt, int binaryIntSize, double *binaryDouble, int binaryDoubleSize, char *binaryClass, double binaryDist)
{
	int i;
	printf("id: %d\n", binaryId);
	printf("number of int entries: %d\n", binaryIntSize);
	for(i = 0; i < binaryIntSize; i++)
	{
		printf("int attribute %d: %d\n", i, binaryInt[i]);
	}
	printf("number of double entries: %d\n", binaryDoubleSize);
	for(i = 0; i < binaryDoubleSize; i++)
	{
		printf("double attribute %d: %.2lf\n", i, binaryDouble[i]);
	}
	printf("char * attribute: %s\n", binaryClass);
	printf("dist: %.2lf\n", binaryDist);
}

/*
	Calculate euclidean distance
	params:
	   - attrInt: vector of int attributes of new entry;
	   - binaryInt: vector of int attributes from training entry;
	   - intSize: number of attributes of size int;
	   - attrDouble: vector of double attributes of new entry;
	   - binaryDouble: vector of double attributes from training entry;
	   - doubleSize: number of attributes of size double;
	   - attrClass: char * attribute of new entry;
	   - binaryClass: char * attribute from training entry
	returns:
	   - euclidean distance from new entry to training entry
*/
double calculateEuclideanDistance(int *attrInt, int *binaryInt, int intSize, double *attrDouble, double *binaryDouble, int doubleSize,
	char *attrClass, char *binaryClass)
{
	int i;
	double distance = 0.00;
	for(i = 0; i < intSize; i++)
	{
		// printf("i: %d\tdistance: %.2lf\tbinaryInt[i]: %d\tattrInt[i]: %d\n", i, distance, binaryInt[i], attrInt[i]);
		distance = distance + (double)pow((binaryInt[i]-attrInt[i]), 2);
	}
	for(i = 0; i < doubleSize; i++)
	{
		distance = distance + pow((binaryDouble[i]-attrDouble[i]), 2);
	}
	distance = sqrt(distance);
	return distance;
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
	Free variables for next entry
	params:
	   - binaryId: id of entry;
	   - binaryInt: int attributes of entry;
	   - binaryIntSize: number of int attributes of entry;
	   - binaryDouble: double attributes of entry;
	   - binaryDoubleSize: number of double attributes of entry;
	   - binaryClass: char * attribute of entry;
	   - binary dist: dist of entry
*/
void freeVariables(int *binaryId, int **binaryInt, int *binaryIntSize, double **binaryDouble, int *binaryDoubleSize, 
	char **binaryClass, double *binaryDist)
{
	(*binaryId) = 0;
	free((*binaryInt));
	(*binaryInt) = NULL;
	(*binaryIntSize) = 0;
	free((*binaryDouble));
	(*binaryDouble) = NULL;
	(*binaryDoubleSize) = 0;
	free((*binaryClass));
	(*binaryClass) = NULL;
	(*binaryDist) = 0.00;
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
//  int attrId, int *attrInt, int intSize, double *attrDouble, int doubleSize, char **class
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
	int i, binaryId = 0, *binaryInt = NULL, binaryIntSize = 0, binaryDoubleSize = 0;
	double *binaryDouble = NULL, binaryDist = 0.00;
	char **classes, *class = NULL;
	classes = (char**)malloc(sizeof(char*)*kValue);
	for(i = 0; i < kValue; i++)
	{
		rewind(file);
		fseek(file, (distances[i].index*getSchemaSize(types, NULL, attrSize)), SEEK_SET);
		readBinaryEntry(file, attrSize, types, &binaryId, &binaryInt, &binaryIntSize, &binaryDouble, &binaryDoubleSize, &class, &binaryDist);
		classes[i] = (char*)malloc(sizeof(char)*(strlen(class)+1));
		strcpy(classes[i], class);
		freeVariables(&binaryId, &binaryInt, &binaryIntSize, &binaryDouble, &binaryDoubleSize, &class, &binaryDist);
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
	   - attrId: store new entry id;
	   - attrInt: vector of type int, to store attributes;
	   - intSize: attrInt size;
	   - attrDouble: vector of type double, to store attributes;
	   - doubleSize: attrDouble size;
	   - class: entry class
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
	Execute knn classification, using euclidean distance (sum(attributes)[sqrt(pow(nthAttr-nthAttr, 2)))])
	params:
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - names: attributes names;
	   - types: attributes types;
	   - nearestNeighbors: vector containing k-nearest neighbors;
	   - kValue: k
	returns:
	   - classified class
*/
char *knn(FILE *file, int attrSize, char **names, char **types, int **nearestNeighbors, int *kValue)
{
	int i, intSize, doubleSize, attrId = 0, *attrInt = NULL, binaryId = 0, *binaryInt = NULL, 
		binaryIntSize = 0, binaryDoubleSize = 0, dataSize = 0;
	double *attrDouble = NULL, binaryDist = 0.00, *binaryDouble = NULL;
	dist *distances = NULL;
	char *class = NULL, *binaryClass = NULL;

	/* Get entry */
	readEntry(stdin, attrSize, types, &attrId, &attrInt, &intSize, &attrDouble, &doubleSize, &(*kValue));
	// printEntry(attrId, attrInt, intSize, attrDouble, doubleSize, "to be classified", 0.00);

	/* Get data number of entries */
	dataSize = getDataSize(file, attrSize, types);
	// distances = (double*)calloc(dataSize, sizeof(double));
	distances = (dist*)malloc(sizeof(dist)*(dataSize));
	/* Calculate euclidean distances from new entry to every training entry */
	for(i = 0; i < dataSize; i++)
	{
		readBinaryEntry(file, attrSize, types, &binaryId, &binaryInt, &binaryIntSize, &binaryDouble, &binaryDoubleSize, &binaryClass, &binaryDist);
		// printEntry(binaryId, binaryInt, binaryIntSize, binaryDouble, binaryDoubleSize, binaryClass, binaryDist);
		init(&distances[i], i);
		distances[i].distance = calculateEuclideanDistance(attrInt, binaryInt, intSize, attrDouble, binaryDouble, doubleSize, class, binaryClass);
		/* Update dist in binary file */
		fseek(file, -8, SEEK_CUR);
		updateEntry(file, &distances[i].distance);
		// fseek(file, 8, SEEK_CUR);
		// printf("distances[i].distance: %.2lf\n", distances[i].distance);
		freeVariables(&binaryId, &binaryInt, &binaryIntSize, &binaryDouble, &binaryDoubleSize, &binaryClass, &binaryDist);
	}
	/* Find k-nearest neighbors */
	(*nearestNeighbors) = realloc((*nearestNeighbors), sizeof(int)*((*kValue)));
	(*nearestNeighbors) = kNearestNeighbors((*kValue), distances, dataSize);
	// attrId, attrInt, intSize, attrDouble, doubleSize, &class
	return classify(file, (*kValue), distances, types, attrSize);
}

/*
	Call knn classification and return matrix containing closest neighbors
	params:
	   - table: ".data" file name, including extension;
	   - file: ".data" file;
	   - attrSize: number of attributes, consequently being number of matrices rows;
	   - names: attributes names;
	   - types: attributes types;
	   - nearestNeighbors: vector containing k-nearest neighbors;
*/
void dump_nn(char *table, FILE *file, int attrSize, char **names, char **types, int **nearestNeighbors)
{
	/* Execute knn */
	int i, j, k, l, kValue = 0, binaryId = 0, *binaryInt = NULL, binaryIntSize = 0, binaryDoubleSize = 0;
	double *binaryDouble = NULL, binaryDist = 0.00;
	char *class = NULL;
	knn(file, attrSize, names, types, nearestNeighbors, &kValue);
	for(i = 0; i < kValue; i++)
	{
		rewind(file);
		fseek(file, ((*nearestNeighbors)[i]*getSchemaSize(types, NULL, attrSize)), SEEK_SET);
		readBinaryEntry(file, attrSize, types, &binaryId, &binaryInt, &binaryIntSize, &binaryDouble, &binaryDoubleSize, &class, &binaryDist);
		/* Print nearest neighbors */
		printf("id = %d\n", binaryId);
		for(j = 0, k = 0, l = 0; j < attrSize; j++)
		{
			if(strcmp(types[j], "int") == 0)
			{
				printf("%s = %d\n", names[j], binaryInt[k]);
				k = k + 1;
			}
			else if(strcmp(types[j], "double") == 0)
			{
				printf("%s = %.2lf\n", names[j], binaryDouble[l]);
				l = l + 1;
			}
			else if(strstr(types[j], "["))
			{
				printf("%s = %s\n", names[j], class);
			}
		}
		printf("dist = %.2lf\n", binaryDist);
		freeVariables(&binaryId, &binaryInt, &binaryIntSize, &binaryDouble, &binaryDoubleSize, &class, &binaryDist);
		// printf("nearestNeighbors[%d]: %d\n", i, (*nearestNeighbors)[i]);
	}
}

int main(void)
{
	/* Variable declarations */
	/* i, j: for loop control variables; attrSize - number of attributes in ".schema" file;
		nearestNeighbors: vector to contain indexes of nearestNeighbors; kValue - for knn */
	int attrSize, *nearestNeighbors, kValue = 0;
	/* schemaName - ".schema" extension file name; schema - only ".schema" name, no extension;
		names - attributes names extracted from ".schema" file; types - attributes types extracted from ".schema" file;
		tableName - name of ".data" file; table: ".data" file name; command - user input: either (exit), (dump_schema),
		(dump_data), (dump_nn) or (knn); originalClassType - to be used for "dump_schema" command */
	char *schemaName, *schema, **names, **types, *tableName, *table, *command, *originalClassType;
	/* fSchema - pointer to ".schema" file; fData - pointer to ".data" file */
	FILE *fSchema, *fData;

	/* Step 1: Reads .schema file name */
	schemaName = NULL;
	schemaName = read(stdin, '\n');
	// printf("%s\n", schemaName);
	/* Step 1.1: Open file */
	fSchema = fopen(schemaName, "r");
	if(fSchema)
	{
		/* Step 2: get ".schema" name and number of lines */
		/* Get only schema name, without ".schema" extension */
		schema = NULL;
		schema = split(schemaName, '.', 0);
		/* -1 given table is not an attribute */
		attrSize = countLines(fSchema) - 1;
		// printf("schema: %s\nattrSize: %d\n", schema, attrSize);
		/* Step 2.2: allocate names and types matrices */
		names = (char**)malloc(sizeof(char*)*(attrSize));
		types = (char**)malloc(sizeof(char*)*(attrSize));
		/* Step 2.3: Read ".schema" first line to store table name */
		free(schemaName);
		schemaName = NULL;
		schemaName = read(fSchema, '\n');
		tableName = split(schemaName, ' ', 1);
		table = NULL;
		table = (char*)calloc((strlen(tableName)+6), sizeof(char));
		strcat(table, tableName);
		strcat(table, ".data");
		// printf("%s\n", table);

		/* Step 3: Read names and types */
		originalClassType = NULL;
		readNameAndType(fSchema, names, types, &originalClassType, attrSize);
		// printf("names:\n");
		// printMatrix(names, attrSize, -1);
		// printf("types:\n");
		// printMatrix(types, attrSize, -1);

		/* Step 4: Create ".data" file */
		fData = fopen(table, "wb");
		if(fData)
		{
			readTrainingTest(fData, types, attrSize);
		}
		else
		{
			printf("Failed to write %s file.\n", table);
		}
		fclose(fData);
		command = NULL;
		command = read(stdin, '\n');
		while(strcmp(command, "exit") != 0)
		{
			/* dump_schema command */
			if(strcmp(command, "dump_schema") == 0)
			{
				dumpSchema(tableName, attrSize, names, types, originalClassType);
			}
			/* dump_data command */
			else if(strcmp(command, "dump_data") == 0)
			{
				fData = fopen(table, "rb");
				dumpData(fData, attrSize, names, types, getDataSize(fData, attrSize, types));
				fclose(fData);
			}
			/* dump_nn command */
			else if(strcmp(command, "dump_nn") == 0)
			{
				fData = fopen(table, "rb+");
				nearestNeighbors = NULL;
				dump_nn(table, fData, attrSize, names, types, &nearestNeighbors);
				fclose(fData);
			}
			/* knn command */
			else if(strcmp(command, "knn") == 0)
			{
				fData = fopen(table, "rb+");
				nearestNeighbors = NULL;
				printf("%s\n", knn(fData, attrSize, names, types, &nearestNeighbors, &kValue));
				fclose(fData);
			}
			else
			{
				printf("Command not found.\n");
			}
			free(command);
			command = NULL;
			command = read(stdin, '\n');
		}
		free(command);
		freeMatrix(names, attrSize);
		freeMatrix(types, attrSize);
		free(schemaName);
		free(tableName);
		free(table);
		free(schema);
		fclose(fSchema);
	}
	else
	{
		printf("File not found.\n");
	}
	return 0;
}