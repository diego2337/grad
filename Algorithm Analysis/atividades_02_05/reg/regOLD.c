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
	Struct of a pseudo vector with different types
	value: value to be stored
	type: type of value
	bytes: type size
*/
typedef struct _tVector{
	void *value;
	char *type;
	int bytes;
}tVector;
/* Typedef annotation for simpler pointer parameter management */
typedef tVector *Vector;

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
		printf("tVector at position %d\ntype:%s\tbytes:%d\n", i, data[i].type, data[i].bytes);
	}
}

/*
    Read file name
    returns:
       - line containing file name
*/
char *readFileName()
{
    int count;
    char c;
    count = 0;
    char *fileName = NULL;
    do{
        c = fgetc(stdin);
        fileName = (char*)realloc(fileName, sizeof(char)*(count+1));
        fileName[count++] = c;
    }while(c != '\n' && c != '\r');
    fileName[count-1] = '\0';
    return fileName;
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
    /* Code below gives SIGSEGV */
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != '\n' && c != '\r' && c != EOF);
    if(feof(file))
    {
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
	Initialize given tVector position
	params:
	   - data: tVector type data to be initialized;
       - type: type casting for "type" in tVector;
       - k: position to be initialized.
*/
void init(Vector **data, char *t, int k)
{
	Vector aux;
	aux = (tVector*)malloc(sizeof(tVector)*1);
	/* Initialize value */
	aux->value = NULL;
	/* Initialize bytes */
	aux->bytes = 0;
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
			aux->bytes = ((aux->bytes)*10) + (number[i]-48);
			i = i + 1;
			c = number[i];
		}
		aux->type = (char*)malloc(sizeof(char)*(strlen(t)+1));
		strcpy(aux->type, "char");	
	}
	else
	{
		aux->type = (char*)malloc(sizeof(char)*(strlen(t)+1));
		strcpy(aux->type, t);
		/* Assign bytes' correct size */
		if(strcmp(aux->type, "int") == 0)
		{
			aux->bytes = 4;	
		}
		else if(strcmp(aux->type, "double") == 0)
		{
			aux->bytes = 8;	
		}
		else if(strcmp(aux->type, "float") == 0)
		{
			aux->bytes = 4;	
		}
		else if(strcmp(aux->type, "char") == 0)
		{
			aux->bytes = 1;
		}
	}
	/* Copy to data struct */
	(*data)[k] = aux;
	printStruct((*data), (k+1));
}

/*
	Read both name and type from ".dat" file
	param:
	   - file: file to be read
	returns:
	   - EOF control signal
*/
int readNameAndType(FILE *file, Vector *data, int k)
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
		/* Creates struct to store .dat information */
		(*data) = (tVector*)realloc((*data), sizeof(tVector)*(k+1));
		init(&data, fieldType2, k);
		free(fieldType2);
	}
	else
	{
		signal = -1;
	}
	free(fieldType);
	return signal;
}

/* Main function */
int main(void)
{
	/* Variable declarations */
	FILE *file;
	char *fileName, *regFileName;
	Vector data;
	int i, signal = 1;

	/* Step 1: read .dat file name */
	fileName = NULL;
	fileName = readLine(stdin);
	file = fopen(fileName, "rb");
	if(file)
	{
		/* Step 2: read .reg file name */
		regFileName = NULL;
		regFileName = readRegFileName(file);
		/* First line was read - file ptr now on second line */

		/* Step 3: read key-name and key-type */
		data = NULL;
		// signal = readNameAndType(file, &data, 0);
		// printStruct(data, 1);

		/* Step 4: read whole .dat file */
		i = 0;
		while(signal == 1)
		{
			signal = readNameAndType(file, &data, i);
			i = i + 1;
			// printStruct(data, i);
		}
		// printf("%s\n", fileName);

		/* Step ?: free variables */
		fclose(file);
		free(fileName);
		free(regFileName);
	}
	return 0;
}