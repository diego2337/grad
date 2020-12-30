#include <stdio.h>
#include <stdlib.h>

/*
	Read from a file until a given character occur
	params:
	   - file: file to be read
	   - delimiter: character that delimits reading
	returns:
	   - command read
*/
char *read(FILE *file, char delimiter)
{
	int count;
    char c;
    count = 0;
    char *line = NULL;
    do{
        c = fgetc(file);
        line = (char*)realloc(line, sizeof(char)*(count+1));
        line[count++] = c;
    }while(c != delimiter && c != EOF);
    line[count-1] = '\0';
    return line;
}

int main(void)
{
	int i, n;
	register char *str;
	scanf("%d\n", &n);
	for(i = 0; i < (n*7); i++)
	{
		str = read(stdin, '\n');
		if((i % 7) == 0)
		{
			printf("%s/", str);
		}
		else if((i % 7) == 1)
		{
			printf("%s/", str);
		}
		else if((i % 7) == 2)
		{
			printf("%s - ", str);
		}
		else if((i % 7) == 3)
		{
			printf("%s:", str);
		}
		else if((i % 7) == 4)
		{
			printf("%s:", str);
		}
		else if((i % 7) == 5)
		{
			printf("%s\n", str);
		}
		else if((i % 7) == 6)
		{
			printf("%s\n", str);
		}
		free(str);
		str = NULL;
	}
	return 0;
}