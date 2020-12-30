/*
	Program that executes DCT (discrete cossine transformation) and returns K biggest values
	Author: Diego Cintra
	USP Number: 10094043
	Date: 09/04/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.141592653589793238462643383279

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
	if(*(double*)a < *(double*)b) 
	{
		return -1;
	}
	else if(*(double*)a == *(double*)b) 
	{
		return 0;
	}
	else
	{ 
		return 1;
	}
}

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
void printVector2(double *vector, int size)
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
	// printf("%d\n", size);
}

/*
	Execute DCT-II, from https://en.wikipedia.org/wiki/Discrete_cosine_transform#DCT-II
	params:
	   - amp: amplitude vector
	   - size: amp size
	returns:
	   - transformed array
*/
double *dct(unsigned char *amp, int size)
{
	int i, j;
	double *X;
	X = (double*)malloc(sizeof(double)*size);
	for(i = 0; i < size; i++)
	{
		X[i] = 0.0000000000;
		for(j = 0; j < size; j++)
		{
			// printf("j:%d\t amp[j]:%d\t cos(PI/(size))*(j+(1/2)*i:%lf\n", j, amp[j], (cos((PI/(size))*(j+(1/2)*i))));
			// X[i] = X[i] + amp[j]*cos((PI/size)*(j+(1/2))*i);
			/* Original formulation from http://ieeexplore.ieee.org.ez67.periodicos.capes.gov.br/stamp/stamp.jsp?tp=&arnumber=1672377&isnumber=35077 */
			if(i == 0)
			{
				X[i] = X[i] + amp[j];
			}
			else
			{
				X[i] = X[i] + (amp[j]*cos( ( (2*j + 1)*i*PI )/( 2*((double)size) ) ));
			}
		}
		if(i == 0)
		{
			X[i] = X[i] * (sqrt(2)/((double)size));
		}
		else
		{
			X[i] = X[i] * (2/((double)size));
		}
	}
	// printVector2(X, size);
	return X;
}

/* Main function */
int main(void)
{
	FILE *file;
	char *fileName;
	unsigned char *amp, *amp2;
	double *transformed;
	int i, size, k;
	/* Step 1: read file name */
	fileName = NULL;
	fileName = readFileName();
	file = fopen(fileName, "rb");
	if(file)
	{
		/* Step 2: read audio file */
		amp = NULL;
		amp = readAudio(file, &size);
		/* Step 3: read K value */
		scanf("%d", &k);
		/* Step 4: execute DCT */
		transformed = NULL;
		transformed = dct(amp, size);
		/* Step 5: sort transformed vector */
		qsort(transformed, size, sizeof(double), cmp);
		/* Step 6: print K biggest values */
		for(i = (size-1); i >= (size-k); i--)
		{
			printf("%.2lf\n", transformed[i]);
		}
		/* Step 7: free dynamically allocated variables */
		free(amp);
	}
	else
	{
		printf("File not found.\n");
	}
	fclose(file);
	return 0;
}