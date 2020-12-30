#include <stdio.h>
#include <stdlib.h>

long fibonacci(long n, long **cache)
{
	if((*cache)[n] != -1) return (*cache)[n];
	else
	{
		(*cache)[n] = fibonacci(n-1, cache) + fibonacci(n-2, cache);
		return (*cache)[n];
	}
}

int main(void)
{
	int i;
	long n, *cache;
	scanf("%ld", &n);
	cache = (long*)calloc(n+1, sizeof(long));
	for(i = 0; i < n+1; i++) cache[i] = -1;
	cache[0] = 0;
	cache[1] = 1;
	printf("%ld\n", fibonacci(n, &cache));
	for(i = 0; i < n; i++)
	{
		printf("%ld ", cache[i]);
	}
	printf("\n");
	free(cache);
	return 0;
}
