/*
    Program that takes multiple commands for a minesweeper game.
    Author: Diego Cintra
    USP Number: 10094043
    Date: 07/04/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMatrix(char **matrix, int n, int m)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
        {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMatrix(char **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

/*
    Read file name
    returns:
       - line containing file name
*/
char *readFileName()
{
    int count;
    char c;
    count = 0;
    char *fileName = NULL;
    // fileName = NULL;
    do{
        c = fgetc(stdin);
        fileName = (char*)realloc(fileName, sizeof(char)*(count+1));
        fileName[count++] = c;
    }while(c != '\n' && c != '\r');
    fileName[count-1] = '\0';
    return fileName;
}

/*
    Read and return mine
    param:
       - file: file to read line from
       - i: number of rows (to be updated during function execution)
       - j: number of columns (to be updated during function execution)
    returns:
       - a char type matrix containing minesweeper board
*/
char **readMine(FILE *file, int *i, int *j)
{
    int m = 1;
    char c, **board;
    *i = 0;
    *j = 0;
    board = NULL;
    c = fgetc(file);
    do
    {
        if(c == '\r')
        {
            /* Read '\n' character */
            c = fgetc(file);
        }
        if(c == '\n')
        {
            /* Allocate new row, reset j counter, increase i counter, create NULL char* row */
            *i = *i + 1;
            if(m == 1)
            {
                m = 0;
                m = *j;
            }
            *j = 0;
            board = (char**)realloc(board, sizeof(char*)*(*i+1));
            board[(*i)] = NULL;
        }
        else
        {
            /* Allocate new column, increase j counter */
            if(board == NULL)
            {
                board = (char**)malloc(sizeof(char*)*1);
                board[(*i)] = NULL;
            }
            board[(*i)] = (char*)realloc(board[(*i)], sizeof(char)*(*j+1));
            board[(*i)][(*j)] = c;
            /* Adjusting m dimension - by end of do-while, will already contain correct m */
            *j = *j + 1;
        }
        c = fgetc(file);
    }while(!feof(file));
    free(board[(*i)]);
    *j = m;
    /* Adjust i counter so that both i and j become n x m dimensions */
    // *i = *i + 1;
    return board;
}

/*
    Check given position regarding mines
    params:
       - board: minesweeper board
       - n: number of rows
       - m: number of columns
       - i: x position
       - j: y position
    returns:
       - number to fill board cell at given position
*/
char checkPosition(char **board, int n, int m, int i, int j)
{
    int count = 0;
    /* Check to see if (i,j) position isn't a mine */
    if(board[i][j] == '*')
    {
        return '*';
    }
    else
    {
        /* Check north */
        if((i-1) >= 0)
        {
            if(board[i-1][j] == '*')
            {
                count = count + 1;
            }
        }
        /* Check northeast */
        if((i-1) >= 0 && (j+1) < m)
        {
            if(board[i-1][j+1] == '*')
            {
                count = count + 1;
            }
        }
        /* Check east */
        if((j+1) < m)
        {
            if(board[i][j+1] == '*')
            {
                count = count + 1;
            }
        }
        /* Check southeast */
        if((i+1) < n && (j+1) < m)
        {
            if(board[i+1][j+1] == '*')
            {
                count = count + 1;
            }
        }
        /* Check south */
        if((i+1) < n)
        {
            if(board[i+1][j] == '*')
            {
                count = count + 1;
            }
        }
        /* Check southwest */
        if((i+1) < n && (j-1) >= 0)
        {
            if(board[i+1][j-1] == '*')
            {
                count = count + 1;
            }
        }
        /* Check west */
        if((j-1) >= 0)
        {
            if(board[i][j-1] == '*')
            {
                count = count + 1;
            }
        }
        /* Check northwest */
        if((i-1) >= 0 && (j-1) >= 0)
        {
            if(board[i-1][j-1] == '*')
            {
                count = count + 1;
            }
        }
        // printf("%d\t%c\n", count, count+48);
        return count != 0 ? (count+48) : '.';
    }
}

/*
    Fill mine with hints
    params:
       - board: minesweeper board
       - n: number of rows
       - m: number of columns
*/
void fillMine(char **board, int n, int m)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < m; j++)
        {
            board[i][j] = checkPosition(board, n, m, i, j);
        }
    }
}

/*
    Recursively reveal minesweeper board
    params:
       - minesweeper: X filled matrix to be opened
       - board: original minesweeper board to check
       - n: number of rows
       - m: number of columns
       - x: x position of board
       - y: y position of board
*/
void open(char **minesweeper, char **board, int n, int m, int x, int y)
{
    if((x >= 0) && (x < n) && (y >= 0) && (y < m))
    {
        // printf("x:%d\ty:%d\n", x, y);
        if(board[x][y] == '.' && minesweeper[x][y] != board[x][y])
        {
            /* Mark position as visited */
            minesweeper[x][y] = '.';
            /* Recursively check adjacent positions */
            open(minesweeper, board, n, m, (x-1), y);
            open(minesweeper, board, n, m, (x-1), (y+1));
            open(minesweeper, board, n, m, x, (y+1));
            open(minesweeper, board, n, m, (x+1), (y+1));
            open(minesweeper, board, n, m, (x+1), y);
            open(minesweeper, board, n, m, (x+1), (y-1));
            open(minesweeper, board, n, m, x, (y-1));
            open(minesweeper, board, n, m, (x-1), (y-1));
        }
        else 
        {
            minesweeper[x][y] = board[x][y];
            return;
        }
    }
    else
    {
        return;
    }
}

/*
    Reveal mine position
    params:
       - board: minesweeper board
       - n: number of rows
       - m: number of columns
       - x: x position of board
       - y: y position of board
*/
void checkPos(char **board, int n, int m, int x, int y)
{
    int i, j;
    char **minesweeper;
    /* Build matrix filled with 'X' */
    minesweeper = (char**)malloc(sizeof(char*)*n);
    for(i = 0; i < n; i++)
    {
        minesweeper[i] = (char*)malloc(sizeof(char)*m);
        for(j = 0; j < m; j++)
        {
            minesweeper[i][j] = 'X';
        }
    }
    /* Check board for position - three different cases */
    switch(board[x][y])
    {
        case '*':
        /* Game over: print board */
        printMatrix(board, n, m);
        break;

        case '.':
        /* Recursively opens board */
        open(minesweeper, board, n, m, x, y);
        printMatrix(minesweeper, n, m);
        break;

        default: 
        /* If it's a number */
        minesweeper[x][y] = board[x][y];
        printMatrix(minesweeper, n, m);
        break;
    }
}

/* Main function to execute */
int main(void)
{
    FILE *file;
    int n, m, x, y, opt;
    char *fileName, **board;
    /* Step 1: read stdin command and file containing board */
    scanf("%d\n", &opt);
    fileName = NULL;
    board = NULL;
    fileName = readFileName();
    // printf("%s", fileName);
    /* Step 2: open file */
    file = fopen(fileName, "r");
    if(file)
    {
        /* Step 3: read file contents */
        board = readMine(file, &n, &m);
        /* Step 4: switch command option */
        switch(opt)
        {
            case 1:
            /* Option 1: print the given matrix */
            printMatrix(board, n, m);
            break;

            case 2:
            /* Option 2: fill with hints */
            fillMine(board, n, m);
            printMatrix(board, n, m);
            break;

            case 3:
            /* Option 3: check position */
            scanf("%d %d", &x, &y);
            fillMine(board, n, m);
            checkPos(board, n, m, x, y);
            break;

            default:
            printf("Unrecognized command.\n");
            break;
        }
        free(fileName);
        freeMatrix(board, n);
        fclose(file);
    }
    else
    {
        printf("File not found.\n");
    }
    return 0;
}