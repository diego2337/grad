/* Void * cast test */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	void *test;
	test = NULL;
	test = (void*)realloc(sizeof(void)*3);
	test[0] = (int*)1;
	test[1] = (char*)"oi";
	return 0;
}