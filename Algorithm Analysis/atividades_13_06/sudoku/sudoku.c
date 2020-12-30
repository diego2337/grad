/*
	Program that finds a solution for a 16x16 sudoku board.
	Author: Diego S. Cintra
	USP Number: 10094043
	Date: 06/06/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>

enum{
	FIRST,
	SECOND
};

/*
	Struct for each cell of board
*/
typedef struct _cell{
	char c;
	char *possibleMoves;
	int possibleCount;
}cell;

/*
	Struct for sudoku
*/
typedef struct _sudoku{
	cell **board;
	int emptyPositions, r, c;
}sudoku;

/* 
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMat(char **mat, int c, int r)
{
    int i, j;
    for(i = 0; i < r; i++)
    {
		for(j = 0; j < c; j++)
        {
            printf("%c", mat[i][j]);
        }
        printf("\n");
    }
}

/*
	Dynamically allocates matrix
	params:
	   - c: number of columns;
	   - r: number of rows
	returns:
	   - allocated matrix
*/
char **allocMat(int c, int r)
{
	int i;
	char **board = NULL;
	board = (char**)calloc(r, sizeof(char*));
	for(i = 0; i < r; i++)
	{
		board[i] = (char*)calloc(c, sizeof(char));
	}
	return board;
}

/*
	Read matrix from ".mat" file
	params:
	   - sudoku structure
	returns:
	   - sudoku matrix
*/
char **readMat(char *path, int *c, int *r)
{
	int i, j;
	char **board = NULL;
	(*c) = 16;
	(*r) = 16;
	board = allocMat((*c), (*r));
	for(i = 0; i < (*r); i++)
	{
		for(j = 0; j < (*c); j++)
		{
			fscanf(stdin, "%c", &board[i][j]);
		}
		fscanf(stdin, "\n");
	}
	return board;
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMat(char **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

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


/* 
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printCell(cell **mat, int c, int r)
{
    int i, j;
    for(i = 0; i < r; i++)
    {
		for(j = 0; j < c; j++)
        {
            printf("%c", mat[i][j].c);
        }
        printf("\n");
    }
}

/*
	Print cell struct possibleMoves
	params:
	   - board: cell **struct;
	   - r: board number of rows;
	   - c: board number of columns
*/
void printPossibleMoves(sudoku *s)
{
	int i, j;
	for(i = 0; i < s->r; i++)
	{
		for(j = 0; j < s->c; j++)
		{
			printf("char: %c\tpossibleMoves: %s\n", s->board[i][j].c, s->board[i][j].possibleMoves);
		}
	}
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeCell(cell **matrix, int n)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
    	for(j = 0; j < n; j++)
    	{
    		free(matrix[i][j].possibleMoves);
    	}
        free(matrix[i]);
    }
    free(matrix);
}

/*
	Find if given character is in vector
	params:
	   - vector: vector;
	   - size; vector size;
	   - c: character to be found
	returns:
	   - (index) if found, (-1) otherwise
*/
int findInVector(char *vector, int size, char c)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(vector[i] == c)
		{
			return i;
		}
	}
	return -1;
}

cell **allocCell(int c, int r)
{
	int i, j;
	cell **b;
	b = (cell**)calloc(r, sizeof(cell*));
	for(i = 0; i < r; i++)
	{
		b[i] = (cell*)calloc(c, sizeof(cell));
		for(j = 0; j < c; j++)
		{
			b[i][j].possibleMoves = (char*)calloc(2, sizeof(char));
			strcpy(b[i][j].possibleMoves, "");
		}
	}
	return b;
}

void readIntoSudoku(sudoku **s, char **board)
{
	int i, j;
	(*s)->board = allocCell((*s)->c, (*s)->r);
	for(i = 0; i < (*s)->r; i++)
	{
		for(j = 0; j < (*s)->c; j++)
		{
			(*s)->board[i][j].c = board[i][j];
			if(board[i][j] == '.')
			{
				(*s)->emptyPositions = (*s)->emptyPositions + 1;
			}
		}
	}
}

/*
	Check all possible moves for a given sudoku line
	params:
	   - s: sudoku struct;
	   - i: cell row wise position;
	   - j: cell column wise position;
	   - possibleMoves: char *vector with all possibilities
*/
void checkLine(sudoku *s, int i, int j, char **possibleMoves)
{
	int k, l;
	/*char possibleMoves[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};*/
	for(k = 0; k < s->c; k++)
	{
		if(j != k)
		{
			l = findInVector((*possibleMoves), 16, s->board[i][k].c);
			if(l >= 0)
			{
				(*possibleMoves)[l] = '-';
			}
		}
	}
}

/*
	Check all possible moves for a given sudoku column
	params:
	   - s: sudoku struct;
	   - i: cell row wise position;
	   - j: cell column wise position;
	   - possibleMoves: char *vector with all possibilities
*/
void checkColumn(sudoku *s, int i, int j, char **possibleMoves)
{
	int k, l;
	for(k = 0; k < s->r; k++)
	{
		if(i != k)
		{
			l = findInVector((*possibleMoves), 16, s->board[k][j].c);
			if(l >= 0)
			{
				(*possibleMoves)[l] = '-';
			}
		}
	}
}

/*
	Check all possible moves inside a block
	params:
	   - s: sudoku struct;
	   - i: cell row wise position;
	   - j: cell column wise position;
	   - sudokuSize: size of sudoku - 9x9, 16x16...;
	   - possibleMoves: char *vector with all possibilities
*/
void checkBlock(sudoku *s, int i, int j, int sudokuSize, char **possibleMoves)
{
	/* Find block */
	int block = sqrt(sudokuSize), rowBlock, columnBlock, k, l, m;
	rowBlock = (sudokuSize-1)-i;
	columnBlock = (sudokuSize-1)-j;
	/* Find block corner */
	rowBlock = i - (i % block);
	columnBlock = j - (j % block);
	/* Find possible moves inside block */
	for(k = rowBlock; k < (rowBlock+block); k++)
	{
		for(l = columnBlock; l < (columnBlock+block); l++)
		{
			if(i != k || j != l)
			{
				m = findInVector((*possibleMoves), 16, s->board[k][l].c);
				if(m >= 0)
				{
					(*possibleMoves)[m] = '-';
				}
			}
		}
	}
}

void freePossibleMoves(sudoku **s)
{
	int i, j;
	for(i = 0; i < (*s)->r; i++)
	{
		for(j = 0; j < (*s)->c; j++)
		{
			free((*s)->board[i][j].possibleMoves);
			(*s)->board[i][j].possibleMoves = (char*)calloc(2, sizeof(char));
			strcpy((*s)->board[i][j].possibleMoves, "");
			(*s)->board[i][j].possibleCount = 0;
		}
	}
}


void initPossibleMoves(sudoku **s)
{
	int i, j, k;
	char *possibleMoves = NULL;
	for(i = 0; i < (*s)->r; i++)
	{
		for(j = 0; j < (*s)->c; j++)
		{
			/*possibleMoves = fillPossibleMoves();*/
			if((*s)->board[i][j].c == '.')
			{
				possibleMoves = (char*)calloc(17, sizeof(char));
				strcpy(possibleMoves, "0123456789ABCDEF");
				checkLine((*s), i, j, &possibleMoves);
				checkColumn((*s), i, j, &possibleMoves);
				checkBlock((*s), i, j, 16, &possibleMoves);
				for(k = 0; k < 16; k++)
				{
					if(possibleMoves[k] != '-')
					{
						(*s)->board[i][j].possibleMoves = (char*)realloc((*s)->board[i][j].possibleMoves, sizeof(char)*((*s)->board[i][j].possibleCount+1));
						(*s)->board[i][j].possibleMoves[(*s)->board[i][j].possibleCount] = possibleMoves[k];
						(*s)->board[i][j].possibleCount = (*s)->board[i][j].possibleCount + 1;
					}
				}
				(*s)->board[i][j].possibleMoves = (char*)realloc((*s)->board[i][j].possibleMoves, sizeof(char)*((*s)->board[i][j].possibleCount+1));
				(*s)->board[i][j].possibleMoves[(*s)->board[i][j].possibleCount] = '\0';
				(*s)->board[i][j].possibleCount = (*s)->board[i][j].possibleCount + 1;
				free(possibleMoves);
				possibleMoves = NULL;
			}
		}
	}
}

void initSudoku(sudoku **s)
{
	char **board = NULL;
	/* Read into matrix */
	board = readMat(NULL, &(*s)->c, &(*s)->r);
	readIntoSudoku(&(*s), board);
	freeMat(board, (*s)->r);
	initPossibleMoves(&(*s));
}

void freeSudoku(sudoku **s)
{
	freeCell((*s)->board, (*s)->r);
	free((*s));
}

/*
	Find and assign to params best option
	params:
	   - s: sudoku struct;
	   - i: row from cell best option;
	   - j: column from cell best option
*/
void getBestOption(sudoku *s, int *i, int *j)
{
	int k, l, max = INT_MAX;
	for(k = 0; k < s->r; k++)
	{
		for(l = 0; l < s->c; l++)
		{
			if(s->board[k][l].possibleCount != 0 && s->board[k][l].possibleCount < max)
			{
				(*i) = k;
				(*j) = l;
				max = s->board[k][l].possibleCount;
			}
		}
	}
}

/*
	Execute sudoku, obtaining best option and running
	param:
	   - s: sudoku struct
*/
void executeSudoku(sudoku *s)
{
	int i, j;
	while(s->emptyPositions > 0)
	{
		/* Get best option - lowest number of possibilities */
		getBestOption(s, &i, &j);
		s->board[i][j].c = s->board[i][j].possibleMoves[FIRST];
		s->emptyPositions = s->emptyPositions - 1;
		/*printCell(s->board, s->c, s->r);
		printf("\n");*/
		freePossibleMoves(&s);
		initPossibleMoves(&s);
		/*printPossibleMoves(s);*/
	}
	printCell(s->board, s->c, s->r);
}

int main(void)
{
	sudoku *s;

	/* Step 1: Initialize sudoku */
	s = (sudoku*)calloc(1, sizeof(sudoku));
	initSudoku(&s);
	/* Step 2: Make a move */
	executeSudoku(s);
	
	/* Step ???: Free variables */
	freeSudoku(&s);
	return 0;
}