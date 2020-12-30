/*
	Program that reads FEN notation chess input, sequence of moves and check possible win or draw situations to a given player.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 10/05/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
	Structs representing each piece
	general params:
	   - row, column: position at board
	   - pointer to function: function that will check its possible moves
*/
/* ------------------------------------------------------------------------------------------------------------ */

/*
	Struct for king
*/
typedef struct _king{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _king*, int, int, char);
	char **(*checkMoves)(struct _king*, char**, char*, char*);
	void (*updatePosition)(struct _king*, int, int);
}king;

/*
	Struct for pawn
*/
typedef struct _pawn{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _pawn*, int, int, char);
	char **(*checkMoves)(struct _pawn*, king*, char**, char*, char*, int);
	void (*updatePosition)(struct _pawn*, int, int);
}pawn;

/*
	Struct for knight
*/
typedef struct _knight{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _knight*, int, int, char);
	char **(*checkMoves)(struct _knight*, king*, char**, char*, char*);
	void (*updatePosition)(struct _knight*, int, int);
}knight;

/*
	Struct for bishop
*/
typedef struct _bishop{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _bishop*, int, int, char);
	char **(*checkMoves)(struct _bishop*, king*, char**, char*, char*, int);
	void (*updatePosition)(struct _bishop*, int, int);
}bishop;

/*
	Struct for queen
*/
typedef struct _queen{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _queen*, int, int, char);
	char **(*checkMoves)(struct _queen*, king*, char**, char*, char*, int);
	void (*updatePosition)(struct _queen*, int, int);
}queen;

/*
	Struct for rook
*/
typedef struct _rook{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _rook*, int, int, char);
	char **(*checkMoves)(struct _rook*, king*, char**, char*, char*, int);
	void (*updatePosition)(struct _rook*, int, int);
}rook;

/*
	Enum to identify pieces positions in "nPieces" vector
*/
enum
{
	BLACKPAWN,
	WHITEPAWN,
	BLACKKNIGHT,
	WHITEKNIGHT,
	BLACKBISHOP,
	WHITEBISHOP,
	BLACKQUEEN,
	WHITEQUEEN,
	BLACKKING,
	WHITEKING,
	BLACKROOK,
	WHITEROOK
};

/*
	Sums a vector
	params:
	   - vector: int vector to be summed;
	   - size: vector size
	returns:
	   - sum of elements
*/
int sumVector(int *vector, int size)
{
	int i, sum;
	sum = 0;
	for(i = 0; i < size; i++)
	{
		sum = sum + vector[i];
	}
	return sum;
}

/*
	Converts a null terminating string to integer
	param:
	   - number: char type number to be converted
	returns:
	   - int type number
*/
int toNum(char *number)
{
	int i, num = 0;
	for(i = 0; i < strlen(number); i++)
	{
		num = ((num)*10) + (number[i]-48);
	}
	return num;
}

/*
    Deallocates matrix from heap memory
    params:
       - matrix: the matrix to be freed
       - n: matrix's size
*/
void freeMatrix0(int **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
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
    Prints matrix 
    params:
       - matrix: the matrix to be printed
       - n: matrix line's length
       - m: matrix column's length
*/
void printMatrix0(int **matrix, int n, int m)
{
	int i, j;
	for(i = 0; i < n; i++)
	{
    	for(j = 0; j < m; j++)
	    {
	        printf("%d", matrix[i][j]);
	    }
	    printf("\n");
	}
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
    	for(i = 0; i < n; i++)
    	{
    		for(j = 0; j < strlen(matrix[i]); j++)
    		{
    			printf("%c", matrix[i][j]);
    		}
    		printf("\n");
    	}
    }
}

/*
	Check if king is in check by knights
	params:
	   - k: king struct
	   - board: chess board
	   - knightPiece: knight piece
	returns:
	   - (1) if any knight puts a king in check, (0) otherwise
*/
int checkCheckKnights(king *k, char **board, char knightPiece)
{
	int i, j, check = 0;
	/* North */
	i = k->row+2;
	j = k->column;
	if(i < 8 && i >= 0 && (j+1) < 8 && (j-1) >= 0 && (board[i][j+1] == knightPiece || board[i][j-1] == knightPiece))
	{
		check = check + 1;
	}
	/* East */
	i = k->row;
	j = k->column+2;
	if((i+1) < 8 && (i-1) >= 0 && j < 8 && j >= 0 && (board[i+1][j] == knightPiece || board[i-1][j] == knightPiece))
	{
		check = check + 1;
	}
	/* South */
	i = k->row-2;
	j = k->column;
	if(i < 8 && i >= 0 && (j+1) < 8 && (j-1) >= 0 && (board[i][j+1] == knightPiece || board[i][j-1] == knightPiece))
	{
		check = check + 1;
	}
	/* West */
	i = k->row;
	j = k->column-2;
	if((i+1) < 8 && (i-1) >= 0 && j < 8 && j >= 0 && (board[i+1][j] == knightPiece || board[i-1][j] == knightPiece))
	{
		check = check + 1;
	}
	/* If none of above is possible, no check. Return 0 */
	return check;
}

/*
	Check to see if king is in check
	params:
	   - k: king struct
	   - board: chess board
	returns:
	   - (1) if check, (0) if check
*/
int checkCheck(king *k, char **board)
{
	int i, j, check = 0;
	char pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece, kingPiece;
	switch(k->isWhite)
	{
		case 'Y':
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
			kingPiece = 'k';
		break;

		case 'N':
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
			kingPiece = 'K';
		break;

		default:
		break;
	}
	/* Check how many possible moves are possible for given king */
	// moves = checkNumberOfMoves(k, board, NULL);

	/* Check north, west, east and south for either rook or queen */
	/* North */
	for(i = k->row+1, j = k->column; i < 8; i++)
	{
		/* Check enemy king */
		if(i == k->row+1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		/* Must be either rook or queen; else, another piece. Not in check */
		if((board[i][j] == queenPiece || board[i][j] == rookPiece))
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			i = 9;
		}
	}
	/* East */
	for(i = k->row, j = k->column+1; j < 8; j++)
	{
		/* Check enemy king */
		if(j == k->column+1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == rookPiece)) /* Must be either rook or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			j = 9;
		}
	}
	/* South */
	for(i = k->row-1, j = k->column; i >= 0; i--)
	{
		/* Check enemy king */
		if(i == k->row-1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == rookPiece)) /* Must be either rook or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			i = -1;
		}
	}
	/* West */
	for(i = k->row, j = k->column-1; j >= 0; j--)
	{
		/* Check enemy king */
		if(j == k->column-1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == rookPiece)) /* Must be either rook or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			j = -1;
		}
	}

	/* Check for knights */
	check = check + checkCheckKnights(k, board, knightPiece);

	/* Check for pawns */
	switch(k->isWhite)
	{
		case 'Y':
			i = k->row+1;
		break;

		case 'N':
			i = k->row-1;
		break;

		default:
		break;
	}
	j = k->column;
	if(i < 8 && i >= 0 && (j+1) < 8 && (j-1) >= 0 && (board[i][j+1] == pawnPiece || board[i][j-1] == pawnPiece))
	{
		check = check + 1;
	}

	/* Check for bishops or queens */
	/* Northeast */
	for(i = k->row+1, j = k->column+1; i < 8 && j < 8; i++, j++)
	{
		/* Check enemy king */
		if(i == k->row+1 && j == k->column+1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == bishopPiece)) /* Must be either bishop or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			i = 9;
		}
	}
	/* Southeast */
	for(i = k->row-1, j = k->column+1; i >= 0 && j < 8; i--, j++)
	{
		/* Check enemy king */
		if(i == k->row-1 && j == k->column+1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == bishopPiece)) /* Must be either bishop or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			j = 9;
		}
	}
	/* Southwest */
	for(i = k->row-1, j = k->column-1; i >= 0 && j >= 0; i--, j--)
	{
		/* Check enemy king */
		if(i == k->row-1 && j == k->column-1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == bishopPiece)) /* Must be either bishop or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			i = -1;
		}
	}
	/* Northwest */
	for(i = k->row+1, j = k->column-1; i < 8 && j >= 0; i++, j--)
	{
		/* Check enemy king */
		if(i == k->row+1 && j == k->column-1)
		{
			if(board[i][j] == kingPiece)
			{
				check = check + 1;
			}
		}
		if((board[i][j] == queenPiece || board[i][j] == bishopPiece)) /* Must be either bishop or queen; else, another piece. Not in check */
		{
			check = check + 1;
		}
		else if(board[i][j] != '-')
		{
			j = -1;
		}
	}
	if (check != 0)
	{
		return 1;
	}
	else /* If none of the above is possible, no check. Return 0 */
	{
		return 0;
	}
}

/*
	Check possibility of move causing/keeping check
	params:
	   - k: king struct;
	   - board: chess board;
	   - originalRowPos: original piece position, row-wise;
	   - originalColumnPos: original piece position, column-wise;
	   - i: new piece position, row-wise;
	   - j: new piece position, column-wise.
    returns:
       - (1) if check or (0) if no check
*/
int checkPossibleCheck(king *k, char **board, int originalRowPos, int originalColumnPos, int i, int j)
{
	int m, n, check;
	char piece, **testBoard;
	/* Create test board */
	testBoard = (char**)malloc(sizeof(char*)*8);
	for(m = 0; m < 8; m++)
	{
		testBoard[m] = (char*)malloc(sizeof(char)*8);
	}
	for(m = 0; m < 8; m++)
	{
		for(n = 0; n < 8; n++)
		{
			testBoard[m][n] = board[m][n];
		}
	}
	/* Change board to test check */
	piece = testBoard[originalRowPos][originalColumnPos];
	testBoard[originalRowPos][originalColumnPos] = '-';
	testBoard[i][j] = piece;
	check = checkCheck(k, testBoard);
	freeMatrix(testBoard, 8);
	if(check == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
	Write possibility of move into matrix
	params:
	   - possibility: matrix to be written;
	   - count: matrix line;
	   - i: row
	   - j: column
	   - character: character to write;
	   - isAttack: if 1, writes 'x' right after character
*/
void writePossibility(char ***possibilities, int *count, int i, int j, char character, int isAttack)
{
	if(!isAttack)
	{
		(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
		(*possibilities)[(*count)] = NULL;
		(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
		(*possibilities)[(*count)][0] = character;
		(*possibilities)[(*count)][1] = j+97;
		(*possibilities)[(*count)][2] = i+48 + 1;
		(*possibilities)[(*count)][3] = '\0';
		(*count) = (*count) + 1;
	}
	else
	{
		(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
		(*possibilities)[(*count)] = NULL;
		(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*5);
		(*possibilities)[(*count)][0] = character;
		(*possibilities)[(*count)][1] = 'x';
		(*possibilities)[(*count)][2] = j+97;
		(*possibilities)[(*count)][3] = i+48 + 1;
		(*possibilities)[(*count)][4] = '\0';
		(*count) = (*count) + 1;
	}
}

/*
	Check if pawn is being promoted
	params:
	   - p: pawn struct to be checked;
	   - board: chess board;
	   - possibilities: char matrix to store possible promotions
	   - count: char matrix counter for realloc
	   - move: indicates following enum moves: 0 - move forward; 1 - attack northwest/southwest; 2 - attack northeast/southeast
	returns:
	   - promoted or not
*/
int checkPromotion(pawn *p, char **board, char ***possibilities, int *count, int move)
{
	int i, promotion = 0;
	char pieces[8] = {'n', 'b', 'r', 'q', 'N', 'B', 'R', 'Q'};
	switch(move)
	{
		/* Case 0: move forward */
		case 0:
			switch(p->isWhite)
			{
				case 'Y':
					if(p->row+1 == 7 && board[p->row+1][p->column] == '-')
					{
						for(i = 4; i < 8; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*5);
							(*possibilities)[(*count)][0] = (p->column)+97;
							(*possibilities)[(*count)][1] = (*possibilities)[(*count)][0];
							(*possibilities)[(*count)][2] = (p->row+1)+48 + 1;
							(*possibilities)[(*count)][3] = pieces[i];
							(*possibilities)[(*count)][4] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				case 'N':
					if(p->row+1 == 2 && board[p->row-1][p->column] == '-')
					{
						for(i = 0; i < 4; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*5);
							(*possibilities)[(*count)][0] = (p->column)+97;
							(*possibilities)[(*count)][1] = (*possibilities)[(*count)][0];
							(*possibilities)[(*count)][2] = (p->row+1)+48 - 1;
							(*possibilities)[(*count)][3] = pieces[i];
							(*possibilities)[(*count)][4] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				default:
				break;
			}
		break;

		/* Case 1: Attack northwest/southwest */
		case 1:
			switch(p->isWhite)
			{
				case 'Y':
					if(p->row+1 == 7 && p->row+1 < 8 && p->column-1 >= 0 && board[p->row+1][p->column-1] != '-')
					{
						for(i = 4; i < 8; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->column-1)+97;
							(*possibilities)[(*count)][3] = (p->row+1)+48 + 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				case 'N':
					if(p->row+1 == 2 && p->row-1 >= 0 && p->column-1 >= 0 && board[p->row-1][p->column-1] != '-')
					{
						for(i = 0; i < 4; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->column-1)+97;
							(*possibilities)[(*count)][3] = (p->row+1)+48 - 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				default:
				break;
			}
		break;

		/* Case 2: Attack northeast/southeast */
		case 2:
			switch(p->isWhite)
			{
				case 'Y':
					if(p->row+1 == 7 && p->row+1 < 8 && p->column+1 < 8 && board[p->row+1][p->column+1] != '-')
					{
						for(i = 4; i < 8; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->column)+97 + 1;
							(*possibilities)[(*count)][3] = (p->row+1)+48 + 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				case 'N':
					if(p->row+1 == 2 && p->row-1 >= 0 && p->column+1 < 8 && board[p->row-1][p->column+1] != '-')
					{
						for(i = 0; i < 4; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->column)+97 + 1;
							(*possibilities)[(*count)][3] = (p->row+1)+48 - 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				default:
				break;
			}
		break;

		default:
		break;
	}
	return promotion;
}

char **checkMovesPawn(pawn* p, king *k, char **board, char *castling, char *enPassant, int kingMove)
{
	int count = 0;
	char **possibilities;
	possibilities = NULL;
	switch(p->isWhite)
	{
		case 'Y':
			/* Check if forward is clear */
			if(p->row+1 < 8 && board[p->row+1][p->column] == '-' && !(checkPossibleCheck(k, board, p->row, p->column, p->row+1, p->column)))
			{
				if(!checkPromotion(p, board, &possibilities, &count, 0))
				{
					writePossibility(&possibilities, &count, p->row+1, p->column, (p->column)+97, 0);
				}
			}
			/* Check if pawn is at beggining - no need to check promotion */
			if(p->row+1 == 2 && p->row+2 < 8 && board[p->row+1][p->column] == '-' && board[p->row+2][p->column] == '-' &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row+2, p->column)))
			{
				writePossibility(&possibilities, &count, p->row+2, p->column, (p->column)+97, 0);
			}
			/* Check pawn attacks */
			/* Left attack */
			if(p->row+1 < 8 && p->column-1 >= 0 && board[p->row+1][p->column-1] != '-' &&
				board[p->row+1][p->column-1] >= 97 && !(checkPossibleCheck(k, board, p->row, p->column, p->row+1, p->column-1)))
			{
				if(!checkPromotion(p, board, &possibilities, &count, 1))
				{
					writePossibility(&possibilities, &count, p->row+1, p->column-1, (p->column)+97, 1);
				}
			}
			/* Left attack if king moved */
			if(kingMove && p->row+1 < 8 && p->column-1 >= 0 && board[p->row+1][p->column-1] == '-')
			{
				writePossibility(&possibilities, &count, p->row+1, p->column-1, (p->column)+97, 0);
			}
			/* Right attack */
			if(p->row+1 < 8 && p->column+1 < 8 && board[p->row+1][p->column+1] != '-' &&
				board[p->row+1][p->column+1] >= 97 && !(checkPossibleCheck(k, board, p->row, p->column, p->row+1, p->column+1)))
			{
				if(!checkPromotion(p, board, &possibilities, &count, 2))
				{
					writePossibility(&possibilities, &count, p->row+1, p->column+1, (p->column)+97, 1);
				}
			}
			/* Right attack if king moved */
			if(kingMove && p->row+1 < 8 && p->column+1 < 8 && board[p->row+1][p->column+1] == '-' && 
				!(checkPossibleCheck(k, board, p->row, p->column, p->row+1, p->column+1)))
			{
				writePossibility(&possibilities, &count, p->row+1, p->column+1, (p->column)+97, 0);
			}
			/* En Passant - left */
			if(p->row+1 == 5 && p->column-1 >= 0 && board[p->row][p->column-1] == 'p' && 
				!(checkPossibleCheck(k, board, p->row, p->column, p->row, p->column-1)))
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
				possibilities[count][0] = (p->column)+97;
				possibilities[count][1] = 'x';
				possibilities[count][2] = (p->column)+97 - 1;
				possibilities[count][3] = (p->row+1)+48 + 1;
				possibilities[count][4] = 'e';
				possibilities[count][5] = '.';
				possibilities[count][6] = 'p';
				possibilities[count][7] = '.';
				possibilities[count][8] = '\0';
			}
			/* En Passant - right */
			if(p->row+1 == 5 && p->column+1 < 8 && board[p->row][p->column+1] == 'p' &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row, p->column+1)))
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
				possibilities[count][0] = (p->column)+97;
				possibilities[count][1] = 'x';
				possibilities[count][2] = (p->column)+97 + 1;
				possibilities[count][3] = (p->row+1)+48 + 1;
				possibilities[count][4] = 'e';
				possibilities[count][5] = '.';
				possibilities[count][6] = 'p';
				possibilities[count][7] = '.';
				possibilities[count][8] = '\0';
			}
		break;

		case 'N':
			/* Check if pawn is at beggining - no need to check promotion */
			if(p->row+1 == 7 && board[p->row-2][p->column] == '-' && board[p->row-1][p->column] == '-' &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row-2, p->column)))
			{
				writePossibility(&possibilities, &count, p->row-2, p->column, (p->column)+97, 0);
			}
			/* Check if forward is clear */
			if(p->row-1 >= 0 && board[p->row-1][p->column] == '-' && !(checkPossibleCheck(k, board, p->row, p->column, p->row-1, p->column)))
			{
				if(!checkPromotion(p, board, &possibilities, &count, 0))
				{
					writePossibility(&possibilities, &count, p->row-1, p->column, (p->column)+97, 0);
				}
			}
			/* Check pawn attacks */
			/* Left attack */
			if(p->row-1 >= 0 && p->column-1 >= 0 && board[p->row-1][p->column-1] != '-' &&
				board[p->row-1][p->column-1] >= 65 && board[p->row-1][p->column-1] < 97 &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row-1, p->column-1)))
			{
				if(!checkPromotion(p, board, &possibilities, &count, 1))
				{
					writePossibility(&possibilities, &count, p->row-1, p->column-1, (p->column)+97, 1);
				}
			}
			/* Left attack if king moved */
			if(kingMove && p->row-1 >= 0 && p->column-1 >= 0 && board[p->row-1][p->column-1] == '-')
			{
				writePossibility(&possibilities, &count, p->row-1, p->column-1, (p->column)+97, 0);
			}
			/* Right attack */
			if(p->row-1 >= 0 && p->column+1 < 8 && board[p->row-1][p->column+1] != '-' &&
				board[p->row-1][p->column+1] >= 65 && board[p->row-1][p->column+1] < 97 &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row-1, p->column+1)))
			{
				if(!checkPromotion(p, board, &possibilities, &count, 2))
				{
					writePossibility(&possibilities, &count, p->row-1, p->column+1, (p->column)+97, 1);
				}
			}
			/* Right attack if king moved */
			if(kingMove && p->row-1 >= 0 && p->column+1 < 8 && board[p->row-1][p->column+1])
			{
				writePossibility(&possibilities, &count, p->row-1, p->column+1, (p->column)+97, 0);
			}
			/* En Passant - left */
			if(p->row+1 == 4 && p->column-1 >= 0 && board[p->row][p->column-1] == 'P' &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row, p->column-1)))
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
				possibilities[count][0] = (p->column)+97;
				possibilities[count][1] = 'x';
				possibilities[count][2] = (p->column)+97 - 1;
				possibilities[count][3] = (p->row+1)+48 - 1;
				possibilities[count][4] = 'e';
				possibilities[count][5] = '.';
				possibilities[count][6] = 'p';
				possibilities[count][7] = '.';
				possibilities[count][8] = '\0';
				count = count + 1;
			}
			/* En Passant - right */
			if(p->row+1 == 4 && p->column+1 < 8 && board[p->row][p->column+1] == 'P' &&
				!(checkPossibleCheck(k, board, p->row, p->column, p->row, p->column+1)))
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
				possibilities[count][0] = (p->column)+97;
				possibilities[count][1] = 'x';
				possibilities[count][2] = (p->column)+97 + 1;
				possibilities[count][3] = (p->row+1)+48 - 1;
				possibilities[count][4] = 'e';
				possibilities[count][5] = '.';
				possibilities[count][6] = 'p';
				possibilities[count][7] = '.';
				possibilities[count][8] = '\0';
				count = count + 1;
			}
		break;

		default:
		break;
	}
	p->possibleMoves = count;
	return possibilities;
}

/*
	Checks simultaneous positions for knights or rooks
	params:
	   - moves1: possible moves from first element
	   - moves2: possible moves from second element
	   - n: number of possible moves from first piece
	   - m: number of possible moves from second piece
	   - firstColumn: first column position
	   - secondColumn: second column position
	returns:
	   - list of moves for either knight or rook
*/
char **checkSimultaneousPositions(char **moves1, char **moves2, int n, int m, char firstColumn, char secondColumn)
{
	int i, j;
	char **possibilities, *tmp;
	possibilities = NULL;
	/* Compare strings from moves1 on moves2 */
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
		{
			if(strcmp(moves1[i], moves2[j]) == 0) /* Same piece at same position */
			{
				/* Change moves1 strings */
				tmp = (char*)malloc(sizeof(char)*strlen(moves1[i])+2);
				tmp[0] = moves1[i][0];
				tmp[1] = firstColumn;
				tmp[2] = moves1[i][1];
				tmp[3] = moves1[i][2];
				tmp[4] = '\0';
				moves1[i] = (char*)realloc(moves1[i], sizeof(char)*strlen(moves1[i])+2);
				strcpy(moves1[i], tmp);
				free(tmp);

				/* Change moves2 strings */
				tmp = (char*)malloc(sizeof(char)*strlen(moves2[j])+2);
				tmp[0] = moves2[j][0];
				tmp[1] = secondColumn;
				tmp[2] = moves2[j][1];
				tmp[3] = moves2[j][2];
				tmp[4] = '\0';
				moves2[j] = (char*)realloc(moves2[j], sizeof(char)*strlen(moves2[j])+2);
				strcpy(moves2[j], tmp);
				free(tmp);
			}
		}
	}
	/* Create possibilities matrix; insert all elements from moves1 and moves2 separately */
	possibilities = (char**)realloc(possibilities, sizeof(char*)*(n+m));
	for(i = 0; i < n; i++)
	{
		possibilities[i] = NULL;
		possibilities[i] = (char*)realloc(possibilities[i], sizeof(char)*(strlen(moves1[i])+1));
		strcpy(possibilities[i], moves1[i]);
	}
	for(j = 0; i < n+m; i++, j++)
	{
		possibilities[i] = NULL;
		possibilities[i] = (char*)realloc(possibilities[i], sizeof(char)*(strlen(moves2[j])+1));
		strcpy(possibilities[i], moves2[j]);
	}
	return possibilities;
}

char **checkMovesKnight(knight* k, king *kng, char **board, char *castling, char *enPassant)
{
	int count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	possibilities = NULL;
	/* Knight has 8 possible moves */
	piece = 'N';
	switch(k->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
		break;
	}
	/* Check southwest first corner */
	if(k->row-1 >= 0 && k->column-2 >= 0 && board[k->row-1][k->column-2] == '-' && 
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-1, k->column-2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row-1, k->column-2, piece, 0);
	}
	else if(k->row-1 >= 0 && k->column-2 >= 0 && 
		(board[k->row-1][k->column-2] == pawnPiece || 
		board[k->row-1][k->column-2] == knightPiece ||
		board[k->row-1][k->column-2] == bishopPiece ||
		board[k->row-1][k->column-2] == queenPiece ||
		board[k->row-1][k->column-2] == rookPiece ||
		board[k->row-1][k->column-2] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-1, k->column-2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row-1, k->column-2, piece, 1);
	}

	/* Check northwest first corner */
	if(k->row+1 < 8 && k->column-2 >= 0 && board[k->row+1][k->column-2] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+1, k->column-2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row+1, k->column-2, piece, 0);
	}
	else if(k->row+1 < 8 && k->column-2 >= 0 && 
		(board[k->row+1][k->column-2] == pawnPiece || 
		board[k->row+1][k->column-2] == knightPiece ||
		board[k->row+1][k->column-2] == bishopPiece ||
		board[k->row+1][k->column-2] == queenPiece ||
		board[k->row+1][k->column-2] == rookPiece ||
		board[k->row+1][k->column-2] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+1, k->column-2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row+1, k->column-2, piece, 1);
	}

	/* Check southwest second corner */
	if(k->row-2 >= 0 && k->column-1 >= 0 && board[k->row-2][k->column-1] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-2, k->column-1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row-2, k->column-1, piece, 0);
	}
	else if(k->row-2 >= 0 && k->column-1 >= 0 && 
		(board[k->row-2][k->column-1] == pawnPiece || 
		board[k->row-2][k->column-1] == knightPiece ||
		board[k->row-2][k->column-1] == bishopPiece ||
		board[k->row-2][k->column-1] == queenPiece ||
		board[k->row-2][k->column-1] == rookPiece ||
		board[k->row-2][k->column-1] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-2, k->column-1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row-2, k->column-1, piece, 1);
	}

	/* Check northwest second corner */
	if(k->row+2 < 8 && k->column-1 >= 0 && board[k->row+2][k->column-1] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+2, k->column-1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row+2, k->column-1, piece, 0);
	}
	else if(k->row+2 < 8 && k->column-1 >= 0 && 
		(board[k->row+2][k->column-1] == pawnPiece || 
		board[k->row+2][k->column-1] == knightPiece ||
		board[k->row+2][k->column-1] == bishopPiece ||
		board[k->row+2][k->column-1] == queenPiece ||
		board[k->row+2][k->column-1] == rookPiece ||
		board[k->row+2][k->column-1] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+2, k->column-1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row+2, k->column-1, piece, 1);
	}

	/* Check southeast first corner */
	if(k->row-2 >= 0 && k->column+1 < 8 && board[k->row-2][k->column+1] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-2, k->column+1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row-2, k->column+1, piece, 0);
	}
	else if(k->row-2 >= 0 && k->column+1 < 8 && 
		(board[k->row-2][k->column+1] == pawnPiece || 
		board[k->row-2][k->column+1] == knightPiece ||
		board[k->row-2][k->column+1] == bishopPiece ||
		board[k->row-2][k->column+1] == queenPiece ||
		board[k->row-2][k->column+1] == rookPiece ||
		board[k->row-2][k->column+1] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-2, k->column+1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row-2, k->column+1, piece, 1);
	}

	/* Check northeast first corner */
	if(k->row+2 < 8 && k->column+1 < 8 && board[k->row+2][k->column+1] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+2, k->column+1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row+2, k->column+1, piece, 0);
	}
	else if(k->row+2 < 8 && k->column+1 < 8 && 
		(board[k->row+2][k->column+1] == pawnPiece || 
		board[k->row+2][k->column+1] == knightPiece ||
		board[k->row+2][k->column+1] == bishopPiece ||
		board[k->row+2][k->column+1] == queenPiece ||
		board[k->row+2][k->column+1] == rookPiece ||
		board[k->row+2][k->column+1] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+2, k->column+1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row+2, k->column+1, piece, 1);
	}

	/* Check southeast second corner */
	if(k->row-1 >= 0 && k->column+2 < 8 && board[k->row-1][k->column+2] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-1, k->column+2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row-1, k->column+2, piece, 0);
	}
	else if(k->row-1 >= 0 && k->column+2 < 8 && 
		(board[k->row-1][k->column+2] == pawnPiece || 
		board[k->row-1][k->column+2] == knightPiece ||
		board[k->row-1][k->column+2] == bishopPiece ||
		board[k->row-1][k->column+2] == queenPiece ||
		board[k->row-1][k->column+2] == rookPiece ||
		board[k->row-1][k->column+2] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row-1, k->column+2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row-1, k->column+2, piece, 1);
	}

	/* Check northeast second corner */
	if(k->row+1 < 8 && k->column+2 < 8 && board[k->row+1][k->column+2] == '-' &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+1, k->column+2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->row+1, k->column+2, piece, 0);
	}
	else if(k->row+1 < 8 && k->column+2 < 8 && 
		(board[k->row+1][k->column+2] == pawnPiece || 
		board[k->row+1][k->column+2] == knightPiece ||
		board[k->row+1][k->column+2] == bishopPiece ||
		board[k->row+1][k->column+2] == queenPiece ||
		board[k->row+1][k->column+2] == rookPiece ||
		board[k->row+1][k->column+2] == kingPiece) &&
		!(checkPossibleCheck(kng, board, k->row, k->column, k->row+1, k->column+2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->row+1, k->column+2, piece, 1);
	}
	k->possibleMoves = count;
	return possibilities;
}

char **checkMovesBishop(bishop* b, king *k, char **board, char *castling, char *enPassant, int kingMove)
{
	int i, j, count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	possibilities = NULL;
	piece = 'B';
	switch(b->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
		break;
	}

	/* Check lower left diagonal */
	for(i = b->row-1, j = b->column-1; i >= 0 && j >= 0; i--, j--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = -1;
		}
		else /* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check upper left diagonal */
	for(i = b->row+1, j = b->column-1; i < 8 && j >= 0; i++, j--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = 9;
		}
		else /* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check lower right diagonal */
	for(i = b->row-1, j = b->column+1; i >= 0 && j < 8; i--, j++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = -1;
		}
		else /* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check upper right diagonal */
	for(i = b->row+1, j = b->column+1; i < 8 && j < 8; i++, j++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, b->row, b->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = 9;
		}
		else /* Friendly piece - stop */
		{
			i = 9;
		}
	}
	b->possibleMoves = count;
	return possibilities;
}

char **checkMovesQueen(queen* q, king *k, char **board, char *castling, char *enPassant, int kingMove)
{
	int i, j, count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	possibilities = NULL;
	piece = 'Q';
	switch(q->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
		break;
	}
	/* Check all possible positions, regardless of sorting; qsort will handle it later */
	/* Check lower left diagonal */
	for(i = q->row-1, j = q->column-1; i >= 0 && j >= 0; i--, j--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = -1;
		}
		else /* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check left row */
	for(i = q->row, j = q->column-1; j >= 0; j--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			j = -1;
		}
		else /* Friendly piece - stop */
		{
			j = -1;
		}
	}

	/* Check upper left diagonal */
	for(i = q->row+1, j = q->column-1; i < 8 && j >= 0; i++, j--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = 9;
		}
		else /* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check up column */
	for(i = q->row+1, j = q->column; i < 8; i++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = 9;
		}
		else /* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check lower right diagonal */
	for(i = q->row-1, j = q->column+1; i >= 0 && j < 8; i--, j++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = -1;
		}
		else /* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check right row */
	for(i = q->row, j = q->column+1; j < 8; j++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			j = 9;
		}
		else /* Friendly piece - stop */
		{
			j = 9;
		}	
	}

	/* Check upper right diagonal */
	for(i = q->row+1, j = q->column+1; i < 8 && j < 8; i++, j++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = 9;
		}
		else /* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check down column */
	for(i = q->row-1, j = q->column; i >= 0; i--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece)
			&& !(checkPossibleCheck(k, board, q->row, q->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, q->row, q->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = -1;
		}
		else /* Friendly piece - stop */
		{
			i = -1;
		}
	}
	q->possibleMoves = count;
	return possibilities;
}

char **checkMovesKing(king* k, char **board, char *castling, char *enPassant)
{
	int i, j, count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece, castlingPiece;
	possibilities = NULL;
	piece = 'K';
	switch(k->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
			castlingPiece = 'R';
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
			castlingPiece = 'r';
		break;
	}
	/* Check lower left diagonal */
	i = k->row-1;
	j = k->column-1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->row = i;
		k->column = j;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row+1, k->column+1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row+1, k->column+1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);	
		}
		k->row = i+1;
		k->column = j+1;
	}

	/* Check left */
	i = k->row;
	j = k->column-1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->column = j;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row, k->column+1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row, k->column+1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->column = j+1;
	}

	/* Check upper left diagonal */
	i = k->row+1;
	j = k->column-1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->row = i;
		k->column = j;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row-1, k->column+1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row-1, k->column+1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->row = i-1;
		k->column = j+1;
	}

	/* Check up column */
	i = k->row+1;
	j = k->column;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->row = i;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row-1, k->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row-1, k->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->row = i-1;
	}

	/* Check lower right diagonal */
	i = k->row-1;
	j = k->column+1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->row = i;
		k->column = j;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row+1, k->column-1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row+1, k->column-1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->row = i+1;
		k->column = j-1;
	}

	/* Check right row */
	i = k->row;
	j = k->column+1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->column = j;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row, k->column-1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row, k->column-1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->column = j-1;
	}

	/* Check upper right row */		
	i = k->row+1;
	j = k->column+1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->row = i;
		k->column = j;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row-1, k->column-1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row-1, k->column-1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->row = i-1;
		k->column = j-1;
	}

	/* Check down column */
	i = k->row-1;
	j = k->column;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->row = i;
		if(board[i][j] == '-' && !(checkPossibleCheck(k, board, k->row+1, k->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
		|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, board, k->row+1, k->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			
		}
		k->row = i+1;
	}

	/* Right Castling */
	if(k->column+3 < 8 && k->row+1 == 1 && board[k->row][k->column+1] == '-' && 
		board[k->row][k->column+2] == '-' && board[k->row][k->column+3] == castlingPiece)
	{
		k->column = k->column+2;
		if(k->column < 8 && !(checkPossibleCheck(k, board, k->row, k->column-2, k->row, k->column)))
		{
			k->column = k->column-2;
			writePossibility(&possibilities, &count, k->row, (k->column+2), piece, 0);
		}
	}
	/* Left Castling */
	if(k->column-4 >= 0 && k->row+1 == 1 && board[k->row][k->column-1] == '-' && 
		board[k->row][k->column-2] == '-' && board[k->row][k->column-3] == '-' &&
		board[k->row][k->column-4] == castlingPiece)
	{
		k->column = k->column-2;
		if(k->column >= 0 && !(checkPossibleCheck(k, board, k->row, k->column+2, k->row, k->column)))
		{
			k->column = k->column+2;
			writePossibility(&possibilities, &count, k->row, (k->column-2), piece, 0);
		}
	}
	k->possibleMoves = count;
	return possibilities;
}

char **checkMovesRook(rook* r, king *k, char **board, char *castling, char *enPassant, int kingMove)
{
	int i, j, count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	piece = 'R';
	switch(r->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
		break;
	}
	possibilities = NULL;
	/* Check all four possible positions, regardless of sorting; qsort will handle it later */
	/* Check left row */
	for(i = r->row, j = r->column-1; j >= 0; j--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			j = -1;
		}
		else /* Friendly piece - stop */
		{
			j = -1;
		}
	}

	/* Check up column */
	for(i = r->row+1, j = r->column; i < 8; i++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = 9;
		}
		else /* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check right row */
	for(i = r->row, j = r->column+1; j < 8; j++)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			j = 9;
		}
		else /* Friendly piece - stop */
		{
			j = 9;
		}	
	}

	/* Check down column */
	for(i = r->row-1, j = r->column; i >= 0; i--)
	{
		if(kingMove && (board[i][j] == '-' || board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(board[i][j] == '-' && !(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((board[i][j] == pawnPiece || board[i][j] == knightPiece || board[i][j] == bishopPiece || board[i][j] == queenPiece
			|| board[i][j] == kingPiece || board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, board, r->row, r->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 1);
			/* Since we found a piece, stop */
			i = -1;
		}
		else /* Friendly piece - stop */
		{
			i = -1;
		}
	}
	r->possibleMoves = count;
	return possibilities;
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
	for(i = (size-1), k = 0; i >= 0 && k < (strlen(input)); i--)
	{
		for(j = 0; j < size; j++)
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

/*
	Check if king can be saved
	params:
	   - nPieces: pieces in play;
	   - p: array of pawns;
	   - k: array of knights;
	   - b: array of bishops;
	   - q: queen;
	   - k: king;
	   - r: array of rooks;
	returns:
	   - (1) if no saving, (0) otherwise
*/
int cantBeSaved(int *nPieces, pawn *p, knight *n, bishop *b, queen *q, king *k, rook *r, char **board)
{
	int i, j, saved = 1;
	char **possibilities;
	possibilities = NULL;
	switch(k->isWhite)
	{
		case 'Y':
			/* Check all pieces for possible moves */
			for(i = 1; i < 12; i = i + 2)
			{
				if(i == WHITEPAWN)
				{
					for(j = 0; j < nPieces[WHITEPAWN]; j++)
					{
						if(p[j].eaten == 'N')
						{
							possibilities = p[j].checkMoves(&p[j], k, board, NULL, NULL, 0);
							printMatrix(possibilities, p[j].possibleMoves, -1);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = nPieces[WHITEPAWN] + 1;
							}
						}
					}
				}
				else if(i == WHITEKNIGHT)
				{
					for(j = 0; j < nPieces[WHITEKNIGHT]; j++)
					{
						if(n[j].eaten == 'N')
						{
							possibilities = n[j].checkMoves(&n[j], k, board, NULL, NULL);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = nPieces[WHITEKNIGHT] + 1;
							}
						}
					}
				}
				else if(i == WHITEBISHOP)
				{
					for(j = 0; j < nPieces[WHITEBISHOP]; j++)
					{
						if(b[j].eaten == 'N')
						{
							possibilities = b[j].checkMoves(&b[j], k, board, NULL, NULL, 0);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = nPieces[WHITEBISHOP] + 1;
							}
						}
					}
				}
				else if(i == WHITEQUEEN)
				{
					for(j = 0; j < nPieces[WHITEQUEEN]; j++)
					{
						if(q[j].eaten == 'N')
						{
							possibilities = q[j].checkMoves(&q[j], k, board, NULL, NULL, 0);	
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + nPieces[WHITEQUEEN] + 1;
							}
						}
					}
				}
				else if(i == WHITEKING)
				{
					for(j = 0; j < nPieces[WHITEKING]; j++)
					{
						if(k[j].eaten == 'N')
						{
							possibilities = k[j].checkMoves(&k[j], board, NULL, NULL);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + nPieces[WHITEKING] + 1;
							}
						}
					}
				}
				else if(i == WHITEROOK)
				{
					for(j = 0; j < nPieces[WHITEROOK]; j++)
					{
						if(r[j].eaten == 'N')
						{
							possibilities = r[j].checkMoves(&r[j], k, board, NULL, NULL, 0);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + nPieces[WHITEROOK] + 1;
							}
						}
					}
				}
			}
		break;

		case 'N':
			for(i = 0; i < 12; i = i + 2)
			{
				if(i == BLACKPAWN)
				{
					for(j = 0; j < nPieces[BLACKPAWN]; j++)
					{
						if(p[j].eaten == 'N')
						{
							possibilities = p[j].checkMoves(&p[j], k, board, NULL, NULL, 0);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = nPieces[BLACKPAWN] + 1;
							}
						}
					}
				}
				else if(i == BLACKKNIGHT)
				{
					for(j = 0; j < nPieces[BLACKKNIGHT]; j++)
					{
						if(n[j].eaten == 'N')
						{
							possibilities = n[j].checkMoves(&n[j], k, board, NULL, NULL);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = nPieces[BLACKKNIGHT] + 1;
							}
						}
					}
				}
				else if(i == BLACKBISHOP)
				{
					for(j = 0; j < nPieces[BLACKBISHOP]; j++)
					{
						if(b[j].eaten == 'N')
						{
							possibilities = b[j].checkMoves(&b[j], k, board, NULL, NULL, 0);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = nPieces[BLACKBISHOP] + 1;
							}
						}
					}
				}
				else if(i == BLACKQUEEN)
				{
					for(j = 0; j < nPieces[BLACKQUEEN]; j++)
					{
						if(q[j].eaten == 'N')
						{
							possibilities = q[j].checkMoves(&q[j], k, board, NULL, NULL, 0);	
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + nPieces[BLACKQUEEN] + 1;
							}
						}
					}
				}
				else if(i == BLACKKING)
				{
					for(j = 0; j < nPieces[BLACKKING]; j++)
					{
						if(k[j].eaten == 'N')
						{
							possibilities = k[j].checkMoves(&k[j], board, NULL, NULL);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + nPieces[BLACKKING] + 1;
							}
						}
					}
				}
				else if(i == BLACKROOK)
				{
					for(j = 0; j < nPieces[BLACKROOK]; j++)
					{
						if(r[j].eaten == 'N')
						{
							possibilities = r[j].checkMoves(&r[j], k, board, NULL, NULL, 0);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + nPieces[BLACKROOK] + 1;
							}
						}
					}
				}
			}
		break;

		default:
			printf("Shouldn't be here\n");
		break;
	}
	return saved;
}

/*
	Make a copy between king structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void structCopy(king **destination, king *source)
{
	if(!(*destination))
	{
		(*destination) = (king*)malloc(sizeof(king)*1);
	}
	(*destination)->init = source->init;
	switch(source->isWhite)
	{
		case 'Y':
			(*destination)->init((*destination), source->row, source->column, 'Y');
		break;

		case 'N':
			(*destination)->init((*destination), source->row, source->column, 'N');
		break;

		default:
		break;
	}
}

/*
	Check number of moves possible for a given king
	params:
	   - k: king struct;
	   - board: chess board;
	   - moves: 3x3 matrix containing (1) for every possible move and (0) otherwise
	returns:
	   - number of possible moves
*/
int checkNumberOfMoves(king *k, char **board, int ***moveSet)
{
	int moves;
	moves = 0;
	/* North */
	if(k->row-1 >= 0 && board[k->row-1][k->column] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[0][1] = 1;
	}
	/* Northeast */
	if(k->row-1 >= 0 && k->column+1 < 8 && board[k->row-1][k->column+1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[1][1] = 1;
	}
	/* East */
	if(k->column+1 < 8 && board[k->row][k->column+1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[1][2] = 1;
	}
	/* Southeast */
	if(k->row+1 < 8 && k->column+1 < 8 && board[k->row+1][k->column+1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[2][2] = 1;
	}
	/* South */
	if(k->row+1 < 8 && board[k->row+1][k->column] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[2][1] = 1;
	}
	/* Southwest */
	if(k->row+1 < 8 && k->column-1 >= 0 && board[k->row+1][k->column-1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[2][0] = 1;
	}
	/* West */
	if(k->column-1 >= 0 && board[k->row][k->column-1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[1][0] = 1;
	}
	/* Northwest */
	if(k->row-1 >= 0 && k->column-1 >= 0 && board[k->row-1][k->column-1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[0][0] = 1;
	}
	return moves;
}

/*
	Update chess board
	params:
	   - board: chess board;
	   - originalRowPos: original piece position, row-wise;
	   - originalColumnPos: original piece position, column-wise;
	   - i: new piece position, row-wise;
	   - j: new piece position, column-wise
*/
void updateBoard(char **board, int originalRowPos, int originalColumnPos, int i, int j)
{
	char piece;
	/* Check possible castlings */
	/* White pieces case */
	if((board[originalRowPos][originalColumnPos] == 'K' || board[originalRowPos][originalColumnPos] == 'k') && j == originalColumnPos + 2)
	{
		/* Right castling */
		piece = board[originalRowPos][originalColumnPos];
		board[originalRowPos][originalColumnPos] = '-';
		board[originalRowPos][originalColumnPos+2] = piece;

		piece = board[originalRowPos][originalColumnPos+3];
		board[originalRowPos][originalColumnPos+3] = '-';
		board[originalRowPos][originalColumnPos+1] = piece;
	}
	else if((board[originalRowPos][originalColumnPos] == 'K' || board[originalRowPos][originalColumnPos] == 'k') && j == originalColumnPos - 2)
	{
		/* Left castling */
		piece = board[originalRowPos][originalColumnPos];
		board[originalRowPos][originalColumnPos] = '-';
		board[originalRowPos][originalColumnPos-2] = piece;

		piece = board[originalRowPos][originalColumnPos-4];
		board[originalRowPos][originalColumnPos-4] = '-';
		board[originalRowPos][originalColumnPos-1] = piece;
	}
	else
	{
		piece = board[originalRowPos][originalColumnPos];
		board[originalRowPos][originalColumnPos] = '-';
		board[i][j] = piece;
	}
}

/* ------------------------------------------------------------------------------------------------------------ */

/*
	Update position functions
*/

/* ------------------------------------------------------------------------------------------------------------ */
void updatePosPawn(pawn *p, int i, int j)
{
	p->row = i;
	p->column = j;
}

void updatePosKnight(knight *k, int i, int j)
{
	k->row = i;
	k->column = j;
}

void updatePosBishop(bishop *b, int i, int j)
{
	b->row = i;
	b->column = j;
}

void updatePosQueen(queen *q, int i, int j)
{
	q->row = i;
	q->column = j;
}

void updatePosKing(king *k, int i, int j)
{
	k->row = i;
	k->column = j;
}

void updatePosRook(rook *r, int i, int j)
{
	r->row = i;
	r->column = j;
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
	p->eaten = 'N';
	p->checkMoves = (**checkMovesPawn);
	p->updatePosition = (*updatePosPawn);
}

void initKnight(knight *k, int i, int j, char isWhite)
{
	k->row = i;
	k->column = j;
	k->possibleMoves = 0;
	k->isWhite = isWhite;
	k->eaten = 'N';
	k->checkMoves = (**checkMovesKnight);
	k->updatePosition = (*updatePosKnight);
}

void initBishop(bishop *b, int i, int j, char isWhite)
{
	b->row = i;
	b->column = j;
	b->possibleMoves = 0;
	b->isWhite = isWhite;
	b->eaten = 'N';
	b->checkMoves = (**checkMovesBishop);
	b->updatePosition = (*updatePosBishop);
}

void initQueen(queen *q, int i, int j, char isWhite)
{
	q->row = i;
	q->column = j;
	q->possibleMoves = 0;
	q->isWhite = isWhite;
	q->eaten = 'N';
	q->checkMoves = (**checkMovesQueen);
	q->updatePosition = (*updatePosQueen);
}

void initKing(king *k, int i, int j, char isWhite)
{
	k->row = i;
	k->column = j;
	k->possibleMoves = 0;
	k->isWhite = isWhite;
	k->eaten = 'N';
	k->checkMoves = (**checkMovesKing);
	k->updatePosition = (*updatePosKing);
}

void initRook(rook *r, int i, int j, char isWhite)
{
	r->row = i;
	r->column = j;
	r->possibleMoves = 0;
	r->isWhite = isWhite;
	r->eaten = 'N';
	r->checkMoves = (**checkMovesRook);
	r->updatePosition = (*updatePosRook);
}

/*
	Print a finish message 
	params:
	   - str: message to be printed.
*/
void endGame(const char *str)
{
	printf("%s\n", str);
}

/*
	Check possible castling, printing if possible
	param:
	   - board: chess board
*/
void checkCastling(char **board)
{
	int castling = 0;
	/* White pieces castling */
	if(board[0][7] == 'R' && board[0][4] == 'K')
	{
		printf("K");
		castling = 1;
	}
	if(board[0][0] == 'R' && board[0][4] == 'K')
	{
		printf("Q");
		castling = 1;
	}
	/* Black pieces castling */
	if(board[7][7] == 'r' && board[7][4] == 'k')
	{
		printf("k");
		castling = 1;
	}
	if(board[7][0] == 'r' && board[7][4] == 'k')
	{
		printf("q");
		castling = 1;
	}
	if(castling == 0)
	{
		printf("-");
	}
}

/*
	Check possible en Passant, printing if possible
	params:
	   - move1: original piece position;
	   - move2: move;
	   - isPawn: (1) if was pawn move, (0) if wasn't
*/
void checkEnPassant(char *move1, char *move2, int isPawn)
{
	if(isPawn && (move1[1]-49 == 1 || move1[1]-49 == 6) && abs((move2[1]-49) - (move1[1]-49)) == 2)
	{
		if(move2[1]-49 > move1[1]-49)
		{
			printf("%c%c", move2[0], move2[1]-1);
		}
		else
		{
			printf("%c%c", move2[0], move2[1]+1);	
		}
	}
	else
	{
		printf("-");
	}
}

/*
	Print with FEN notation
	params:
	   - board: board to be printed;
	   - move1: original piece position;
	   - move2: move made from move1;
	   - turn: next turn;
	   - halfturn: number of halfturns;
	   - turnCount: number of turns;
	   - enPassant: en Passant from first turn;
	   - isPawn: (1) if was pawn move, (0) if wasn't. Used for checking enPassant
*/
void printFEN(char **board, char *move1, char *move2, char turn, int halfturn, int turnCount, char *enPassant, int isPawn)
{
	int i, j, emptySpaces;
	for(i = 7; i >= 0; i--)
	{
		for(j = 0; j < 8; j++)
		{
			emptySpaces = 0;
			/* Count number of empty spaces */
			while(j < 8 && board[i][j] == '-')
			{
				emptySpaces = emptySpaces + 1;
				j = j + 1;
			}
			if(emptySpaces != 0)
			{
				printf("%d", emptySpaces);
			}
			if(j < 8 && board[i][j] != '-')
			{
				printf("%c", board[i][j]);
			}
		}
		if(i != 0)
		{
			printf("/");
		}
	}
	printf(" ");
	printf("%c", turn);
	printf(" ");
	/* Check possible castlings */
	checkCastling(board);
	printf(" ");
	/* Check en Passant */
	if(!enPassant)
	{
		checkEnPassant(move1, move2, isPawn);
	}
	else
	{
		printf("%s", enPassant);
	}
	printf(" ");
	printf("%d", halfturn);
	printf(" ");
	printf("%d\n", turnCount);
	
}

int main(void)
{
	/* Local variable declarations */
	/* i, j: loop control variables; nPieces: each position indicates number of pieces read; follows order defined through lines
		459-470 */
	int i, j, *nPieces, finish, halfturn, turn, turnCount, materials, check, isPawn, **moveSet, moves;
	/* input: string to store commands passed through stdin; board: char type matrix board to annotate elements at given positions;
		board: matrix to store pieces positions
		turn: store current turn
		castling: store castling information

	*/
	char *input, **board, *castling, *enPassant, *move, *move1, *move2, piece;
	/*
		Structures representing pieces
		pawn, rooks, bishops...: containing each piece in a given board position;
	*/
	pawn *bP, *wP;
	knight *bN, *wN;
	bishop *bB, *wB;
	queen *bQ, *wQ;
	king *bK, *wK, *aux;
	rook *bR, *wR;
	
	/* Step 1: Read input */
	/* Step 1.1: Read chess board */
	input = read(stdin, ' ');
	/* Step 1.2: Create pieces and initialize board */
	board = initializeBoard(input, 8);
	// printMatrix(board, 8, 8);
	bP = (pawn*)malloc(sizeof(pawn)*8);
	wP = (pawn*)malloc(sizeof(pawn)*8);
	bN = (knight*)malloc(sizeof(knight)*2);
	wN = (knight*)malloc(sizeof(knight)*2);
	bB = (bishop*)malloc(sizeof(bishop)*2);
	wB = (bishop*)malloc(sizeof(bishop)*2);
	bQ = (queen*)malloc(sizeof(queen)*1);
	wQ = (queen*)malloc(sizeof(queen)*1);
	bK = (king*)malloc(sizeof(king)*1);
	wK = (king*)malloc(sizeof(king)*1);
	bR = (rook*)malloc(sizeof(rook)*2);
	wR = (rook*)malloc(sizeof(rook)*2);
	nPieces = (int*)calloc(12, sizeof(int));
	/* Step 1.3: Initialize pieces structs */
	for(j = 0; j < 8; j++)
	{
		for(i = 0; i < 8; i++)
		{
			switch(board[i][j])
			{
				case 'p':
					bP[nPieces[BLACKPAWN]].init = (*initPawn);
					bP[nPieces[BLACKPAWN]].init(&bP[nPieces[BLACKPAWN]], i, j, 'N');
					nPieces[BLACKPAWN] = nPieces[BLACKPAWN] + 1;
				break;

				case 'P':
					wP[nPieces[WHITEPAWN]].init = (*initPawn);
					wP[nPieces[WHITEPAWN]].init(&wP[nPieces[WHITEPAWN]], i, j, 'Y');
					nPieces[WHITEPAWN] = nPieces[WHITEPAWN] + 1;
				break;

				case 'n':
					bN[nPieces[BLACKKNIGHT]].init = (*initKnight);
					bN[nPieces[BLACKKNIGHT]].init(&bN[nPieces[BLACKKNIGHT]], i, j, 'N');
					nPieces[BLACKKNIGHT] = nPieces[BLACKKNIGHT] + 1;
				break;

				case 'N':
					wN[nPieces[WHITEKNIGHT]].init = (*initKnight);
					wN[nPieces[WHITEKNIGHT]].init(&wN[nPieces[WHITEKNIGHT]], i, j, 'Y');
					nPieces[WHITEKNIGHT] = nPieces[WHITEKNIGHT] + 1;
				break;

				case 'b':
					bB[nPieces[BLACKBISHOP]].init = (*initBishop);
					bB[nPieces[BLACKBISHOP]].init(&bB[nPieces[BLACKBISHOP]], i, j, 'N');
					nPieces[BLACKBISHOP] = nPieces[BLACKBISHOP] + 1;
				break;

				case 'B':
					wB[nPieces[WHITEBISHOP]].init = (*initBishop);
					wB[nPieces[WHITEBISHOP]].init(&wB[nPieces[WHITEBISHOP]], i, j, 'Y');
					nPieces[WHITEBISHOP] = nPieces[WHITEBISHOP] + 1;
				break;

				case 'q':
					bQ[nPieces[BLACKQUEEN]].init = (*initQueen);
					bQ[nPieces[BLACKQUEEN]].init(&bQ[nPieces[BLACKQUEEN]], i, j, 'N');
					nPieces[BLACKQUEEN] = nPieces[BLACKQUEEN] + 1;
				break;

				case 'Q':
					wQ[nPieces[WHITEQUEEN]].init = (*initQueen);
					wQ[nPieces[WHITEQUEEN]].init(&wQ[nPieces[WHITEQUEEN]], i, j, 'Y');
					nPieces[WHITEQUEEN] = nPieces[WHITEQUEEN] + 1;
				break;

				case 'k':
					bK[nPieces[BLACKKING]].init = (*initKing);
					bK[nPieces[BLACKKING]].init(&bK[nPieces[BLACKKING]], i, j, 'N');
					nPieces[BLACKKING] = nPieces[BLACKKING] + 1;
				break;

				case 'K':
					wK[nPieces[WHITEKING]].init = (*initKing);
					wK[nPieces[WHITEKING]].init(&wK[nPieces[WHITEKING]], i, j, 'Y');
					nPieces[WHITEKING] = nPieces[WHITEKING] + 1;
				break;

				case 'r':
					bR[nPieces[BLACKROOK]].init = (*initRook);
					bR[nPieces[BLACKROOK]].init(&bR[nPieces[BLACKROOK]], i, j, 'N');
					nPieces[BLACKROOK] = nPieces[BLACKROOK] + 1;
				break;

				case 'R':
					wR[nPieces[WHITEROOK]].init = (*initRook);
					wR[nPieces[WHITEROOK]].init(&wR[nPieces[WHITEROOK]], i, j, 'Y');
					nPieces[WHITEROOK] = nPieces[WHITEROOK] + 1;
				break;

				default: /* Not a piece; do nothing */
				break;
			}
		}
	}

	materials = sumVector(nPieces, 12);

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

	/* Step 5: read half-turns*/
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	halfturn = toNum(input);

	/* Step 6: read turn */
	free(input);
	input = NULL;
	input = read(stdin, '\n');
	turnCount = toNum(input);
	move1  = NULL;
	move2  = NULL;

	printFEN(board, move1, move2, turn, halfturn, turnCount, enPassant, 0);

	free(castling);
	free(enPassant);
	castling = NULL;
	enPassant = NULL;

	// moveSet = (int**)calloc(3, sizeof(int*));
	// for(i = 0; i < 3; i++)
	// {
	// 	moveSet[i] = (int*)calloc(3, sizeof(int));
	// }
	// moves = 0;
	// check = 0;
	// switch(turn)
	// {
	// 	case 'w':
	// 		moves = checkNumberOfMoves(bK, board, &moveSet);
	// 		if(moves != 0)
	// 		{
	// 			for(i = 0; i < 3; i++)
	// 			{
	// 				for(j = 0; j < 3; j++)
	// 				{
	// 					if(moveSet[i][j] != 0)
	// 					{
	// 						aux = NULL;
	// 						structCopy(&aux, bK);
	// 						aux->updatePosition(&(*aux), aux->row+(i-1), aux->column+(j-1));
	// 						check = check + checkCheck(aux, board);
	// 						free(aux);
	// 					}
	// 				}
	// 			}
	// 		}
	// 		// check = checkCheck(wK, board);
	// 	break;

	// 	case 'b':
	// 		moves = checkNumberOfMoves(wK, board, &moveSet);
	// 		if(moves != 0)
	// 		{
	// 			for(i = 0; i < 3; i++)
	// 			{
	// 				for(j = 0; j < 3; j++)
	// 				{
	// 					if(moveSet[i][j] != 0)
	// 					{
	// 						aux = NULL;
	// 						structCopy(&aux, wK);
	// 						aux->updatePosition(&(*aux), aux->row+(i-1), aux->column+(j-1));
	// 						check = check + checkCheck(aux, board);
	// 						free(aux);
	// 					}
	// 				}
	// 			}
	// 		}
	// 		// check = checkCheck(bK, board);
	// 	break;

	// 	default:
	// 	break;
	// }
	// /* Either checkmate or stalemate */
	// if(moves != 0 && moves == check)
	// {
	// 	switch(turn)
	// 	{
	// 		case 'w':
	// 			if(checkCheck(bK, board) && cantBeSaved(nPieces, bP, bN, bB, bQ, bK, bR, board))
	// 			{
	// 				endGame("Cheque-mate -- Vitoria: BRANCO");
	// 				finish = 1;
	// 			}
	// 			else if(!checkCheck(bK, board))
	// 			{
	// 				endGame("Empate -- Afogamento");
	// 			}
	// 		break;

	// 		case 'b':
	// 			if(checkCheck(wK, board) && cantBeSaved(nPieces, wP, wN, wB, wQ, wK, wR, board))
	// 			{
	// 				endGame("Cheque-mate -- Vitoria: PRETO");
	// 				finish = 1;
	// 			}
	// 			else if(!checkCheck(wK, board))
	// 			{
	// 				endGame("Empate -- Afogamento");
	// 			}
	// 		break;

	// 		default:
	// 		break;
	// 	}
	// }

	/* Step 7: read moves until game ends */
	finish = 0;
	while(!finish)
	{
		isPawn = 0;
		check = 0;
		move = NULL;
		move = read(stdin, '\n');
		// printf("move: %s\n", move);
		/* Step 7.1: Break move string to find piece */
		/* Check promotion */
		move1  = NULL;
		move2  = NULL;
		// promotion = 0;
		if(isalpha(move[0]))
		{
			if(strlen(move) == 5)
			{
				move1 = (char*)malloc(sizeof(char)*3);
				move1[0] = move[0];
				move1[1] = move[1];
				move1[2] = '\0';
				move2 = (char*)malloc(sizeof(char)*3);
				move2[0] = move[2];
				move2[1] = move[3];
				move2[2] = '\0';
				// promotion = move[4];
			}
			else
			{
				move1 = (char*)malloc(sizeof(char)*3);
				move1[0] = move[0];
				move1[1] = move[1];
				move1[2] = '\0';
				move2 = (char*)malloc(sizeof(char)*3);
				move2[0] = move[2];
				move2[1] = move[3];
				move2[2] = '\0';
			}
			piece = board[move1[1]-49][move1[0]-97];
			free(move);
		}
		/* Step 7.2: Check if half-turns aren't over 50 */
		if(halfturn >= 50)
		{
			endGame("Empate -- Regra dos 50 Movimentos");
			finish = 1;
		}
		/* Step 7.3: Check lack of materials */
		if(materials <= 3)
		{
			/* Check if it's king and bishop against king */
			if(nPieces[BLACKBISHOP] != 0 && nPieces[BLACKKING] != 0 && nPieces[WHITEKING] != 0)
			{
				endGame("Empate -- Falta de Material");
				finish = 1;
				return 0;
			}
			else if(nPieces[WHITEBISHOP] != 0 && nPieces[WHITEKING] != 0 && nPieces[BLACKKING] != 0)
			{
				endGame("Empate -- Falta de Material");
				finish = 1;
				return 0;
			}
			/* Check it it's king and knight against king */
			else if(nPieces[BLACKKNIGHT] != 0 && nPieces[BLACKKING] != 0 && nPieces[WHITEKING] != 0)
			{
				endGame("Empate -- Falta de Material");
				finish = 1;
				return 0;
			}
			else if(nPieces[WHITEKNIGHT] != 0 && nPieces[WHITEKING] != 0 && nPieces[BLACKKING] != 0)
			{
				endGame("Empate -- Falta de Material");
				finish = 1;
				return 0;
			}
			/* Check if it's only king against king */
			else if(nPieces[BLACKKING] != 0 && nPieces[WHITEKING] != 0)
			{
				endGame("Empate -- Falta de Material");
				finish = 1;
				return 0;	
			}
		}
		/* Step 7.4: Check for threefold repetition - since no input has such output, this case is yet to be tested */
		/* Step 7.5: Check if move is valid */
		if(move1 != NULL && move2 != NULL)
		{
			switch(piece)
			{
				case 'p':
					isPawn = 1;
					halfturn = 0;
					for(i = 0; i < nPieces[BLACKPAWN]; i++)
					{
						/* Pawn found; check all possible moves */
						if(bP[i].row == move1[1]-49 && bP[i].column == move1[0]-97)
						{
							bP[i].updatePosition(&bP[i], move2[1]-49, move2[0]-97);
							i = nPieces[BLACKPAWN] + 1;
							halfturn = -1;
						}
					}
				break;

				case 'P':
					isPawn = 1;
					halfturn = 0;
					for(i = 0; i < nPieces[WHITEPAWN]; i++)
					{
						/* Pawn found; check all possible moves */
						if(wP[i].row == move1[1]-49 && wP[i].column == move1[0]-97)
						{
							wP[i].updatePosition(&wP[i], move2[1]-49, move2[0]-97);
							i = nPieces[WHITEPAWN] + 1;
							halfturn = -1;
						}
					}
				break;

				case 'n':
					for(i = 0; i < nPieces[BLACKKNIGHT]; i++)
					{
						/* Knight found; check all possible moves */
						if(bN[i].row == move1[1]-49 && bN[i].column == move1[0]-97)
						{
							bN[i].updatePosition(&bN[i], move2[1]-49, move2[0]-97);
							i = nPieces[BLACKKNIGHT] + 1;
						}
					}
				break;

				case 'N':
					for(i = 0; i < nPieces[WHITEKNIGHT]; i++)
					{
						/* Knight found; check all possible moves */
						if(wN[i].row == move1[1]-49 && wN[i].column == move1[0]-97)
						{
							wN[i].updatePosition(&wN[i], move2[1]-49, move2[0]-97);
							i = nPieces[WHITEKNIGHT] + 1;
						}
					}
				break;

				case 'b':
					for(i = 0; i < nPieces[BLACKBISHOP]; i++)
					{
						/* Bishop found; check all possible moves */
						if(bB[i].row == move1[1]-49 && bB[i].column == move1[0]-97)
						{
							bB[i].updatePosition(&bB[i], move2[1]-49, move2[0]-97);
							i = nPieces[BLACKBISHOP] + 1;
						}
					}
				break;

				case 'B':
					for(i = 0; i < nPieces[WHITEBISHOP]; i++)
					{
						/* Bishop found; check all possible moves */
						if(wB[i].row == move1[1]-49 && wB[i].column == move1[0]-97)
						{
							wB[i].updatePosition(&wB[i], move2[1]-49, move2[0]-97);
							i = nPieces[WHITEBISHOP] + 1;
						}
					}
				break;

				case 'q':
					for(i = 0; i < nPieces[BLACKQUEEN]; i++)
					{
						/* Queen found; check all possible moves */
						if(bQ[i].row == move1[1]-49 && bQ[i].column == move1[0]-97)
						{
							bQ[i].updatePosition(&bQ[i], move2[1]-49, move2[0]-97);
							i = nPieces[BLACKQUEEN] + 1;
						}
					}
				break;

				case 'Q':
					for(i = 0; i < nPieces[WHITEQUEEN]; i++)
					{
						/* Queen found; check all possible moves */
						if(wQ[i].row == move1[1]-49 && wQ[i].column == move1[0]-97)
						{
							wQ[i].updatePosition(&wQ[i], move2[1]-49, move2[0]-97);
							i = nPieces[WHITEQUEEN] + 1;
						}
					}
				break;

				case 'k':
					for(i = 0; i < nPieces[BLACKKING]; i++)
					{
						/* King found; check all possible moves */
						if(bK[i].row == move1[1]-49 && bK[i].column == move1[0]-97)
						{
							bK[i].updatePosition(&bK[i], move2[1]-49, move2[0]-97);
							i = nPieces[BLACKKING] + 1;
						}
					}
				break;

				case 'K':
					for(i = 0; i < nPieces[WHITEKING]; i++)
					{
						/* King found; check all possible moves */
						if(wK[i].row == move1[1]-49 && wK[i].column == move1[0]-97)
						{
							wK[i].updatePosition(&wK[i], move2[1]-49, move2[0]-97);
							i = nPieces[WHITEKING] + 1;
						}
					}
				break;

				case 'r':
					for(i = 0; i < nPieces[BLACKROOK]; i++)
					{
						/* Rook found; check all possible moves */
						if(bR[i].row == move1[1]-49 && bR[i].column == move1[0]-97)
						{
							bR[i].updatePosition(&bR[i], move2[1]-49, move2[0]-97);
							i = nPieces[BLACKROOK] + 1;
						}
					}
				break;

				case 'R':
					for(i = 0; i < nPieces[WHITEROOK]; i++)
					{
						/* Rook found; check all possible moves */
						if(wR[i].row == move1[1]-49 && wR[i].column == move1[0]-97)
						{
							wR[i].updatePosition(&wR[i], move2[1]-49, move2[0]-97);
							i = nPieces[WHITEROOK] + 1;
						}
					}
				break;

				default: /* Not a piece; do nothing */
				break;
			}
		}
		/* Check if capture occurred */
		/* Step 7.5: Check if move is valid */
		if(move1 != NULL && move2 != NULL)
		{
			piece = board[move2[1]-49][move2[0]-97];
			switch(piece)
			{
				case 'p':
					for(i = 0; i < nPieces[BLACKPAWN]; i++)
					{
						/* Pawn found; set as eaten */
						if(bP[i].row == move2[1]-49 && bP[i].column == move2[0]-97)
						{
							bP[i].eaten = 'Y';
						}
					}
				break;

				case 'P':
					for(i = 0; i < nPieces[WHITEPAWN]; i++)
					{
						/* Pawn found; set as eaten */
						if(wP[i].row == move2[1]-49 && wP[i].column == move2[0]-97)
						{
							wP[i].eaten = 'Y';
						}
					}
				break;

				case 'n':
					for(i = 0; i < nPieces[BLACKKNIGHT]; i++)
					{
						/* Knight found; set as eaten */
						if(bN[i].row == move2[1]-49 && bN[i].column == move2[0]-97)
						{
							bN[i].eaten = 'Y';
						}
					}
				break;

				case 'N':
					for(i = 0; i < nPieces[WHITEKNIGHT]; i++)
					{
						/* Knight found; set as eaten */
						if(wN[i].row == move2[1]-49 && wN[i].column == move2[0]-97)
						{
							wN[i].eaten = 'Y';
						}
					}
				break;

				case 'b':
					for(i = 0; i < nPieces[BLACKBISHOP]; i++)
					{
						/* Bishop found; set as eaten */
						if(bB[i].row == move2[1]-49 && bB[i].column == move2[0]-97)
						{
							bB[i].eaten = 'Y';
						}
					}
				break;

				case 'B':
					for(i = 0; i < nPieces[WHITEBISHOP]; i++)
					{
						/* Bishop found; set as eaten */
						if(wB[i].row == move2[1]-49 && wB[i].column == move2[0]-97)
						{
							wB[i].eaten = 'Y';
						}
					}
				break;

				case 'q':
					for(i = 0; i < nPieces[BLACKQUEEN]; i++)
					{
						/* Queen found; set as eaten */
						if(bQ[i].row == move2[1]-49 && bQ[i].column == move2[0]-97)
						{
							bQ[i].eaten = 'Y';
						}
					}
				break;

				case 'Q':
					for(i = 0; i < nPieces[WHITEQUEEN]; i++)
					{
						/* Queen found; set as eaten */
						if(wQ[i].row == move2[1]-49 && wQ[i].column == move2[0]-97)
						{
							wQ[i].eaten = 'Y';
						}
					}
				break;

				case 'k':
					for(i = 0; i < nPieces[BLACKKING]; i++)
					{
						/* King found; set as eaten */
						if(bK[i].row == move2[1]-49 && bK[i].column == move2[0]-97)
						{
							bK[i].eaten = 'Y';
						}
					}
				break;

				case 'K':
					for(i = 0; i < nPieces[WHITEKING]; i++)
					{
						/* King found; set as eaten */
						if(wK[i].row == move2[1]-49 && wK[i].column == move2[0]-97)
						{
							wK[i].eaten = 'Y';
						}
					}
				break;

				case 'r':
					for(i = 0; i < nPieces[BLACKROOK]; i++)
					{
						/* Rook found; set as eaten */
						if(bR[i].row == move2[1]-49 && bR[i].column == move2[0]-97)
						{
							bR[i].eaten = 'Y';
						}
					}
				break;

				case 'R':
					for(i = 0; i < nPieces[WHITEROOK]; i++)
					{
						/* Rook found; set as eaten */
						if(wR[i].row == move2[1]-49 && wR[i].column == move2[0]-97)
						{
						wR[i].eaten = 'Y';
						}
					}
				break;

				default: /* Not a piece; do nothing */
				break;
			}
		}
		/* Update board */
		if(move1 != NULL && move2 != NULL)
		{
			/* If moved position was occupied, capture; halfturn reset */
			if(board[move2[1]-49][move2[0]-97] != '-')
			{
				halfturn = -1;
				materials = materials - 1;
			}
			halfturn = halfturn + 1;
			/* Update pieces positions */
			updateBoard(board, move1[1]-49, move1[0]-97, move2[1]-49, move2[0]-97);
		}
		/* Update variables */
		if(turn == 'w')
		{
			turn = 'b';
		}
		else if(turn == 'b')
		{
			turnCount = turnCount + 1;
			turn = 'w';
		}
		/* Print with FEN notation */
		if(finish == 0 && move1 != NULL && move2 != NULL)
			printFEN(board, move1, move2, turn, halfturn, turnCount, enPassant, isPawn);
		/* Return for checking checks/checkmates */
		if(turn == 'w')
		{
			turn = 'b';
		}
		else if(turn == 'b')
		{
			turn = 'w';
		}
		/* Step 7.6: Check if adjacent king positions put him in check */
		moveSet = (int**)calloc(3, sizeof(int*));
		for(i = 0; i < 3; i++)
		{
			moveSet[i] = (int*)calloc(3, sizeof(int));
		}
		moves = 0;
		switch(turn)
		{
			case 'w':
				moves = checkNumberOfMoves(bK, board, &moveSet);
				if(moves != 0)
				{
					for(i = 0; i < 3; i++)
					{
						for(j = 0; j < 3; j++)
						{
							if(moveSet[i][j] != 0)
							{
								aux = NULL;
								structCopy(&aux, bK);
								aux->updatePosition(&(*aux), aux->row+(i-1), aux->column+(j-1));
								check = check + checkPossibleCheck(aux, board, bK->row, bK->column, aux->row, aux->column);
								// check = check + checkCheck(aux, board);
								free(aux);
							}
						}
					}
				}
				// check = checkCheck(wK, board);
			break;

			case 'b':
				moves = checkNumberOfMoves(wK, board, &moveSet);
				if(moves != 0)
				{
					for(i = 0; i < 3; i++)
					{
						for(j = 0; j < 3; j++)
						{
							if(moveSet[i][j] != 0)
							{
								aux = NULL;
								structCopy(&aux, wK);
								aux->updatePosition(&(*aux), aux->row+(i-1), aux->column+(j-1));
								check = check + checkPossibleCheck(aux, board, wK->row, wK->column, aux->row, aux->column);
								// check = check + checkCheck(aux, board);
								free(aux);
							}
						}
					}
				}
				// check = checkCheck(bK, board);
			break;

			default:
			break;
		}
		/* Either checkmate or stalemate */
		if(moves != 0 && moves == check)
		{
			switch(turn)
			{
				case 'w':
					if(checkCheck(bK, board) && cantBeSaved(nPieces, bP, bN, bB, bQ, bK, bR, board))
					{
						endGame("Cheque-mate -- Vitoria: BRANCO");
						finish = 1;
					}
					// else if(!checkCheck(bK, board) && !cantBeSaved(nPieces, bP, bN, bB, bQ, bK, bR, board))
					// {
					// 	endGame("Empate -- Afogamento");
					// 	return 0;
					// }
				break;

				case 'b':
					if(checkCheck(wK, board) && cantBeSaved(nPieces, wP, wN, wB, wQ, wK, wR, board))
					{
						endGame("Cheque-mate -- Vitoria: PRETO");
						finish = 1;
					}
					// else if(!checkCheck(wK, board) && !cantBeSaved(nPieces, wP, wN, wB, wQ, wK, wR, board))
					// {
					// 	endGame("Empate -- Afogamento");
					// 	return 0;
					// }
				break;

				default:
				break;
			}
		}
		/* Update variables */
		if(turn == 'w')
		{
			turn = 'b';
		}
		else if(turn == 'b')
		{
			turn = 'w';
		}
		freeMatrix0(moveSet, 3);
		free(move1);
		free(move2);
	}


	/* Step 8: Free variables */
	freeMatrix(board, 8);
	free(input);
	free(nPieces);
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