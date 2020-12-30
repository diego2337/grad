/*
	Program that reads FEN notation chess input and prints all possible moves (and conditions) to a given player.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 19/04/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	Structs representing each piece
	general params:
	   - row, column: position at board
	   - pointer to function: function that will check its possible moves
*/
/* ------------------------------------------------------------------------------------------------------------ */

/*
	Struct for pawn
*/
typedef struct _pawn{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _pawn*, int, int, char);
	char **(*checkMoves)(struct _pawn**, char**, char*, char*);
}pawn;

/*
	Struct for knight
*/
typedef struct _knight{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _knight*, int, int, char);
	char **(*checkMoves)(struct _knight**, char**, char*, char*);
}knight;

/*
	Struct for bishop
*/
typedef struct _bishop{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _bishop*, int, int, char);
	char **(*checkMoves)(struct _bishop**, char**, char*, char*);
}bishop;

/*
	Struct for queen
*/
typedef struct _queen{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _queen*, int, int, char);
	char **(*checkMoves)(struct _queen**, char**, char*, char*);
}queen;

/*
	Struct for king
*/
typedef struct _king{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _king*, int, int, char);
	char **(*checkMoves)(struct _king**, char**, char*, char*);
}king;

/*
	Struct for rook
*/
typedef struct _rook{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _rook*, int, int, char);
	char **(*checkMoves)(struct _rook**, char**, char*, char*);
}rook;


/* ------------------------------------------------------------------------------------------------------------ */

/*
	Function declaration of possible moves for each piece
*/

/* ------------------------------------------------------------------------------------------------------------ */

char **checkMovesPawn(pawn **p, char **board, char *castling, char *enPassant)
{
	int count = 0;
	char **possibilities;
	possibilities = NULL;
	possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
	count = count + 1;
	switch((*p)->isWhite)
	{
		case 'Y':
			/* Check if forward is clear */
			if((*p)->row+1 < 8 && board[(*p)->row+1][(*p)->column] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = ((*p)->row)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Check if pawn is at beggining */
			if((*p)->row == 2 && (*p)->row+2 < 8 && board[(*p)->row+2][(*p)->column] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = ((*p)->row)+48 + 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Check pawn attacks */
			/* Left attack */
			if((*p)->row+1 < 8 && (*p)->column-1 >= 0 && board[(*p)->row+1][(*p)->column-1] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = ((*p)->column)+97 - 1;
				possibilities[count][2] = ((*p)->row)+97 + 1;
				count = count + 1;
			}
			/* Right attack */
			if((*p)->row+1 < 8 && (*p)->column+1 < 8 && board[(*p)->row+1][(*p)->column+1] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = ((*p)->column)+97 + 1;
				possibilities[count][2] = ((*p)->row)+97 + 1;
				count = count + 1;
			}
			/* En Passant */
		break;

		case 'N':
			/* Check if forward is clear */
			if((*p)->row-1 >= 0 && board[(*p)->row-1][(*p)->column] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = ((*p)->row)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Check if pawn is at beggining */
			if((*p)->row == 7 && (*p)->row-2 >= 0 && board[(*p)->row-1][(*p)->column] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = ((*p)->row)+48 - 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Check pawn attacks */
			/* Left attack */
			if((*p)->row-1 < 8 && (*p)->column-1 >= 0 && board[(*p)->row-1][(*p)->column-1] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = ((*p)->column)+97 - 1;
				possibilities[count][2] = ((*p)->row)+97 - 1;
				count = count + 1;
			}
			/* Right attack */
			if((*p)->row-1 < 8 && (*p)->column+1 < 8 && board[(*p)->row-1][(*p)->column+1] == '-')
			{
				possibilities[count] = (char*)realloc(possibilities, sizeof(char)*4);
				possibilities[count][0] = ((*p)->column)+97;
				possibilities[count][1] = ((*p)->column)+97 + 1;
				possibilities[count][2] = ((*p)->row)+97 - 1;
				count = count + 1;
			}
			/* En Passant */
		break;

		default:
		break;
	}
	(*p)->possibleMoves = (count-1);
	return possibilities;
}

char **checkMovesKnight(knight **n, char **board, char *castling, char *enPassant)
{
	char **possibilities;
	possibilities = NULL;

	return possibilities;
}

char **checkMovesBishop(bishop **b, char **board, char *castling, char *enPassant)
{
	char **possibilities;
	possibilities = NULL;

	return possibilities;
}

char **checkMovesQueen(queen **q, char **board, char *castling, char *enPassant)
{
	char **possibilities;
	possibilities = NULL;

	return possibilities;
}

char **checkMovesKing(king **k, char **board, char *castling, char *enPassant)
{
	char **possibilities;
	possibilities = NULL;

	return possibilities;
}

char **checkMovesRook(rook **r, char **board, char *castling, char *enPassant)
{
	char **possibilities;
	possibilities = NULL;

	return possibilities;
}

/* ------------------------------------------------------------------------------------------------------------ */

/*
	Initialization of piece structs
*/

/* ------------------------------------------------------------------------------------------------------------ */

void initPawn(pawn *p, int i, int j, char isWhite)
{
	p->row = i;
	p->column = j;
	p->possibleMoves = 0;
	p->isWhite = isWhite;
	p->checkMoves = (**checkMovesPawn);
}

void initKnight(knight *k, int i, int j, char isWhite)
{
	k->row = i;
	k->column = j;
	k->possibleMoves = 0;
	k->isWhite = isWhite;
	k->checkMoves = (**checkMovesKnight);
}

void initBishop(bishop *b, int i, int j, char isWhite)
{
	b->row = i;
	b->column = j;
	b->possibleMoves = 0;
	b->isWhite = isWhite;
	b->checkMoves = (**checkMovesBishop);
}

void initQueen(queen *q, int i, int j, char isWhite)
{
	q->row = i;
	q->column = j;
	q->possibleMoves = 0;
	q->isWhite = isWhite;
	q->checkMoves = (**checkMovesQueen);
}

void initKing(king *k, int i, int j, char isWhite)
{
	k->row = i;
	k->column = j;
	k->possibleMoves = 0;
	k->isWhite = isWhite;
	k->checkMoves = (**checkMovesKing);
}

void initRook(rook *r, int i, int j, char isWhite)
{
	r->row = i;
	r->column = j;
	r->possibleMoves = 0;
	r->isWhite = isWhite;
	r->checkMoves = (**checkMovesRook);
}

/* ------------------------------------------------------------------------------------------------------------ */

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
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMatrix(char **matrix, int n, int m)
{
    int i, j;
    if(m != -1)
    {
    	for(i = 0; i < n; i++)
	    {
    		for(j = 0; j < m; j++)
	        {
	            printf("%c", matrix[i][j]);
	        }
	        printf("\n");
	    }
    }
    else
    {
    	for(i = 0; i  < n; i++)
    	{
    		for(j = 0; j < strlen(matrix[n]); j++)
    		{
    			printf("%c", matrix[i][j]);
    		}
    		printf("\n");
    	}
    }
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
	Initialize chess board
	param:
	   - input: string containing board initialization
	   - size: chess size
	returns:
	   - annotated board
*/
char **initializeBoard(char *input, int size)
{
	int i, j, k, zeroCount;
	char **board;
	zeroCount = 0;
	board = (char**)malloc(sizeof(char*)*size);
	for(i = 0; i < size; i++)
	{
		board[i] = (char*)malloc(sizeof(char)*size);
	}
	for(i = 0, k = 0; i < size && k < (strlen(input)); i++)
	{
		for(j = 0; j < size && k < (strlen(input)); j++)
		{
			/* Indicates that there are still spaces without pieces to be filled */
			if(zeroCount != 0)
			{
				board[i][j] = '-';
				zeroCount = zeroCount - 1;
			}
			else if(input[k] >= 48 && input[k] <= 57) /* Indicates spaces without pieces */
			{
				zeroCount = ((input[k]-'0')-1);
				board[i][j] = '-';
				k = k + 1;
			}
			else
			{
				board[i][j] = input[k];
				k = k + 1;
			}
		}
		k = k + 1;
	}
	return board;
}

int main(void)
{
	/* Local variable declarations */
	/* i, j: loop control variables */
	int i, j;
	/* input: string to store commands passed through stdin; board: char type matrix board to annotate elements at given positions;
		board: matrix to store pieces positions
		turn: store current turn
		castling: store castling information

	*/
	char *input, **board, turn, *castling, *enPassant, **printM;
	/*
		Structures representing pieces
		pawn, rooks, bishops...: containing each piece in a given board position;
	*/
	pawn *bP, *wP;
	knight *bN, *wN;
	bishop *bB, *wB;
	queen *bQ, *wQ;
	king *bK, *wK;
	rook *bR, *wR;
	
	/* Step 1: Read input */
	/* Step 1.1: Read chess board */
	input = read(stdin, ' ');
	/* Step 1.2: Create pieces and initialize board */
	board = initializeBoard(input, 8);
	// printMatrix(board, 8, 8);
	bP = (pawn*)malloc(sizeof(pawn)*1);
	wP = (pawn*)malloc(sizeof(pawn)*1);
	bN = (knight*)malloc(sizeof(knight)*1);
	wN = (knight*)malloc(sizeof(knight)*1);
	bB = (bishop*)malloc(sizeof(bishop)*1);
	wB = (bishop*)malloc(sizeof(bishop)*1);
	bQ = (queen*)malloc(sizeof(queen)*1);
	wQ = (queen*)malloc(sizeof(queen)*1);
	bK = (king*)malloc(sizeof(king)*1);
	wK = (king*)malloc(sizeof(king)*1);
	bR = (rook*)malloc(sizeof(rook)*1);
	wR = (rook*)malloc(sizeof(rook)*1);
	/* Step 1.3: Initialize pieces structs */
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			switch(board[i][j])
			{
				case 'p':
					bP->init = (*initPawn);
					bP->init(bP, i, j, 'N');
				break;

				case 'P':
					wP->init = (*initPawn);
					wP->init(wP, i, j, 'Y');
				break;

				case 'n':
					bN->init = (*initKnight);
					bN->init(bN, i, j, 'N');
				break;

				case 'N':
					wN->init = (*initKnight);
					wN->init(wN, i, j, 'Y');
				break;

				case 'b':
					bB->init = (*initBishop);
					bB->init(bB, i, j, 'N');
				break;

				case 'B':
					wB->init = (*initBishop);
					wB->init(wB, i, j, 'Y');
				break;

				case 'q':
					bQ->init = (*initQueen);
					bQ->init(bQ, i, j, 'N');
				break;

				case 'Q':
					wQ->init = (*initQueen);
					wQ->init(wQ, i, j, 'Y');
				break;

				case 'k':
					bK->init = (*initKing);
					bK->init(bK, i, j, 'N');
				break;

				case 'K':
					wK->init = (*initKing);
					wK->init(wK, i, j, 'Y');
				break;

				case 'r':
					bR->init = (*initRook);
					bR->init(bR, i, j, 'N');
				break;

				case 'R':
					wR->init = (*initRook);
					wR->init(wR, i, j, 'Y');
				break;

				default: /* Not a piece; do nothing */
				break;
			}
		}
	}

	/* Step 2: read turn */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	turn = input[0];

	/* Step 3: read castling */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	/* Step 3.1: stores only possible castlings for current turn */
	castling = (char*)malloc(sizeof(char)*2);
	if(turn == 'w')
	{
		castling[0] = input[0];
		castling[1] = input[1];
	}
	else
	{
		castling[0] = input[0];
		castling[1] = input[1];
	}

	/* Step 4: read en passant */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	enPassant = (char*)malloc(sizeof(char)*2);
	strcpy(enPassant, input);

	/* Step 5: check for all available moves */
	if(turn == 'w')
	{
		printM = wP->checkMoves(&wP, board, castling, enPassant);
		printMatrix(printM, wP->possibleMoves, -1);
		wN->checkMoves(&wN, board, castling, enPassant);
		wB->checkMoves(&wB, board, castling, enPassant);
		wQ->checkMoves(&wQ, board, castling, enPassant);
		wK->checkMoves(&wK, board, castling, enPassant);
		wR->checkMoves(&wR, board, castling, enPassant);
	}
	else
	{
		bP->checkMoves(&bP, board, castling, enPassant);
		bN->checkMoves(&bN, board, castling, enPassant);
		bB->checkMoves(&bB, board, castling, enPassant);
		bQ->checkMoves(&bQ, board, castling, enPassant);
		bK->checkMoves(&bK, board, castling, enPassant);
		bR->checkMoves(&bR, board, castling, enPassant);
	}
	
	/* Step ???: Free variables */
	freeMatrix(board, 8);
	free(input);
	free(castling);
	free(enPassant);
	free(printM);
	free(bP);
	free(wP);
	free(bN);
	free(wN);
	free(bB);
	free(wB);
	free(bQ);
	free(wQ);
	free(bK);
	free(wK);
	free(bR);
	free(wR);
	return 0;
}