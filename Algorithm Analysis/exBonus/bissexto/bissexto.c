#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int n;
	scanf("%d", &n);
	if((n % 4) == 0 && (n % 10) != 0)
	{
		printf("SIM");
	}
	else if((n % 400) == 0)
	{
		printf("SIM");
	}
	else
	{
		printf("NAO");
	}
	printf("\n");
	return 0;
}