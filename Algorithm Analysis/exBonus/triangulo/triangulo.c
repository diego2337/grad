/*
	Program that, given 3 integer numbers representing triangle sides, checks if such triangle is right. 
	Author: Diego Cintra
	Date: March 23rd 2017
*/
#include <stdio.h>
#include <stdlib.h>

/* gcd: finds the greatest common divisor between a and b. */
int gcd(int a, int b)
{
	if(b == 0)
	{
		return a;
	}
	else
	{
		return gcd(b, a % b);
	}
}

int isRight(int x, int y, int z)
{
	if(x >= y && x >= z) // x is biggest
	{
		if(x*x == y*y + z*z)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(y >= x && y >= z) // y is biggest
	{
		if(y*y == x*x + z*z)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(z >= x && z >= y) // z is biggest
	{
		if(z*z == x*x + y*y)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int main(void)
{
	int x, y, z, greatest;
	/* Input for triangle sides */
	scanf("%d %d %d", &x, &y, &z);
	/* First step: find the greates common divisor between all of them (GCD) */
	greatest = gcd(x, y);
	greatest = gcd(greatest, z);
	/* Second step: simplify the elements */
	x = x / greatest;
	y = y / greatest;
	z = z / greatest;
	/* Third step: check if is right triangle */
	if((x != 0 && y != 0 && z != 0) && isRight(x, y, z))
	{
		printf("SIM");
	}
	else
	{
		printf("NAO");
	}
	// printf("%d", greatest);
	return 0;
}