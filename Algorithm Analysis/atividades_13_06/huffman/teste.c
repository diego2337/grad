#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	char *n = (char*)malloc(sizeof(char)*1);
	strcpy(n, "");
	char *bits = (char*)malloc(sizeof(char)*5);
	strcpy(bits, "0101");
	n = (char*)realloc(n, sizeof(char)*(strlen(bits)+1));
	strcat(n, bits);
	printf("n: %s\n", n);
	return 0;
}