/*
	Program that calculates both discrete fourier transform and its inverse over a given audio file.
	Author: Diego S. Cintra
	USP Number: 10094043
	Date: 18/05/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define M_PI 3.14159265358979323846

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

typedef struct _mag{
	int index;
	double z;
	double complex cmplx;
}mag;

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
	if((*(mag*)a).z < (*(mag*)b).z) 
	{
		return 1;
	}
	else if((*(mag*)a).z == (*(mag*)b).z) 
	{
		return 0;
	}
	else
	{ 
		return -1;
	}
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
    }while(c != delimiter && c != '\r' && c != EOF);
    line[count-1] = '\0';
    return line;
}

/*
	Read audio from binary file, storing it as unsigned char
	params:
	   - file: audio file;
	   - n: number of observations
	returns:
	   - unsigned char type signal
*/
unsigned char *readAudio(FILE *file, int *n)
{
	int audioSize;
	unsigned char *audio = NULL;
	fseek(file, 0, SEEK_END);
	audioSize = ftell(file);
	rewind(file);
	audio = (unsigned char*)malloc(sizeof(unsigned char)*audioSize);
	fread(audio, 1, audioSize, file);
	(*n) = audioSize;
	return audio;
}

/*
	Compute discrete Fourier transform (DFT)
	params:
	   - signal: audio signal to be processed;
	   - n: number of observations;
	returns:
	   - dft processed signal
*/
double complex *dft(unsigned char *signal, int n)
{
	int i, j;
	double complex z, *coeffs;
	coeffs = (double complex*)calloc(((n/2)+1), sizeof(double complex));
	for(i = 0; i < (n/2)+1; i++)
	{
		for(j = 0; j < n; j++)
		{
			/*z = signal[j]*(cos((double)i*2.00*M_PI*((double)j/(double)n))) - 
				signal[j]*((sin((double)i*2.00*M_PI*((double)j/(double)n))) * I);*/
			z = signal[j]*cexp(-I * (double)i * 2.00 * M_PI * ((double)j/(double)n));
			// z = cos((double)i*2.00*M_PI*((double)j/(double)n)) - (sin((double)i*2.00*M_PI*((double)j/(double)n)) ) * I;
			coeffs[i] = coeffs[i] + z;
			// coeffs[i] = coeffs[i] + ((double)signal[j]*z);
		}
		if(i == 0)
		{
			coeffs[i] = coeffs[i] * (double)(1.00/(double)n);
		}
		else
		{
			coeffs[i] = coeffs[i] * (double)(2.00/(double)n);
		}
	}
	return coeffs;
}

/*
	Compute inverse discrete Fourier transform
	params:
	   - coefficients: coefficients;
	returns:
	   - original signal
*/
unsigned char *inverseDft(double complex *coefficients, /*double *coefficients*/ int n)
{
	int i, j;
	unsigned char *newSignal;
	double complex z;
	newSignal = (unsigned char*)calloc(n, sizeof(unsigned char));
	// z2 = (double complex *)calloc(n, sizeof(double complex));
	for(i = 0; i < n; i++)
	{
		z = 0.000 + 0.000 * I;
		for(j = 0; j < (n/2)+1; j++)
		{
			z = z + (coefficients[j]*(cos((double)j*2.00*M_PI*((double)i/(double)n))) + 
				coefficients[j]*((sin((double)j*2.00*M_PI*((double)i/(double)n)) * I)));
			// z = cexp(I * (double)j * 2.00 * M_PI * ((double)i/(double)n));
		}
		newSignal[i] = (unsigned char)round(z);
		// newSignal[i] = (unsigned char)(creal(z2[i])-cimag(z2[i]));
	}
	return newSignal;
}

/*
	Get coefficients magnitudes
	params:
	   - coeffs: complex double type containing transformed values;
	   - n: coeffs size
	returns:
	   - mag struct containing magnitude value and index
*/
mag *getMagnitudes(double complex *coeffs, int n)
{
	int i;
	mag *magnitudes;
	magnitudes = (mag*)calloc((n/2)+1, sizeof(mag));
	for(i = 0; i < (n/2)+1; i++)
	{
		magnitudes[i].cmplx = coeffs[i];
		magnitudes[i].z = cabs(coeffs[i]);
		magnitudes[i].index = i;
	}
	return magnitudes;
}

int main(void)
{
	int i, C, n, magSize;
	char *audio;
	unsigned char *signal, *newSignal;
	// double *coefficients;
	double complex *coeffs;
	mag *magnitudes;
	FILE *file;
	/* Step 1: read audio name */
	audio = read(stdin, '\n');
	file = fopen(audio, "rb");
	if(file)
	{
		/* Step 2: read number of most relevant components */
		scanf("%d", &C);
		if(DEBUG_TEST)
		{
			signal = (unsigned char*)malloc(sizeof(unsigned char)*3);
			signal[0] = 111;
			signal[1] = 145;
			signal[2] = 120;
			n = 3;
		}
		else
		{
			signal = readAudio(file, &n);
		}

		/* Step 3: calculate discrete fourier transform */
		coeffs = dft(signal, n);
		
		/* Step 4: calculate coefficients magnitudes */
		magnitudes = getMagnitudes(coeffs, n);
		qsort(magnitudes, (n/2)+1, sizeof(mag), cmp);

		/* Step 5: count number of magnitudes bigger than 0.1 */
		magSize = 0;
		for(i = 0; i < (n/2)+1; i++)
		{
			if(magnitudes[i].z > 0.100000)
			{
				magSize = magSize + 1;
			}
		}

		/* Step 6: remove least relevant components */
		for(i = C; i < (n/2)+1; i++)
		{
			magnitudes[i].cmplx = 0.00 + 0.00 * I;
		}

		/* Step 7: return coefficients to original positions */
		for(i = 0; i < (n/2)+1; i++)
		{
			coeffs[magnitudes[i].index] = magnitudes[i].cmplx;
		}

		/* Step 8: calculate inverse */
		newSignal = inverseDft(coeffs, n);
	
		/* Step 9: print results */
		printf("%d\n", n);
		printf("%d\n", magSize);
		for(i = 0; i < C; i++)
		{
			printf("%d ", (int)magnitudes[i].z);
		}
		printf("\n");
		for(i = 0; i < n; i++)
		{
			printf("%d\n", newSignal[i]);
		}
		free(newSignal);
		// free(coefficients);
		free(magnitudes);
		free(coeffs);
		free(signal);
		fclose(file);
	}
	else
	{
		printf("File not found.\n");
	}
	/* Step ???: free variables */
	free(audio);
	return 0;
}