/*
	Program that reads FEN notation chess input and prints all possible moves (and conditions) to a given player.
	Author: Diego Cintra
	USP Number: 10094043
	Date: 03/05/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	Compare function for qsort
	params:
	   - a: a element to be compared
	   - b: b element to be compared
	returns:
	   - result of comparison between a and b
*/
int cmp(const void *a, const void *b)
{
	int i;
	char *aux, *aux2;
	aux = (char*)malloc(sizeof(char)*strlen(*(char**)a)+1);
	aux2 = (char*)malloc(sizeof(char)*strlen(*(char**)b)+1);
	strcpy(aux, *(char**)a);
	strcpy(aux2, *(char**)b);
	// char *aux = *(char**)a;
	// char *aux2 = *(char**)b;
	if(strlen(aux) == 4 && aux[1] == 'x')
	{
		for(i = 1; i < 3; i++)
		{
			aux[i] = aux[i+1];
		}
	}
	if(strlen(aux2) == 4 && aux2[1] == 'x')
	{
		for(i = 1; i < 3; i++)
		{
			aux2[i] = aux2[i+1];	
		}
	}
	i = strcmp(aux, aux2);
	free(aux);
	free(aux2);
	return i;
}

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
	char **(*checkMoves)(struct _pawn*, char**, char*, char*, int);
}pawn;

/*
	Struct for knight
*/
typedef struct _knight{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _knight*, int, int, char);
	char **(*checkMoves)(struct _knight*, char**, char*, char*);
}knight;

/*
	Struct for bishop
*/
typedef struct _bishop{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _bishop*, int, int, char);
	char **(*checkMoves)(struct _bishop*, char**, char*, char*, int);
}bishop;

/*
	Struct for queen
*/
typedef struct _queen{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _queen*, int, int, char);
	char **(*checkMoves)(struct _queen*, char**, char*, char*, int);
}queen;

/*
	Struct for king
*/
typedef struct _king{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _king*, int, int, char);
	char **(*checkMoves)(struct _king*, char**, char*, char*);
}king;

/*
	Struct for rook
*/
typedef struct _rook{
	int row, column, possibleMoves;
	char isWhite;
	void (*init)(struct _rook*, int, int, char);
	char **(*checkMoves)(struct _rook*, char**, char*, char*, int);
}rook;


/* ------------------------------------------------------------------------------------------------------------ */

/*
	Function declaration of possible moves for each piece
*/

/* ------------------------------------------------------------------------------------------------------------ */

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

char **checkMovesPawn(pawn* p, char **board, char *castling, char *enPassant, int kingMove)
{
	int count = 0;
	char **possibilities;
	possibilities = NULL;
	switch(p->isWhite)
	{
		case 'Y':
			/* Check if forward is clear */
			if(p->row+1 < 8 && board[p->row+1][p->column] == '-')
			{
				if(!checkPromotion(p, board, &possibilities, &count, 0))
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = (p->column)+97;
					possibilities[count][1] = possibilities[count][0];
					possibilities[count][2] = (p->row+1)+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
			}
			/* Check if pawn is at beggining - no need to check promotion */
			if(p->row+1 == 2 && p->row+2 < 8 && board[p->row+1][p->column] == '-' && board[p->row+2][p->column] == '-')
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = (p->column)+97;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = (p->row+1)+48 + 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Check pawn attacks */
			/* Left attack */
			if(p->row+1 < 8 && p->column-1 >= 0 && board[p->row+1][p->column-1] != '-' &&
				board[p->row+1][p->column-1] >= 97)
			{
				if(!checkPromotion(p, board, &possibilities, &count, 1))
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = (p->column)+97;
					possibilities[count][1] = 'x';
					possibilities[count][2] = (p->column)+97 - 1;
					possibilities[count][3] = (p->row+1)+48 + 1;
					possibilities[count][4] = '\0';
					count = count + 1;
				}
			}
			/* Left attack if king moved */
			if(kingMove && p->row+1 < 8 && p->column-1 >= 0 && board[p->row+1][p->column-1] == '-')
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = (p->column)+97 - 1;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = (p->row+1)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Right attack */
			if(p->row+1 < 8 && p->column+1 < 8 && board[p->row+1][p->column+1] != '-' &&
				board[p->row+1][p->column+1] >= 97)
			{
				if(!checkPromotion(p, board, &possibilities, &count, 2))
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = (p->column)+97;
					possibilities[count][1] = 'x';
					possibilities[count][2] = (p->column)+97 + 1;
					possibilities[count][3] = (p->row+1)+48 + 1;
					possibilities[count][4] = '\0';
					count = count + 1;
				}
			}
			/* Right attack if king moved */
			if(kingMove && p->row+1 < 8 && p->column+1 < 8 && board[p->row+1][p->column+1] == '-')
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = (p->column)+97 + 1;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = (p->row+1)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* En Passant - left */
			if(p->row+1 == 5 && p->column-1 >= 0 && board[p->row][p->column-1] == 'p')
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
			if(p->row+1 == 5 && p->column+1 < 8 && board[p->row][p->column+1] == 'p')
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
			if(p->row+1 == 7 && board[p->row-2][p->column] == '-' && board[p->row-1][p->column] == '-')
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = (p->column)+97;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = (p->row+1)+48 - 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Check if forward is clear */
			if(p->row-1 >= 0 && board[p->row-1][p->column] == '-')
			{
				if(!checkPromotion(p, board, &possibilities, &count, 0))
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = (p->column)+97;
					possibilities[count][1] = possibilities[count][0];
					possibilities[count][2] = (p->row+1)+48 - 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
			}
			/* Check pawn attacks */
			/* Left attack */
			if(p->row-1 >= 0 && p->column-1 >= 0 && board[p->row-1][p->column-1] != '-' &&
				board[p->row-1][p->column-1] >= 65 && board[p->row-1][p->column-1] < 97)
			{
				if(!checkPromotion(p, board, &possibilities, &count, 1))
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = (p->column)+97;
					possibilities[count][1] = 'x';
					possibilities[count][2] = (p->column)+97 - 1;
					possibilities[count][3] = (p->row+1)+48 - 1;
					possibilities[count][4] = '\0';
					count = count + 1;
				}
			}
			/* Left attack if king moved */
			if(kingMove && p->row-1 >= 0 && p->column-1 >= 0 && board[p->row-1][p->column-1] == '-')
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = (p->column)+97 - 1;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = (p->row+1)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* Right attack */
			if(p->row-1 >= 0 && p->column+1 < 8 && board[p->row-1][p->column+1] != '-' &&
				board[p->row-1][p->column+1] >= 65 && board[p->row-1][p->column+1] < 97)
			{
				if(!checkPromotion(p, board, &possibilities, &count, 2))
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = (p->column)+97;
					possibilities[count][1] = 'x';
					possibilities[count][2] = (p->column)+97 + 1;
					possibilities[count][3] = (p->row+1)+48 - 1;
					possibilities[count][4] = '\0';
					count = count + 1;
				}
			}
			/* Right attack if king moved */
			if(kingMove && p->row-1 >= 0 && p->column+1 < 8 && board[p->row-1][p->column+1])
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = (p->column)+97 + 1;
				possibilities[count][1] = possibilities[count][0];
				possibilities[count][2] = (p->row+1)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			/* En Passant - left */
			if(p->row+1 == 4 && p->column-1 >= 0 && board[p->row][p->column-1] == 'P')
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
			if(p->row+1 == 4 && p->column+1 < 8 && board[p->row][p->column+1] == 'P')
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
	Checks simultaneous positions for knights
	params:
	   - n: knight structure
	   - moves1: possible moves from first knight
	   - moves2: possible moves from second knight
	returns:
	   - list of moves for both knights
*/
char **checkSimultaneousPositions(knight *n, char **moves1, char **moves2)
{
	int i, j;
	char **possibilities, *tmp;
	possibilities = NULL;
	/* Compare strings from moves1 on moves2 */
	for(i = 0; i < n[0].possibleMoves; i++)
	{
		for(j = 0; j < n[1].possibleMoves; j++)
		{
			if(strcmp(moves1[i], moves2[j]) == 0) /* Same piece at same position */
			{
				/* Change moves1 strings */
				tmp = (char*)malloc(sizeof(char)*strlen(moves1[i])+2);
				tmp[0] = moves1[i][0];
				tmp[1] = n[0].column+97;
				tmp[2] = moves1[i][1];
				tmp[3] = moves1[i][2];
				tmp[4] = '\0';
				moves1[i] = (char*)realloc(moves1[i], sizeof(char)*strlen(moves1[i])+2);
				strcpy(moves1[i], tmp);
				free(tmp);

				/* Change moves2 strings */
				tmp = (char*)malloc(sizeof(char)*strlen(moves2[j])+2);
				tmp[0] = moves2[j][0];
				tmp[1] = n[1].column+97;
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
	possibilities = (char**)realloc(possibilities, sizeof(char*)*(n[0].possibleMoves+n[1].possibleMoves));
	for(i = 0; i < n[0].possibleMoves; i++)
	{
		possibilities[i] = NULL;
		possibilities[i] = (char*)realloc(possibilities[i], sizeof(char)*(strlen(moves1[i])+1));
		strcpy(possibilities[i], moves1[i]);
	}
	for(j = 0; i < n[0].possibleMoves+n[1].possibleMoves; i++, j++)
	{
		possibilities[i] = NULL;
		possibilities[i] = (char*)realloc(possibilities[i], sizeof(char)*(strlen(moves2[j])+1));
		strcpy(possibilities[i], moves2[j]);
	}
	return possibilities;
}

/*
	Checks simultaneous positions for rook
	params:
	   - n: rook structure
	   - moves1: possible moves from first rook
	   - moves2: possible moves from second rook
	returns:
	   - list of moves for both knights
*/
char **checkSimultaneousPositionsRooks(rook *n, char **moves1, char **moves2)
{
	int i, j;
	char **possibilities, *tmp;
	possibilities = NULL;
	/* Compare strings from moves1 on moves2 */
	for(i = 0; i < n[0].possibleMoves; i++)
	{
		for(j = 0; j < n[1].possibleMoves; j++)
		{
			if(strcmp(moves1[i], moves2[j]) == 0) /* Same piece at same position */
			{
				/* Change moves1 strings */
				tmp = (char*)malloc(sizeof(char)*strlen(moves1[i])+2);
				tmp[0] = moves1[i][0];
				tmp[1] = n[0].column+97;
				tmp[2] = moves1[i][1];
				tmp[3] = moves1[i][2];
				tmp[4] = '\0';
				moves1[i] = (char*)realloc(moves1[i], sizeof(char)*strlen(moves1[i])+2);
				strcpy(moves1[i], tmp);
				free(tmp);

				/* Change moves2 strings */
				tmp = (char*)malloc(sizeof(char)*strlen(moves2[j])+2);
				tmp[0] = moves2[j][0];
				tmp[1] = n[1].column+97;
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
	possibilities = (char**)realloc(possibilities, sizeof(char*)*(n[0].possibleMoves+n[1].possibleMoves));
	for(i = 0; i < n[0].possibleMoves; i++)
	{
		possibilities[i] = NULL;
		possibilities[i] = (char*)realloc(possibilities[i], sizeof(char)*(strlen(moves1[i])+1));
		strcpy(possibilities[i], moves1[i]);
	}
	for(j = 0; i < n[0].possibleMoves+n[1].possibleMoves; i++, j++)
	{
		possibilities[i] = NULL;
		possibilities[i] = (char*)realloc(possibilities[i], sizeof(char)*(strlen(moves2[j])+1));
		strcpy(possibilities[i], moves2[j]);
	}
	return possibilities;
}

char **checkMovesKnight(knight* k, char **board, char *castling, char *enPassant)
{
	int count = 0;
	char **possibilities;
	possibilities = NULL;
	/* Knight has 8 possible moves */
	switch(k->isWhite)
	{
		case 'Y':
			/* Check southwest first corner */
			if(k->row-1 >= 0 && k->column-2 >= 0 && board[k->row-1][k->column-2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 2;
				possibilities[count][2] = (k->row+1)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-1 >= 0 && k->column-2 >= 0 && 
				(board[k->row-1][k->column-2] == 'p' || 
				board[k->row-1][k->column-2] == 'n' ||
				board[k->row-1][k->column-2] == 'b' ||
				board[k->row-1][k->column-2] == 'q' ||
				board[k->row-1][k->column-2] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 2;
				possibilities[count][3] = (k->row+1)+48 - 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northwest first corner */
			if(k->row+1 < 8 && k->column-2 >= 0 && board[k->row+1][k->column-2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 2;
				possibilities[count][2] = (k->row+1)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+1 < 8 && k->column-2 >= 0 && 
				(board[k->row+1][k->column-2] == 'p' || 
				board[k->row+1][k->column-2] == 'n' ||
				board[k->row+1][k->column-2] == 'b' ||
				board[k->row+1][k->column-2] == 'q' ||
				board[k->row+1][k->column-2] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 2;
				possibilities[count][3] = (k->row+1)+48 + 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check southwest second corner */
			if(k->row-2 >= 0 && k->column-1 >= 0 && board[k->row-2][k->column-1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 1;
				possibilities[count][2] = (k->row+1)+48 - 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-2 >= 0 && k->column-1 >= 0 && 
				(board[k->row-2][k->column-1] == 'p' || 
				board[k->row-2][k->column-1] == 'n' ||
				board[k->row-2][k->column-1] == 'b' ||
				board[k->row-2][k->column-1] == 'q' ||
				board[k->row-2][k->column-1] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 1;
				possibilities[count][3] = (k->row+1)+48 - 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northwest second corner */
			if(k->row+2 < 8 && k->column-1 >= 0 && board[k->row+2][k->column-1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 1;
				possibilities[count][2] = (k->row+1)+48 + 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+2 < 8 && k->column-1 >= 0 && 
				(board[k->row+2][k->column-1] == 'p' || 
				board[k->row+2][k->column-1] == 'n' ||
				board[k->row+2][k->column-1] == 'b' ||
				board[k->row+2][k->column-1] == 'q' ||
				board[k->row+2][k->column-1] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 1;
				possibilities[count][3] = (k->row+1)+48 + 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check southeast first corner */
			if(k->row-2 >= 0 && k->column+1 < 8 && board[k->row-2][k->column+1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 1;
				possibilities[count][2] = (k->row+1)+48 - 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-2 >= 0 && k->column+1 < 8 && 
				(board[k->row-2][k->column+1] == 'p' || 
				board[k->row-2][k->column+1] == 'n' ||
				board[k->row-2][k->column+1] == 'b' ||
				board[k->row-2][k->column+1] == 'q' ||
				board[k->row-2][k->column+1] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 1;
				possibilities[count][3] = (k->row+1)+48 - 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northeast first corner */
			if(k->row+2 < 8 && k->column+1 < 8 && board[k->row+2][k->column+1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 1;
				possibilities[count][2] = (k->row+1)+48 + 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+2 < 8 && k->column+1 < 8 && 
				(board[k->row+2][k->column+1] == 'p' || 
				board[k->row+2][k->column+1] == 'n' ||
				board[k->row+2][k->column+1] == 'b' ||
				board[k->row+2][k->column+1] == 'q' ||
				board[k->row+2][k->column+1] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 1;
				possibilities[count][3] = (k->row+1)+48 + 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check southeast second corner */
			if(k->row-1 >= 0 && k->column+2 < 8 && board[k->row-1][k->column+2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 2;
				possibilities[count][2] = (k->row+1)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-1 >= 0 && k->column+2 < 8 && 
				(board[k->row-1][k->column+2] == 'p' || 
				board[k->row-1][k->column+2] == 'n' ||
				board[k->row-1][k->column+2] == 'b' ||
				board[k->row-1][k->column+2] == 'q' ||
				board[k->row-1][k->column+2] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 2;
				possibilities[count][3] = (k->row+1)+48 - 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northeast second corner */
			if(k->row+1 < 8 && k->column+2 < 8 && board[k->row+1][k->column+2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 2;
				possibilities[count][2] = (k->row+1)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+1 < 8 && k->column+2 < 8 && 
				(board[k->row+1][k->column+2] == 'p' || 
				board[k->row+1][k->column+2] == 'n' ||
				board[k->row+1][k->column+2] == 'b' ||
				board[k->row+1][k->column+2] == 'q' ||
				board[k->row+1][k->column+2] == 'k')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 2;
				possibilities[count][3] = (k->row+1)+48 + 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}
		break;

		case 'N':
			/* Check southwest first corner */
			if(k->row-1 >= 0 && k->column-2 >= 0 && board[k->row-1][k->column-2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 2;
				possibilities[count][2] = (k->row+1)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-1 >= 0 && k->column-2 >= 0 &&
				(board[k->row-1][k->column-2] == 'P' || 
				board[k->row-1][k->column-2] == 'N' ||
				board[k->row-1][k->column-2] == 'B' ||
				board[k->row-1][k->column-2] == 'Q' ||
				board[k->row-1][k->column-2] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 2;
				possibilities[count][3] = (k->row+1)+48 - 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northwest first corner */
			if(k->row+1 < 8 && k->column-2 >= 0 && board[k->row+1][k->column-2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 2;
				possibilities[count][2] = (k->row+1)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+1 < 8 && k->column-2 >= 0 &&
				(board[k->row+1][k->column-2] == 'P' || 
				board[k->row+1][k->column-2] == 'N' ||
				board[k->row+1][k->column-2] == 'B' ||
				board[k->row+1][k->column-2] == 'Q' ||
				board[k->row+1][k->column-2] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 2;
				possibilities[count][3] = (k->row+1)+48 + 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check southwest second corner */
			if(k->row-2 >= 0 && k->column-1 >= 0 && board[k->row-2][k->column-1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 1;
				possibilities[count][2] = (k->row+1)+48 - 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-2 >= 0 && k->column-1 >= 0 &&
				(board[k->row-2][k->column-1] == 'P' || 
				board[k->row-2][k->column-1] == 'N' ||
				board[k->row-2][k->column-1] == 'B' ||
				board[k->row-2][k->column-1] == 'Q' ||
				board[k->row-2][k->column-1] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 1;
				possibilities[count][3] = (k->row+1)+48 - 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northwest second corner */
			if(k->row+2 < 8 && k->column-1 >= 0 && board[k->row+2][k->column-1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 - 1;
				possibilities[count][2] = (k->row+1)+48 + 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+2 < 8 && k->column-1 >= 0 &&
				(board[k->row+2][k->column-1] == 'P' || 
				board[k->row+2][k->column-1] == 'N' ||
				board[k->row+2][k->column-1] == 'B' ||
				board[k->row+2][k->column-1] == 'Q' ||
				board[k->row+2][k->column-1] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 - 1;
				possibilities[count][3] = (k->row+1)+48 + 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check southeast first corner */
			if(k->row-2 >= 0 && k->column+1 < 8 && board[k->row-2][k->column+1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 1;
				possibilities[count][2] = (k->row+1)+48 - 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-2 >= 0 && k->column+1 < 8 &&
				(board[k->row-2][k->column+1] == 'P' || 
				board[k->row-2][k->column+1] == 'N' ||
				board[k->row-2][k->column+1] == 'B' ||
				board[k->row-2][k->column+1] == 'Q' ||
				board[k->row-2][k->column+1] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 1;
				possibilities[count][3] = (k->row+1)+48 - 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northeast first corner */
			if(k->row+2 < 8 && k->column+1 < 8 && board[k->row+2][k->column+1] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 1;
				possibilities[count][2] = (k->row+1)+48 + 2;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+2 < 8 && k->column+1 < 8 &&
				(board[k->row+2][k->column+1] == 'P' || 
				board[k->row+2][k->column+1] == 'N' ||
				board[k->row+2][k->column+1] == 'B' ||
				board[k->row+2][k->column+1] == 'Q' ||
				board[k->row+2][k->column+1] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 1;
				possibilities[count][3] = (k->row+1)+48 + 2;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check southeast second corner */
			if(k->row-1 >= 0 && k->column+2 < 8 && board[k->row-1][k->column+2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 2;
				possibilities[count][2] = (k->row+1)+48 - 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row-1 >= 0 && k->column+2 < 8 &&
				(board[k->row-1][k->column+2] == 'P' || 
				board[k->row-1][k->column+2] == 'N' ||
				board[k->row-1][k->column+2] == 'B' ||
				board[k->row-1][k->column+2] == 'Q' ||
				board[k->row-1][k->column+2] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 2;
				possibilities[count][3] = (k->row+1)+48 - 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}

			/* Check northeast second corner */
			if(k->row+1 < 8 && k->column+2 < 8 && board[k->row+1][k->column+2] == '-') /* Empty position */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
				possibilities[count][0] = 'N';
				possibilities[count][1] = (k->column)+97 + 2;
				possibilities[count][2] = (k->row+1)+48 + 1;
				possibilities[count][3] = '\0';
				count = count + 1;
			}
			else if(k->row+1 < 8 && k->column+2 < 8 &&
				(board[k->row+1][k->column+2] == 'P' || 
				board[k->row+1][k->column+2] == 'N' ||
				board[k->row+1][k->column+2] == 'B' ||
				board[k->row+1][k->column+2] == 'Q' ||
				board[k->row+1][k->column+2] == 'K')) /* Occupied position - attack */
			{
				possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
				possibilities[count] = NULL;
				possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
				possibilities[count][0] = 'N';
				possibilities[count][1] = 'x';
				possibilities[count][2] = (k->column)+97 + 2;
				possibilities[count][3] = (k->row+1)+48 + 1;
				possibilities[count][4] = '\0';
				count = count + 1;
			}
		break;

		default:
		break;
	}
	k->possibleMoves = count;
	return possibilities;
}

char **checkMovesBishop(bishop* b, char **board, char *castling, char *enPassant, int kingMove)
{
	int i, j, count = 0;
	char **possibilities;
	possibilities = NULL;
	
	switch(b->isWhite)
	{
		case 'Y':
			/* Check lower left diagonal */
			for(i = b->row-1, j = b->column-1; i >= 0 && j >= 0; i--, j--)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = -1;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}

			/* Check upper left diagonal */
			for(i = b->row+1, j = b->column-1; i < 8 && j >= 0; i++, j--)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = 9;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = 9;
				}
			}

			/* Check lower right diagonal */
			for(i = b->row-1, j = b->column+1; i >= 0 && j < 8; i--, j++)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = -1;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}

			/* Check upper right diagonal */
			for(i = b->row+1, j = b->column+1; i < 8 && j < 8; i++, j++)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = 9;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = 9;
				}
			}
		break;

		case 'N':
			/* Check lower left diagonal */
			for(i = b->row-1, j = b->column-1; i >= 0 && j >= 0; i--, j--)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = -1;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}

			/* Check upper left diagonal */
			for(i = b->row+1, j = b->column-1; i < 8 && j >= 0; i++, j--)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = 9;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = 9;
				}
			}

			/* Check lower right diagonal */
			for(i = b->row-1, j = b->column+1; i >= 0 && j < 8; i--, j++)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = -1;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}

			/* Check upper right diagonal */
			for(i = b->row+1, j = b->column+1; i < 8 && j < 8; i++, j++)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*4);
					possibilities[count][0] = 'B';
					possibilities[count][1] = j+97;
					possibilities[count][2] = i+48 + 1;
					possibilities[count][3] = '\0';
					count = count + 1;
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
					possibilities[count] = NULL;
					possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*5);
					possibilities[count][0] = 'B';
					possibilities[count][1] = 'x';
					possibilities[count][2] = j+97;
					possibilities[count][3] = i+48 + 1;
					possibilities[count][4] = '\0';
					/* Since we found a piece, stop */
					i = 9;
					count = count + 1;
				}
				else /* Friendly piece - stop */
				{
					i = 9;
				}
			}
		break;

		default:
		break;
	}
	b->possibleMoves = count;
	/* for loop from */
	return possibilities;
}

char **checkMovesQueen(queen* q, char **board, char *castling, char *enPassant, int kingMove)
{
	int i, j, count = 0;
	char **possibilities;
	possibilities = NULL;
	/* Check all possible positions, regardless of sorting; qsort will handle it later */
	switch(q->isWhite)
	{
		case 'Y':
			/* Check lower left diagonal */
			for(i = q->row-1, j = q->column-1; i >= 0 && j >= 0; i--, j--)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
					/* Since we found a piece, stop */
					i = -1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}
		break;

		case 'N':
			/* Check lower left diagonal */
			for(i = q->row-1, j = q->column-1; i >= 0 && j >= 0; i--, j--)
			{
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
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
				if(kingMove && board[i][j] == '-')
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'Q', 1);
					/* Since we found a piece, stop */
					i = -1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}
		break;

		default:
		break;
	}
	q->possibleMoves = count;
	return possibilities;
}

char **checkMovesKing(king* k, char **board, char *castling, char *enPassant)
{
	int i, j, count = 0;
	char **possibilities;
	possibilities = NULL;
	switch(k->isWhite)
	{
		case 'Y':
			/* Check lower left diagonal */
			i = k->row-1;
			j = k->column-1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
				
			}

			/* Check left */
			i = k->row;
			j = k->column-1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check upper left diagonal */
			i = k->row+1;
			j = k->column-1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check up column */
			i = k->row+1;
			j = k->column;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check lower right diagonal */
			i = k->row-1;
			j = k->column+1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check right row */
			i = k->row;
			j = k->column+1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check upper right row */		
			i = k->row+1;
			j = k->column+1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check down column */
			i = k->row-1;
			j = k->column;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'p' || board[i][j] == 'n' 
				|| board[i][j] == 'b' || board[i][j] == 'q'
				|| board[i][j] == 'k' || board[i][j] == 'r')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Right Castling */
			if(k->column+3 < 8 && k->row+1 == 1 && board[k->row][k->column+1] == '-' && 
				board[k->row][k->column+2] == '-' && board[k->row][k->column+3] == 'R')
			{
				writePossibility(&possibilities, &count, k->row, (k->column+2), 'K', 0);
			}
			/* Left Castling */
			if(k->column-4 >= 0 && k->row+1 == 1 && board[k->row][k->column-1] == '-' && 
				board[k->row][k->column-2] == '-' && board[k->row][k->column-3] == '-' &&
				board[k->row][k->column-4] == 'R')
			{
				writePossibility(&possibilities, &count, k->row, (k->column-2), 'K', 0);
			}
		break;

		case 'N':
			/* Check lower left diagonal */
			i = k->row-1;
			j = k->column-1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check left */
			i = k->row;
			j = k->column-1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check upper left diagonal */
			i = k->row+1;
			j = k->column-1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check up column */
			i = k->row+1;
			j = k->column;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check lower right diagonal */
			i = k->row-1;
			j = k->column+1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check right row */
			i = k->row;
			j = k->column+1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check upper right row */		
			i = k->row+1;
			j = k->column+1;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Check down column */
			i = k->row-1;
			j = k->column;
			if(i >= 0 && i < 8 && j >= 0 && j < 8 && board[i][j] == '-') /* Empty position */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 0);
			}
			else if(i >= 0 && i < 8 && j >= 0 && j < 8 && (board[i][j] == 'P' || board[i][j] == 'N' 
				|| board[i][j] == 'B' || board[i][j] == 'Q'
				|| board[i][j] == 'K' || board[i][j] == 'R')) /* Occupied - attack */
			{
				writePossibility(&possibilities, &count, i, j, 'K', 1);
			}

			/* Right Castling */
			if(k->column+3 < 8 && k->row+1 == 1 && board[k->row][k->column+1] == '-' && 
				board[k->row][k->column+2] == '-' && board[k->row][k->column+3] == 'r')
			{
				writePossibility(&possibilities, &count, k->row, (k->column+2), 'K', 0);
			}
			/* Left Castling */
			if(k->column-4 >= 0 && k->row+1 == 1 && board[k->row][k->column-1] == '-' && 
				board[k->row][k->column-2] == '-' && board[k->row][k->column-3] == '-' &&
				board[k->row][k->column-4] == 'r')
			{
				writePossibility(&possibilities, &count, k->row, (k->column-2), 'K', 0);
			}
		break;

		default:
		break;
	}
	/* Check lower left diagonal */

	k->possibleMoves = count;
	return possibilities;
}

char **checkMovesRook(rook* r, char **board, char *castling, char *enPassant, int kingMove)
{
	int i, j, count = 0;
	char **possibilities;
	possibilities = NULL;
	/* Check all four possible positions, regardless of sorting; qsort will handle it later */
	switch(r->isWhite)
	{
		case 'Y':
			/* Check left row */
			for(i = r->row, j = r->column-1; j >= 0; j--)
			{
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'k'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
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
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'k'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
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
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'k'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
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
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'k'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'p' || board[i][j] == 'n' || board[i][j] == 'b' || board[i][j] == 'q'
					|| board[i][j] == 'k' || board[i][j] == 'r') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
					/* Since we found a piece, stop */
					i = -1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}
		break;

		case 'N':
			/* Check left row */
			for(i = r->row, j = r->column-1; j >= 0; j--)
			{
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'K'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
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
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'K'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
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
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'K'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
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
				if(kingMove && (board[i][j] == '-' || board[i][j] == 'K'))
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);	
				}
				else if(board[i][j] == '-') /* Empty position */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 0);
				}
				else if(board[i][j] == 'P' || board[i][j] == 'N' || board[i][j] == 'B' || board[i][j] == 'Q'
					|| board[i][j] == 'K' || board[i][j] == 'R') /* Occupied - attack */
				{
					writePossibility(&possibilities, &count, i, j, 'R', 1);
					/* Since we found a piece, stop */
					i = -1;
				}
				else /* Friendly piece - stop */
				{
					i = -1;
				}
			}
		break;

		default:
		break;
	}
	r->possibleMoves = count;
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
	Remove any duplicate string between sMatrix and dMatrix
	params:
	   - k: king struct
	   - sMatrix: source matrix to remove any duplicates;
	   - sSize: sMatrix number of rows;
	   - dMatrix: matrix to find duplicates;
	   - dSize: dMatrix number of rows.
*/
void removeDuplicate(king *k, char ***sMatrix, int sSize, char **dMatrix, int dSize)
{
	int i, j, count = 0, *duplicates;
	char **nMatrix;
	nMatrix = NULL;
	duplicates = (int*)calloc(sSize, sizeof(int));
	for(i = 0; i < sSize; i++)
	{
		for(j = 0; j < dSize; j++)
		{
			/* Duplicate - do not copy contents. Store index */
			dMatrix[j][0] = 'K';
			if(strcmp((*sMatrix)[i], dMatrix[j]) == 0)
			{
				duplicates[i] = 1;
			}
		}
	}
	/* Create new vector without duplicates */
	for(i = 0; i < sSize; i++)
	{
		if(duplicates[i] == 0)
		{
			nMatrix = (char**)realloc(nMatrix, sizeof(char*)*(count+1));
			nMatrix[count] = NULL;
			nMatrix[count] = (char*)realloc(nMatrix[count], sizeof(char)*(strlen((*sMatrix)[i])+1));
			strcpy(nMatrix[count], (*sMatrix)[i]);
			count = count + 1;
		}
	}
	free(duplicates);
	freeMatrix((*sMatrix), sSize);
	(*sMatrix) = nMatrix;
	k->possibleMoves = count;
}

int main(void)
{
	/* Local variable declarations */
	/* i, j: loop control variables; nPieces: each position indicates number of pieces read; follows order defined through lines
		459-470 */
	int i, j, *nPieces;
	/* input: string to store commands passed through stdin; board: char type matrix board to annotate elements at given positions;
		board: matrix to store pieces positions
		turn: store current turn
		castling: store castling information

	*/
	char *input, **board, turn, *castling, *enPassant, **printM, **printM2, **printM3;
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
	printMatrix(board, 8, 8);
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
					bP[nPieces[0]].init = (*initPawn);
					bP[nPieces[0]].init(&bP[nPieces[0]], i, j, 'N');
					nPieces[0] = nPieces[0] + 1;
				break;

				case 'P':
					wP[nPieces[1]].init = (*initPawn);
					wP[nPieces[1]].init(&wP[nPieces[1]], i, j, 'Y');
					nPieces[1] = nPieces[1] + 1;
				break;

				case 'n':
					bN[nPieces[2]].init = (*initKnight);
					bN[nPieces[2]].init(&bN[nPieces[2]], i, j, 'N');
					nPieces[2] = nPieces[2] + 1;
				break;

				case 'N':
					wN[nPieces[3]].init = (*initKnight);
					wN[nPieces[3]].init(&wN[nPieces[3]], i, j, 'Y');
					nPieces[3] = nPieces[3] + 1;
				break;

				case 'b':
					bB[nPieces[4]].init = (*initBishop);
					bB[nPieces[4]].init(&bB[nPieces[4]], i, j, 'N');
					nPieces[4] = nPieces[4] + 1;
				break;

				case 'B':
					wB[nPieces[5]].init = (*initBishop);
					wB[nPieces[5]].init(&wB[nPieces[5]], i, j, 'Y');
					nPieces[5] = nPieces[5] + 1;
				break;

				case 'q':
					bQ[nPieces[6]].init = (*initQueen);
					bQ[nPieces[6]].init(&bQ[nPieces[6]], i, j, 'N');
					nPieces[6] = nPieces[6] + 1;
				break;

				case 'Q':
					wQ[nPieces[7]].init = (*initQueen);
					wQ[nPieces[7]].init(&wQ[nPieces[7]], i, j, 'Y');
					nPieces[7] = nPieces[7] + 1;
				break;

				case 'k':
					bK[nPieces[8]].init = (*initKing);
					bK[nPieces[8]].init(&bK[nPieces[8]], i, j, 'N');
					nPieces[8] = nPieces[8] + 1;
				break;

				case 'K':
					wK[nPieces[9]].init = (*initKing);
					wK[nPieces[9]].init(&wK[nPieces[9]], i, j, 'Y');
					nPieces[9] = nPieces[9] + 1;
				break;

				case 'r':
					bR[nPieces[10]].init = (*initRook);
					bR[nPieces[10]].init(&bR[nPieces[10]], i, j, 'N');
					nPieces[10] = nPieces[10] + 1;
				break;

				case 'R':
					wR[nPieces[11]].init = (*initRook);
					wR[nPieces[11]].init(&wR[nPieces[11]], i, j, 'Y');
					nPieces[11] = nPieces[11] + 1;
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
		/* Check pawns moves */
		for(i = 0; i < nPieces[1]; i++)
		{
			printM = wP[i].checkMoves(&wP[i], board, castling, enPassant, 0);
			printMatrix(printM, wP[i].possibleMoves, -1);
			freeMatrix(printM, wP[i].possibleMoves);
			printM = NULL;
		}
		/* Check knights moves */
		for(i = 0; i < nPieces[3]; i++)
		{
			if(i == 0)
			{
				printM = wN[i].checkMoves(&wN[i], board, castling, enPassant);
				/*printMatrix(printM, wN[i].possibleMoves, -1);
				freeMatrix(printM, wN[i].possibleMoves);
				printM = NULL;*/
			}
			else
			{
				printM2 = wN[i].checkMoves(&wN[i], board, castling, enPassant);
				/*printMatrix(printM, wN[i].possibleMoves, -1);
				freeMatrix(printM, wN[i].possibleMoves);
				printM = NULL;*/
			}
		}

		if(i == 2)
		{
			printM3 = checkSimultaneousPositions(wN, printM, printM2);
			printMatrix(printM3, (wN[0].possibleMoves+wN[1].possibleMoves), -1);
			freeMatrix(printM, wN[0].possibleMoves);
			freeMatrix(printM2, wN[1].possibleMoves);
			freeMatrix(printM3, (wN[0].possibleMoves+wN[1].possibleMoves));
			printM = NULL;
			printM2 = NULL;
		}
		else if(i == 1)
		{
			printMatrix(printM, wN[0].possibleMoves, -1);
			freeMatrix(printM, wN[0].possibleMoves);
			printM = NULL;
		}

		/* Check bishops moves */
		for(i = 0; i < nPieces[5]; i++)
		{
			printM = wB[i].checkMoves(&wB[i], board, castling, enPassant, 0);
			qsort(printM, wB[i].possibleMoves, sizeof(char*), cmp);
			printMatrix(printM, wB[i].possibleMoves, -1);
			freeMatrix(printM, wB[i].possibleMoves);
			printM = NULL;
		}

		/* Check rooks moves */
		for(i = 0; i < nPieces[11]; i++)
		{
			if(i == 0)
			{
				printM = wR[i].checkMoves(&wR[i], board, castling, enPassant, 0);
				// qsort(printM, wR[i].possibleMoves, sizeof(char*), cmp);
				// printMatrix(printM, wR[i].possibleMoves, -1);
				// freeMatrix(printM, wR[i].possibleMoves);
				// printM = NULL;
			}
			else
			{
				printM2 = wR[i].checkMoves(&wR[i], board, castling, enPassant, 0);
			}
		}
		if(i == 2)
		{
			printM3 = checkSimultaneousPositionsRooks(wR, printM, printM2);
			qsort(printM3, (wR[0].possibleMoves+wR[1].possibleMoves), sizeof(char*), cmp);
			printMatrix(printM3, (wR[0].possibleMoves+wR[1].possibleMoves), -1);
			freeMatrix(printM, wR[0].possibleMoves);
			freeMatrix(printM2, wR[1].possibleMoves);
			freeMatrix(printM3, (wR[0].possibleMoves+wR[1].possibleMoves));
			printM = NULL;
			printM2 = NULL;
		}
		else if(i == 1)
		{
			qsort(printM, wR[0].possibleMoves, sizeof(char*), cmp);
			printMatrix(printM, wR[0].possibleMoves, -1);
			freeMatrix(printM, wR[0].possibleMoves);
			printM = NULL;
		}

		/* Check queen's moves */
		if(nPieces[7] > 0)
		{
			printM = wQ[0].checkMoves(&wQ[0], board, castling, enPassant, 0);
			qsort(printM, wQ[0].possibleMoves, sizeof(char*), cmp);
			printMatrix(printM, wQ[0].possibleMoves, -1);
			freeMatrix(printM, wQ[0].possibleMoves);
			printM = NULL;
		}
		/* Check king's moves*/
		if(nPieces[9] > 0)
		{
			printM = wK[0].checkMoves(&wK[0], board, castling, enPassant);
			qsort(printM, wK[0].possibleMoves, sizeof(char*), cmp);
			// freeMatrix(printM, wK[0].possibleMoves);
		}

		/* Check all oponent's moves to remove any possible check or checkmate */
		/* Check pawns moves */
		for(i = 0; i < nPieces[0]; i++)
		{
			printM2 = bP[i].checkMoves(&bP[i], board, castling, enPassant, 1);
			qsort(printM2, bP[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&wK[0], &printM, wK[0].possibleMoves, printM2, bP[i].possibleMoves);
			freeMatrix(printM2, bP[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check knights moves */
		for(i = 0; i < nPieces[2]; i++)
		{
			printM2 = bN[i].checkMoves(&bN[i], board, castling, enPassant);
			qsort(printM2, bN[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&wK[0], &printM, wK[0].possibleMoves, printM2, bN[i].possibleMoves);
			freeMatrix(printM2, bN[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check bishops moves */
		for(i = 0; i < nPieces[4]; i++)
		{
			printM2 = bB[i].checkMoves(&bB[i], board, castling, enPassant, 1);
			qsort(printM2, bB[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&wK[0], &printM, wK[0].possibleMoves, printM2, bB[i].possibleMoves);
			freeMatrix(printM2, bB[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check rooks moves */
		for(i = 0; i < nPieces[10]; i++)
		{
			printM2 = bR[i].checkMoves(&bR[i], board, castling, enPassant, 1);
			qsort(printM2, bR[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&wK[0], &printM, wK[0].possibleMoves, printM2, bR[i].possibleMoves);
			freeMatrix(printM2, bR[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check queen's moves */
		if(nPieces[6] > 0)
		{
			printM2 = bQ[0].checkMoves(&bQ[0], board, castling, enPassant, 1);
			qsort(printM2, bQ[0].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&wK[0], &printM, wK[0].possibleMoves, printM2, bQ[0].possibleMoves);
			freeMatrix(printM2, bQ[0].possibleMoves);
			printM2 = NULL;
		}
		/* Check king's moves */
		if(nPieces[8] > 0)
		{
			printM2 = bK[0].checkMoves(&bK[0], board, castling, enPassant);
			qsort(printM2, bK[0].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&wK[0], &printM, wK[0].possibleMoves, printM2, bK[0].possibleMoves);
			freeMatrix(printM2, bK[0].possibleMoves);
			printM2 = NULL;
		}

		/* Finally, print king's possible moves */
		if(nPieces[9] > 0)
		{
			printMatrix(printM, wK[0].possibleMoves, -1);
			freeMatrix(printM, wK[0].possibleMoves);
		}
	}
	else
	{
		/* Check pawns moves */
		for(i = 0; i < nPieces[0]; i++)
		{
			printM = bP[i].checkMoves(&bP[i], board, castling, enPassant, 0);
			printMatrix(printM, bP[i].possibleMoves, -1);
			freeMatrix(printM, bP[i].possibleMoves);
			printM = NULL;
		}

		/* Check knights moves */
		for(i = 0; i < nPieces[2]; i++)
		{
			if(i == 0)
			{
				printM = bN[i].checkMoves(&bN[i], board, castling, enPassant);
				/*printMatrix(printM, wN[i].possibleMoves, -1);
				freeMatrix(printM, wN[i].possibleMoves);
				printM = NULL;*/
			}
			else
			{
				printM2 = bN[i].checkMoves(&bN[i], board, castling, enPassant);
				/*printMatrix(printM, wN[i].possibleMoves, -1);
				freeMatrix(printM, wN[i].possibleMoves);
				printM = NULL;*/
			}
		}
		if(i == 2)
		{
			printM3 = checkSimultaneousPositions(bN, printM, printM2);
			printMatrix(printM3, (bN[0].possibleMoves+bN[1].possibleMoves), -1);
			freeMatrix(printM, bN[0].possibleMoves);
			freeMatrix(printM2, bN[1].possibleMoves);
			freeMatrix(printM3, (bN[0].possibleMoves+bN[1].possibleMoves));
			printM = NULL;
			printM2 = NULL;
		}
		else if(i == 1)
		{

			printMatrix(printM, bN[0].possibleMoves, -1);
			freeMatrix(printM, bN[0].possibleMoves);
			printM = NULL;
		}

		/* Check bishops moves */
		for(i = 0; i < nPieces[4]; i++)
		{
			printM = bB[i].checkMoves(&bB[i], board, castling, enPassant, 0);
			qsort(printM, bB[i].possibleMoves, sizeof(char*), cmp);
			printMatrix(printM, bB[i].possibleMoves, -1);
			freeMatrix(printM, bB[i].possibleMoves);
			printM = NULL;
		}

		/* Check rooks moves */
		for(i = 0; i < nPieces[10]; i++)
		{
			if(i == 0)
			{
				printM = bR[i].checkMoves(&bR[i], board, castling, enPassant, 0);
				// qsort(printM, wR[i].possibleMoves, sizeof(char*), cmp);
				// printMatrix(printM, wR[i].possibleMoves, -1);
				// freeMatrix(printM, wR[i].possibleMoves);
				// printM = NULL;
			}
			else
			{
				printM2 = bR[i].checkMoves(&bR[i], board, castling, enPassant, 0);
			}
		}
		if(i == 2)
		{
			printM3 = checkSimultaneousPositionsRooks(bR, printM, printM2);
			qsort(printM3, (bR[0].possibleMoves+bR[1].possibleMoves), sizeof(char*), cmp);
			printMatrix(printM3, (bR[0].possibleMoves+bR[1].possibleMoves), -1);
			freeMatrix(printM, bR[0].possibleMoves);
			freeMatrix(printM2, bR[1].possibleMoves);
			freeMatrix(printM3, (bR[0].possibleMoves+bR[1].possibleMoves));
			printM = NULL;
			printM2 = NULL;
		}
		else if(i == 1)
		{
			qsort(printM, (bR[0].possibleMoves), sizeof(char*), cmp);
			printMatrix(printM, bR[0].possibleMoves, -1);
			freeMatrix(printM, bR[0].possibleMoves);
			printM = NULL;
		}

		/* Check queen's moves */
		if(nPieces[6] > 0)
		{
			printM = bQ[0].checkMoves(&bQ[0], board, castling, enPassant, 0);
			qsort(printM, bQ[0].possibleMoves, sizeof(char*), cmp);
			printMatrix(printM, bQ[0].possibleMoves, -1);
			freeMatrix(printM, bQ[0].possibleMoves);
			printM = NULL;
		}

		/* Check king's moves*/
		if(nPieces[8] > 0)
		{
			printM = bK[0].checkMoves(&bK[0], board, castling, enPassant);
			qsort(printM, bK[0].possibleMoves, sizeof(char*), cmp);
			// printMatrix(printM, wK[0].possibleMoves, -1);
			// freeMatrix(printM, wK[0].possibleMoves);
		}

		/* Check all oponent's moves to remove any possible check or checkmate */
		/* Check pawns moves */
		for(i = 0; i < nPieces[1]; i++)
		{
			printM2 = wP[i].checkMoves(&wP[i], board, castling, enPassant, 1);
			qsort(printM2, wP[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&bK[0], &printM, bK[0].possibleMoves, printM2, wP[i].possibleMoves);
			freeMatrix(printM2, wP[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check knights moves */
		for(i = 0; i < nPieces[3]; i++)
		{
			printM2 = wN[i].checkMoves(&wN[i], board, castling, enPassant);
			qsort(printM2, wN[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&bK[0], &printM, bK[0].possibleMoves, printM2, wN[i].possibleMoves);
			freeMatrix(printM2, wN[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check bishops moves */
		for(i = 0; i < nPieces[5]; i++)
		{
			printM2 = wB[i].checkMoves(&wB[i], board, castling, enPassant, 1);
			qsort(printM2, wB[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&bK[0], &printM, bK[0].possibleMoves, printM2, wB[i].possibleMoves);
			freeMatrix(printM2, wB[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check rooks moves */
		for(i = 0; i < nPieces[11]; i++)
		{
			printM2 = wR[i].checkMoves(&wR[i], board, castling, enPassant, 1);
			qsort(printM2, wR[i].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&bK[0], &printM, bK[0].possibleMoves, printM2, wR[i].possibleMoves);
			freeMatrix(printM2, wR[i].possibleMoves);
			printM2 = NULL;
		}

		/* Check queen's moves */
		if(nPieces[7] > 0)
		{
			printM2 = wQ[0].checkMoves(&wQ[0], board, castling, enPassant, 1);
			qsort(printM2, wQ[0].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&bK[0], &printM, bK[0].possibleMoves, printM2, wQ[0].possibleMoves);
			freeMatrix(printM2, wQ[0].possibleMoves);
			printM2 = NULL;
		}
		/* Check king's moves */
		if(nPieces[9] > 0)
		{
			printM2 = wK[0].checkMoves(&wK[0], board, castling, enPassant);
			qsort(printM2, wK[0].possibleMoves, sizeof(char*), cmp);
			removeDuplicate(&bK[0], &printM, bK[0].possibleMoves, printM2, wK[0].possibleMoves);
			freeMatrix(printM2, wK[0].possibleMoves);
			printM2 = NULL;
		}

		/* Finally, print king's possible moves */
		if(nPieces[8] > 0)
		{
			printMatrix(printM, bK[0].possibleMoves, -1);
			freeMatrix(printM, bK[0].possibleMoves);
		}
	}
	
	/* Step ???: Free variables */
	freeMatrix(board, 8);
	free(input);
	free(castling);
	free(enPassant);
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