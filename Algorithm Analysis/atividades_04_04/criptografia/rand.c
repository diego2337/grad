/*
    Program that generates a random sequence of characters, of quadratic size.
    Author: Diego Cintra
    USP number: 10094043
    Date: 30/03/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand(time(NULL));
    char *str;
    int i, n, aux;
    n = 5;
    str = malloc(sizeof(char) * (n*n));
    for(i = 0; i <= n*n; i++)
    {
        aux = rand() % 3 + 1;
        if(aux == 1)
        {
            str[i] = abs(abs(rand() % 63) + 32);
            str[i] = abs(str[i]);
            printf("str1: %d\n", str[i]);
        }
        else if(aux == 2)
        {
            str[i] = abs(abs(rand() % 93) + 64);
            str[i] = abs(str[i]);
            printf("str2: %d\n", str[i]);
        }
        else
        {
            str[i] = abs(rand() % 122 + 97);
            str[i] = abs(str[i]);
            printf("str3: %d\n", abs(str[i]));
        }
    }
    printf("%s\n", str);
    free(str);
}