#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *output;
	unsigned char vector[14] = {127,128,180,182,187,190,127,128,75,77,68,72,127,127};
	output = fopen("teste.raw", "wb");
	fwrite(vector, 1, 14, output);
	return 0;
}