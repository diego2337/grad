#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int *p;
	p = (int*)malloc(sizeof(int)*3);
	if(p[4])
	{
		printf("oi\n");
	}
	else
	{
		printf("ei\n");
	}
	return 0;
}