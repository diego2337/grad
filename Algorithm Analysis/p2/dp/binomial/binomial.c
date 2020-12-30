#include <stdio.h>
#include <stdlib.h>

long binomial(int n, int k, long **pascal)
{
	if(pascal[n][k] != 0)
	{
		return pascal[n][k];
	}
	else
	{
		pascal[n][k] = binomial(n-1, k, pascal) + binomial(n-1, k-1, pascal);
		return pascal[n][k];
	}
}

int main(void)
{
	int i, n, k;
	long **pascal;
	pascal = (long**)calloc(10000, sizeof(long*));
	for(i = 0; i < 10000; i++)
	{
		pascal[i] = (long*)calloc(10000, sizeof(long));
		pascal[i][0] = 1;
		pascal[i][i] = 1;
	}
	scanf("%d %d", &n, &k);
	printf("%ld\n", binomial(n, k, pascal));
	for(i = 0; i < 10000; i++) free(pascal[i]);
	free(pascal);
	return 0;
}
