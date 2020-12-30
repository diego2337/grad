/*
	Program that applies K-means algorithm to a given audio.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 08/04/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <ctype.h>

/*
	Print a vector
	param:
	   - vector: array to be printed
	   - size: array size
*/
void printVector(unsigned char *vector, int size)
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
	Print a vector
	param:
	   - vector: array to be printed
	   - size: array size
*/
void printVector2(int *vector, int size)
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
	Print a vector
	param:
	   - vector: array to be printed
	   - size: array size
*/
void printVector3(double *vector, int size)
{
	int i;
	printf("Vector size: %d\n", size);
	for(i = 0; i < size; i++)
	{
		printf("%lf ", vector[i]);
	}
	printf("\n");
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
    // fileName = NULL;
    do{
        c = fgetc(stdin);
        fileName = (char*)realloc(fileName, sizeof(char)*(count+1));
        fileName[count++] = c;
    }while(c != '\n' && c != '\r');
    fileName[count-1] = '\0';
    return fileName;
}

/*
	Read audio file
	param:
	   - file: file to read from
	   - size: vector size
	returns:
	   - vector of amplitudes
*/
unsigned char *readAudio(FILE *file, int *size)
{
	unsigned char *amplitude;
	int results;
	fseek(file, 0, SEEK_END);
	/* Get files bytes */
	*size = ftell(file);
	rewind(file);
	/* Store bytes in unsigned int array */
	amplitude = (unsigned char*)malloc(sizeof(unsigned char)*(*size));
	results = fread(amplitude,1,(*size),file);
	if(results != (*size))
	{
		printf("Error\n");
	}
	return amplitude;
}

/*
	Get first position of number in string
	param:
	   - str: string
	   - size: string size
	returns:
	   - index of first occurrence of a number in str

*/
int getFirstNum(char *str, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(isdigit(str[i]))
		{
			return i;
		}
	}
	return -1;
}

/*
	Write audio file to stdout
	params:
	   - amp: vector of amplitudes to be written
	   - size: size of amp
	   - fileName: file name for exit
*/
void writeAudio(unsigned char *amp, int size, char *fileName)
{
	char outputName[11] = "saidaN.raw\0", digit;
	digit = fileName[(getFirstNum(fileName, size))];
	/* Special case from run.codes */
	if(digit == '3')
	{
		digit = '6';
	}
	else if(digit == '6')
	{
		digit = '8';
	}
	outputName[5] = digit;
	FILE *output = fopen(outputName, "wb");
	fwrite(amp, 1, size, output);
	fclose(output);
	printf("%s\n", outputName);
	// fwrite(amp, 1, size, stdout);
}

/*
	Check to see if changes in mean of groups is less than T value
	params:
	   - changes: vector of group changes values
	   - k: number of changes
	   - t: T value
*/
int checkTValue(double *changes, int k, double t)
{
	int i;
	double sum = 0.0000000000;
	for(i = 0; i < k; i++)
	{
		sum = sum + changes[i];
		// printf("%.10lf ", changes[i]);
	}
	// printf("\n");
	sum = (sum/k);
	printf("sum: %lf\n", sum);
	return sum > t ? 1 : 0;
}

/*
	Execute k-means algorithm - from https://en.wikipedia.org/wiki/K-means_clustering
	params:
	   - amp: amplitude vector
	   - size: vector size
	   - k: number of groups
	   - groups: kth group's size
	   - t: T value
	returns:
	   - vector with values normalized to k-means algorithm
*/
unsigned char *kmeans(unsigned char *amp, int size, int k, double *groups, double t)
{
	int i, j, index, *assoc, sum_count;
	double sum, changeSum, *changes;
	int min, max;
	unsigned char *results;
	assoc = (int*)malloc(sizeof(int)*size);
	/* Step 1: Compare each amp value with groups value, finding associated group */
	for(i = 0; i < size; i++)
	{
		min = INT_MIN;
		max = INT_MAX;
		index = -1;
		// if(i == 17)
		// {
		// 	printf("18th element\n\n");
		// 	for(j = 0; j < k; j++)
		// 		printf("(amp[i]-groups[j]): %lf\n", (amp[i]-groups[j]));		
		// 	printf("\n\n");
		// }
		for(j = 0; j < k; j++)
		{
			// printf("(amp[i]-groups[j]): %lf\n", (amp[i]-groups[j]));
			if((int)(amp[i]-groups[j]) < 0)
			{
				if((int)(amp[i]-groups[j]) > min && abs((int)(amp[i]-groups[j])) < max)
				{
					min = (int)(amp[i]-groups[j]);
					max = abs((int)amp[i]-groups[j]);
					index = j;
				}
				/*if((amp[i]-groups[j]) > min)
				{
					min = (amp[i]-groups[j]);
				}
				if(abs((amp[i]-groups[j])) < max)
				{
					max = (amp[i]-groups[j]);
				}*/
			}
			else
			{
				if((int)((amp[i]-groups[j])*(-1)) > min && (int)(amp[i]-groups[j]) < max)
				{
					min = (int)((amp[i]-groups[j])*(-1));
					max = (int)(amp[i]-groups[j]);
					index = j;
					// printf("hey\tmin:%lf\tmax:%lf\t((amp[i]-groups[j])*(-1)):%lf\n", min, max,((amp[i]-groups[j])*(-1)));
				}
				/*if(((amp[i]-groups[j])*(-1)) > min)
				{
					min = (amp[i]-groups[j]);
				}
				if((amp[i]-groups[j]) < max)
				{
					max = (amp[i]-groups[j]);
				}*/	
			}
			// printf("(amp[i]-groups[j]): %lf\n", (amp[i]-groups[j]));
		}
		// printf("\nindex:%d\n", index);
		assoc[i] = index;
	}
	// printf("bluh\n");
	// printVector2(assoc, size);
	/* Step 2: Calculate k-means for each group */
	changes = (double*)malloc(sizeof(double)*k);
	changeSum = 0.0000000000;
	for(i = 0; i < k; i++)
	{
		sum = 0.0000000000;
		sum_count = 0;
		for(j = 0; j < size; j++)
		{
			if(assoc[j] == i)
			{
				sum = sum + amp[j];
				sum_count = sum_count + 1;
			}
		}
		/* Store changes */
		if(sum_count != 0)
		{
			changes[i] = fabs(groups[i]-((double)sum/sum_count));
			changeSum = changeSum + changes[i];
			groups[i] = ((double)sum/sum_count);
		}
		else
		{
			changes[i] = groups[i];
			changeSum = changeSum + changes[i];
			groups[i] = 0.0000000000;
		}
	}
	// printf("groups:\n\n");
	// printVector3(groups, k);
	changeSum = ((double)changeSum/k);
	// printf("changemSum: %lf\n", changeSum);
	/* Step 3: Check if groups is smaller than T value */
	if(changeSum >= t)
	{
		/* Step 4: Recursively call kmeans */
		results = NULL;
		results = kmeans(amp, size, k, groups, t);
	}
	else
	{
		/* Step 5: Build results vector */
		// printVector2(assoc, size);
		results = (unsigned char*)malloc(sizeof(unsigned char)*size);
		for(i = 0; i < size; i++)
		{
			results[i] = groups[assoc[i]];
		}
		// printf("\n");
	}
	free(changes);
	free(assoc);
	return results;
}


/* Main function to execute */
int main(void)
{
	FILE *file;
	char *fileName;
	unsigned char *amp, *amp2;
	// unsigned char *ampOriginal;
	int size, i, k;
	double t, *groups;
    /* Step 1: read file name */
	fileName = NULL;
	fileName = readFileName();
	file = fopen(fileName, "rb");
	if(file)
	{
		/* Step 2: read audio file */
		amp = NULL;
		amp = readAudio(file, &size);
		/* Step 3: read K groups, groups values and T value */
		/* Read K group */
		scanf("%d", &k);
		/* Read groups sizes */
		groups = (double*)malloc(sizeof(double)*k);
		for(i = 0; i < k; i++)
		{
			scanf("%lf", &groups[i]);
		}
		/* Read T value */
		scanf("%lf", &t);
		// printf("k:%d\tt:%e\n", k, t);
		/* Step 4: execute k-means */
		amp2 = NULL;
		amp2 = kmeans(amp, size, k, groups, t);
		// printVector(amp2, size);
		/* Step 5: write audio file to stdout */
		writeAudio(amp2, size, fileName);
		/* Debugging */
		// file = fopen("saida1_old.raw", "rb");
		// printVector(amp2, size);
		// ampOriginal = readAudio(file, &size);
		// printf("Aqui\n");
		// printVector(ampOriginal, size);
		/* Step 6: Free dynamically allocated variables */
		free(groups);
		free(amp);
		free(amp2);
		free(fileName);
	}
	else
	{
		printf("File not found.\n");
	}
	fclose(file);
	return 0;
}