#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
	char *fileContent;
	double **matrixM;
	int **matrixY;
	int **matrixX;
	int size;
}input_struct;

char vector[92] = {
' ','a','b','c','d','e','f','g','h','i',
'j','k','l','m','n','o','p','q','r','s',
't','u','v','w','x','y','z','A','B','C',
'D','E','F','G','H','I','J','K','L','M',
'N','O','P','Q','R','S','T','U','V','W',
'X','Y','Z','0','1','2','3','4','5','6',
'7','8','9',':',';','<','=','>','?','@',
'!','\"','#','$','%','&','\'','(',')','*',
'+',',','-','.','/','[','\\',']','_','{',
'|', '}'
};

void printMatrix(int **matrix, int n, int m)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printMessage(input_struct *data)
{
	int i, j;
	for (i = 0; i < data->size; i++)
	{
		for (j = 0; j < data->size; j++)
		{
			printf("%c", vector[abs(data->matrixX[j][i])]);
		}
	}
	printf("\n");
}

int getIndex(char character)
{
	int i;
	for (i = 0; i < 92; i++)
	{
		if (vector[i] == character)
			return i;
	}

	return -1;
}

void swap(input_struct *data, int newPivotPosition, int wrongPivotPosition)
{
	int i;
	double tmp;

	for (i = 0; i < 2*data->size; i++)
	{
		tmp = data->matrixM[newPivotPosition][i];
		data->matrixM[newPivotPosition][i] = data->matrixM[wrongPivotPosition][i];
		data->matrixM[wrongPivotPosition][i] = tmp;
	}
}

void searchNewPivot(input_struct *data, int pivot)
{
	int i;
	for (i = pivot+1; i < data->size; i++)
	{
		if (data->matrixM[i][pivot] != 0.0000000)
		{
			swap(data, i, pivot);
		}
	}
}

void transformPivot(input_struct *data, int pivot)
{
	int i;
	double pivotValue = data->matrixM[pivot][pivot];
	for (i = 0; i < 2*data->size; i++)
	{
		if (data->matrixM[pivot][i] != 0.0000000)
		{
			data->matrixM[pivot][i] = data->matrixM[pivot][i] / pivotValue;
		}
	}
}

void createMatrixY(input_struct *data)
{
	int i, j;
	for (i = 0; i < data->size; i++)
	{
		for (j = 0; j < data->size; j++)
		{
			data->matrixY[j][i] = getIndex(data->fileContent[(data->size*i + j)]);
		}
	}
}

void getInverseMatrix(input_struct *data)
{
	int i, j, k;
	for (i = 0; i < data->size; i++)
	{
		for (j = data->size; j < 2*data->size; j++)
		{
			if (j - i == data->size)
				data->matrixM[i][j] = 1.000000;
			else
				data->matrixM[i][j] = 0.000000;		
		}
	}

	double coeficiente;
	
	for (i = 0; i < data->size; i++)
	{
		if (data->matrixM[i][i] == 0.0000000)
		{
			searchNewPivot(data, i);
		}
		
		if (data->matrixM[i][i] != 1.0000000)
		{
			transformPivot(data, i);
		}

		for (j = 0; j < data->size; j++)
		{
			if (i != j)
			{
				coeficiente = data->matrixM[j][i];
				for (k = 0; k < 2*data->size; k++)
				{
					data->matrixM[j][k] = -coeficiente * data->matrixM[i][k] + data->matrixM[j][k];
				}
			}
		}
	}
}

void calculeMatrixX(input_struct *data)
{
	int i, j, k;
	double sum = 0.0;
	for (i = 0; i < data->size; i++)
	{
		for (j = 0; j < data->size; j++)
		{
			for (k = 0; k < data->size; k++)
			{
				sum += data->matrixM[i][k+data->size] * data->matrixY[k][j];
 			}

			data->matrixX[i][j] = (int)sum;
			sum = 0.0;
		}
	}
}

void produceSolution(input_struct *data)
{
	createMatrixY(data);
	getInverseMatrix(data);
	calculeMatrixX(data);

	printMessage(data);
}

int main(int argc, char const *argv[])
{
	int i, j;
	FILE *fin;
	input_struct data;

	char *fname = (char*)malloc(512 * sizeof(char));
	scanf("%s", fname);

	fin = fopen(fname, "r");

	fseek(fin, 0, SEEK_END);
	int fileSize = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	data.fileContent = (char*)malloc(fileSize * sizeof(char));

	for (i = 0; i < fileSize; i++)
	{
		fscanf(fin, "%c", &(data.fileContent[i]));
	}

	data.size = sqrt(fileSize);

	data.matrixM = (double**)malloc(data.size * sizeof(double*));
	for (i = 0; i < data.size; i++)
	{
		data.matrixM[i] = (double*)malloc(2*data.size * sizeof(double));
	}

	data.matrixY = (int**)malloc(data.size * sizeof(int*));
	for (i = 0; i < data.size; i++)
	{
		data.matrixY[i] = (int*)malloc((data.size) * sizeof(int));
	}

	data.matrixX = (int**)malloc(data.size * sizeof(int*));
	for (i = 0; i < data.size; i++)
	{
		data.matrixX[i] = (int*)malloc((data.size) * sizeof(int));
	}

	for (i = 0; i < data.size; i++)
	{
		for (j = 0; j < data.size; j++)
		{
			scanf("%lf", &data.matrixM[i][j]);
		}
	}

	produceSolution(&data);

	for (i = 0; i < data.size; i++)
	{
		free(data.matrixM[i]);
	}
	free(data.matrixM);

	for (i = 0; i < data.size; i++)
	{
		free(data.matrixY[i]);
	}
	free(data.matrixY);

	for (i = 0; i < data.size; i++)
	{
		free(data.matrixX[i]);
	}
	free(data.matrixX);

	free(data.fileContent);

	return 0;
}