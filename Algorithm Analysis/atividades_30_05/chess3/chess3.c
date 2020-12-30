/*
	Program that plays a chess game, ranking best moves and choosing the best one.
	Author: Diego S. Cintra
	USP Number: 10094043
	Date: 18/05/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#ifdef EAT
#define EAT 1
#else
#define EAT 0
#endif

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

typedef struct _chessBoard chessBoard;
void freeMatrix(char**, int);
void printMatrix(char**, int, int);
void chessBoardStructCopy(chessBoard**, chessBoard*);

/*
	Linked list struct
*/
typedef struct _tList{
	char *data;
	int count;
	struct _tList *next;
}tList;

typedef tList* List;

void initList(tList **l)
{
	(*l)->data = NULL;
	(*l)->count = 0;
	(*l)->next = NULL;
}

/*
	Insert into linked list
	params:
	   - element: element to be inserted;
	   - lList: linked list
*/
void insert(char *element, List *lList)
{
	tList *head = (tList*)malloc(sizeof(tList)*1);
	if((*lList) == NULL)
	{
		initList(&head);
		head->data = (char*)malloc(sizeof(char)*(strlen(element)+1));
		strcpy(head->data, element);
		head->count = 0;
		head->next = NULL;
		(*lList) = head;
	}
	else
	{
		head->data = (char*)malloc(sizeof(char)*(strlen(element)+1));
		strcpy(head->data, element);
		head->count = 0;
		head->next = (*lList);
		(*lList) = head;
	}
	(*lList)->count = (*lList)->count + 1;
}

/*
	Remove from linked list
	params:
	   - element: element to be removed;
	   - lList: linked list
	returns:
	   - 1 on success, -1 otherwise
*/
int listRemove(char *element, List *lList)
{
	tList *r = (tList*)malloc(sizeof(tList)*1);
	r = (*lList);
	while(r->next)
	{
		if(strcmp(r->next->data, element) == 0)
		{
			r->next = r->next->next;
			free(r);
			return 1;
		}
		r = r->next;
	}
	r = (*lList);
	if(strcmp(r->next->data, element) == 0)
	{
		r->next = NULL;
		free(r);
		return 1;
	}
	free(r);
	return -1;
}

/*
	Search from linked list
	params:
	   - element: element to be searched;
	   - lList: linked list
	returns:
	   - pointer to position on success, NULL otherwise
*/
tList *listSearch(char *element, List *lList)
{
	tList *r = NULL;
	r = (*lList);
	if(r)
	{
		if(strcmp(r->data, element) == 0)
		{
			return r;
		}
		while(r->next)
		{
			if(strcmp(r->data, element) == 0)
			{
				return r;
			}
			r = r->next;
		}
	}
	return NULL;
}

void listPrint(tList *lList)
{
	tList *r = NULL;
	r = lList;
	while(r)
	{
		printf("r->data: %s r->count: %d ", r->data, r->count);
		r = r->next;
	}
	printf("\n");
	free(r);
}


/*
	Node struct
*/
typedef struct _node{
	/* Number of elements in linked list */
	int nElements;
	/* Indicates if game is over */
	int endGame;
	/* Linked list */
	tList *keys;
}node;

void initNode(node **n)
{
	if(!(*n))
	{
		(*n) = (node*)malloc(sizeof(node));
	}
	(*n)->nElements = 0;
	(*n)->endGame = 0;
	(*n)->keys = NULL;
}

void initNodes(node **n, int m)
{
	int i;
	for(i = 0; i < m; i++)
	{
		initNode(&(*n));
	}
}

/*
	Hash struct
*/
typedef struct _hash{
	int m;
	node *hashNode;
}hash;

void initHash(hash **h, int m)
{
	if(!(*h))
	{
		(*h) = (hash*)malloc(sizeof(hash));
	}
	(*h)->hashNode = (node*)calloc(m, sizeof(node));
	initNodes(&(*h)->hashNode, m);
	(*h)->m = m;
}

int hashDivision(hash *h, char *key)
{
	int i, value = 0;
	for(i = 0; i < strlen(key); i++)
	{
		value = value + (key[i]*(i+1));
	}
	return (value % h->m);
}

void insertHashNode(node *n, char *key)
{
	tList *element = NULL;
	element = listSearch(key, &n->keys);
	if(element)
	{
		element->count = element->count + 1;
		/* Threefold repetition; end of game */
		if(element->count > 2)
		{
			n->endGame = 1;
		}
	}
	else
	{
		insert(key, &n->keys);
		n->nElements = n->nElements + 1;
	}
}

void putHash(hash *h, char *key)
{
	int i;
	i = hashDivision(h, key);
	insertHashNode(&h->hashNode[i], key);
}

/*
	Structs representing each piece
	general params:
	   - row, column: position at board
	   - pointer to init: function that will initialize struct
	   - pointer to checkMoves: function that will check its possible moves
*/
/* ------------------------------------------------------------------------------------------------------------ */

/*
	General struct for piece
*/
typedef struct _piece{
	int row, column, possibleMoves;
	char isWhite, eaten;
	void (*init)(struct _piece*, int, int, int, char, char);
}piece;

/*
	Struct for king
*/
typedef struct _king{
	piece *peca;
	void (*init)(struct _king*, int, int, char, char);
	char **(*checkMoves)(struct _king*, chessBoard*);
	double (*getScore)(chessBoard*, struct _king*, int);
}king;

/*
	Struct for pawn
*/
typedef struct _pawn{
	piece *peca;
	void (*init)(struct _pawn*, int, int, char, char);
	char **(*checkMoves)(struct _pawn*, king*, chessBoard*);
	double (*getScore)(chessBoard*, struct _pawn*, int);
}pawn;

/*
	Struct for knight
*/
typedef struct _knight{
	piece *peca;
	void (*init)(struct _knight*, int, int, char, char);
	char **(*checkMoves)(struct _knight*, king*, chessBoard*);
	double (*getScore)(chessBoard*, struct _knight*, int);
}knight;

/*
	Struct for bishop
*/
typedef struct _bishop{
	piece *peca;
	void (*init)(struct _bishop*, int, int, char, char);
	char **(*checkMoves)(struct _bishop*, king*, chessBoard*);
	double (*getScore)(chessBoard*, struct _bishop*, int);
}bishop;

/*
	Struct for queen
*/
typedef struct _queen{
	piece *peca;
	void (*init)(struct _queen*, int, int, char, char);
	char **(*checkMoves)(struct _queen*, king*, chessBoard*);
	double (*getScore)(chessBoard*, struct _queen*, int);
}queen;

/*
	Struct for rook
*/
typedef struct _rook{
	piece *peca;
	void (*init)(struct _rook*, int, int, char, char);
	char **(*checkMoves)(struct _rook*, king*, chessBoard*);
	double (*getScore)(chessBoard*, struct _rook*, int);
}rook;

/*
	Struct to store chess board
*/
struct _chessBoard{
	char **board, turn, *castling, *enPassant;
	int *nPieces, materials, halfturn, turnCount, **visitedBoard;
	pawn *bP, *wP;
	knight *bN, *wN;
	bishop *bB, *wB;
	queen *bQ, *wQ;
	king *bK, *wK;
	rook *bR, *wR;
	hash *h;
	void (*updatePosition)(struct _chessBoard*, piece*, int, int, char, int);
	void (*updateTurn)(struct _chessBoard*);
};

/*
	Struct to store possible AI move
*/
typedef struct _possibleMove{
	piece *peca;
	int row, column, castling;
	double value;
}possibleMove;

/*
	Initialize piece struct
*/
void initPiece(piece *peca, int row, int column, int possibleMoves, char isWhite, char eaten)
{
	peca->row = row;
	peca->column = column;
	peca->possibleMoves = possibleMoves;
	peca->isWhite = isWhite;
	peca->eaten = eaten;
}

/*
	Initialize possibleMove struct
*/
void initPossibleMove(possibleMove *p, piece *peca, /*int row, int column, int possibleMoves, char isWhite, char eaten,*/ int newRow, int newColumn)
{
	// p->peca = (piece*)malloc(sizeof(piece));
	// p->peca->init = (*initPiece);
	// p->peca->init(p->peca, row, column, possibleMoves, isWhite, eaten);
	p->peca = NULL;
	p->peca = peca;
	p->value = 0;
	p->row = newRow;
	p->column = newColumn;
	p->castling = 0;
}

char **checkMovesPawn(pawn*, king*, chessBoard*);
double getScorePawn(chessBoard*, pawn*, int);
char **checkMovesKnight(knight*, king*, chessBoard*);
double getScoreKnight(chessBoard*, knight*, int);
char **checkMovesBishop(bishop*, king*, chessBoard*);
double getScoreBishop(chessBoard*, bishop*, int);
char **checkMovesQueen(queen*, king*, chessBoard*);
double getScoreQueen(chessBoard*, queen*, int);
char **checkMovesKing(king*, chessBoard*);
double getScoreKing(chessBoard*, king*, int);
char **checkMovesRook(rook*, king*, chessBoard*);
double getScoreRook(chessBoard*, rook*, int);

void initPawn(pawn *p, int i, int j, char isWhite, char eaten)
{
	p->peca = (piece*)malloc(sizeof(piece)*1);
	p->peca->init = (*initPiece);
	p->peca->init(p->peca, i, j, 0, isWhite, eaten);
	p->checkMoves = (**checkMovesPawn);
	p->getScore = (*getScorePawn);
}

void initKnight(knight *k, int i, int j, char isWhite, char eaten)
{
	k->peca = (piece*)malloc(sizeof(piece)*1);
	k->peca->init = (*initPiece);
	k->peca->init(k->peca, i, j, 0, isWhite, eaten);
	k->checkMoves = (**checkMovesKnight);
	k->getScore = (*getScoreKnight);
}

void initBishop(bishop *b, int i, int j, char isWhite, char eaten)
{
	b->peca = (piece*)malloc(sizeof(piece)*1);
	b->peca->init = (*initPiece);
	b->peca->init(b->peca, i, j, 0, isWhite, eaten);
	b->checkMoves = (**checkMovesBishop);
	b->getScore = (*getScoreBishop);
}

void initQueen(queen *q, int i, int j, char isWhite, char eaten)
{
	q->peca = (piece*)malloc(sizeof(piece)*1);
	q->peca->init = (*initPiece);
	q->peca->init(q->peca, i, j, 0, isWhite, eaten);
	q->checkMoves = (**checkMovesQueen);
	q->getScore = (*getScoreQueen);
}

void initKing(king *k, int i, int j, char isWhite, char eaten)
{
	k->peca = (piece*)malloc(sizeof(piece)*1);
	k->peca->init = (*initPiece);
	k->peca->init(k->peca, i, j, 0, isWhite, eaten);
	k->checkMoves = (**checkMovesKing);
	k->getScore = (*getScoreKing);
}

void initRook(rook *r, int i, int j, char isWhite, char eaten)
{
	r->peca = (piece*)malloc(sizeof(piece)*1);
	r->peca->init = (*initPiece);
	r->peca->init(r->peca, i, j, 0, isWhite, eaten);
	r->checkMoves = (**checkMovesRook);
	r->getScore = (*getScoreRook);
}

void freeMovement(possibleMove **movement)
{
	int i = 0;
	while(movement[i])
	{
		// free((*movement+i)->peca);
		// free((*movement+i));
		i = i + 1;
	}
}

void freeMatrix2(int **matrix, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

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
	if((*(possibleMove*)a).value < (*(possibleMove*)b).value) 
	{
		return 1;
	}
	else if((*(possibleMove*)a).value == (*(possibleMove*)b).value) 
	{
		return 0;
	}
	else
	{ 
		return -1;
	}
}

/*
	Compare function for qsort
	params:
	   - a: a element to be compared
	   - b: b element to be compared
	returns:
	   - result of comparison between a and b
*/
int cmp2(const void *a, const void *b)
{
	if(strcmp( (*(char**)a), (*(char**)b) ) < 0)
	{
		return -1;
	}
	else if(strcmp( (*(char**)a), (*(char**)b) ) == 0)
	{
		return 0;
	}
	else
		return 1;
}

/*
	Get a char and returns either its lower case or upper case
	param:
	   - c: character
	returns:
	   - either lower case or upper case
*/
char reverseCase(char c)
{
	/* Lower case */
	if(c >= 97)
	{
		return toupper(c);
	}
	else
	{
		return tolower(c);
	}
}

/*
	Update game in case of capture
	params:
	   - tabuleiro: struct containing chessboard;
	   - peca: piece struct;
	   - row: new row;
	   - column: new column
*/
void updateCapture(chessBoard *tabuleiro, piece *peca, int row, int column)
{
	int i;
	for(i = 0; i < tabuleiro->nPieces[BLACKPAWN]; i++)
	{
		if(tabuleiro->bP[i].peca->eaten == 'N' && tabuleiro->bP[i].peca->row == row && tabuleiro->bP[i].peca->column == column)
		{
			// tabuleiro->nPieces[BLACKPAWN] = tabuleiro->nPieces[BLACKPAWN] - 1;
			tabuleiro->bP[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEPAWN]; i++)
	{
		if(tabuleiro->wP[i].peca->eaten == 'N' && tabuleiro->wP[i].peca->row == row && tabuleiro->wP[i].peca->column == column)
		{
			// tabuleiro->nPieces[WHITEPAWN] = tabuleiro->nPieces[WHITEPAWN] - 1;
			tabuleiro->wP[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}	
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKNIGHT]; i++)
	{
		if(tabuleiro->bN[i].peca->eaten == 'N' && tabuleiro->bN[i].peca->row == row && tabuleiro->bN[i].peca->column == column)
		{
			// tabuleiro->nPieces[BLACKKNIGHT] = tabuleiro->nPieces[BLACKKNIGHT] - 1;
			tabuleiro->bN[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEKNIGHT]; i++)
	{
		if(tabuleiro->wN[i].peca->eaten == 'N' && tabuleiro->wN[i].peca->row == row && tabuleiro->wN[i].peca->column == column)
		{
			// tabuleiro->nPieces[WHITEKNIGHT] = tabuleiro->nPieces[WHITEKNIGHT] - 1;
			tabuleiro->wN[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKBISHOP]; i++)
	{
		if(tabuleiro->bB[i].peca->eaten == 'N' && tabuleiro->bB[i].peca->row == row && tabuleiro->bB[i].peca->column == column)
		{
			// tabuleiro->nPieces[BLACKBISHOP] = tabuleiro->nPieces[BLACKBISHOP] - 1;
			tabuleiro->bB[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEBISHOP]; i++)
	{
		if(tabuleiro->wB[i].peca->eaten == 'N' && tabuleiro->wB[i].peca->row == row && tabuleiro->wB[i].peca->column == column)
		{
			// tabuleiro->nPieces[WHITEBISHOP] = tabuleiro->nPieces[WHITEBISHOP] - 1;
			tabuleiro->wB[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKQUEEN]; i++)
	{
		if(tabuleiro->bQ[i].peca->eaten == 'N' && tabuleiro->bQ[i].peca->row == row && tabuleiro->bQ[i].peca->column == column)
		{
			// tabuleiro->nPieces[BLACKQUEEN] = tabuleiro->nPieces[BLACKQUEEN] - 1;
			tabuleiro->bQ[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEQUEEN]; i++)
	{
		if(tabuleiro->wQ[i].peca->eaten == 'N' && tabuleiro->wQ[i].peca->row == row && tabuleiro->wQ[i].peca->column == column)
		{
			// tabuleiro->nPieces[WHITEQUEEN] = tabuleiro->nPieces[WHITEQUEEN] - 1;
			tabuleiro->wQ[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKING]; i++)
	{
		if(tabuleiro->bK[i].peca->eaten == 'N' && tabuleiro->bK[i].peca->row == row && tabuleiro->bK[i].peca->column == column)
		{
			// tabuleiro->nPieces[BLACKKING] = tabuleiro->nPieces[BLACKKING] - 1;
			tabuleiro->bK[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEKING]; i++)
	{
		if(tabuleiro->wK[i].peca->eaten == 'N' && tabuleiro->wK[i].peca->row == row && tabuleiro->wK[i].peca->column == column)
		{
			// tabuleiro->nPieces[WHITEKING] = tabuleiro->nPieces[WHITEKING] - 1;
			tabuleiro->wK[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKROOK]; i++)
	{
		if(tabuleiro->bR[i].peca->eaten == 'N' && tabuleiro->bR[i].peca->row == row && tabuleiro->bR[i].peca->column == column)
		{
			// tabuleiro->nPieces[BLACKROOK] = tabuleiro->nPieces[BLACKROOK] - 1;
			tabuleiro->bR[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEROOK]; i++)
	{
		if(tabuleiro->wR[i].peca->eaten == 'N' && tabuleiro->wR[i].peca->row == row && tabuleiro->wR[i].peca->column == column)
		{
			// tabuleiro->nPieces[WHITEROOK] = tabuleiro->nPieces[WHITEROOK] - 1;
			tabuleiro->wR[i].peca->eaten = 'Y';
			tabuleiro->materials = tabuleiro->materials - 1;
		}
	}
	/* Capture occurred; update halfturn */
	tabuleiro->halfturn = -1;
}

/*
	Update variables associated with a given turn
	param:
	   - tabuleiro: struct containing game info
*/
void updateTurn(chessBoard *tabuleiro)
{
	/* Update turn */
	switch(tabuleiro->turn)
	{
		case 'w':
			tabuleiro->turn = 'b';
		break;

		case 'b':
			tabuleiro->turn = 'w';
			/* Update turnCount */
			tabuleiro->turnCount = tabuleiro->turnCount + 1;
		break;

		default:
		break;
	}
	/* Update half-turn */
	tabuleiro->halfturn = tabuleiro->halfturn + 1;
}

/*
	Update board with pawn promotion
	params:
	   - tabuleiro: struct containing game info;
	   - peca: piece struct;
	   - row: new row;
	   - column: new column
*/
void updatePromotion(chessBoard *tabuleiro, piece *peca, int row, int column, char promotion)
{
	int i;
	/* Find piece */
	/* FIXME: always white queen because of test cases */
	for(i = 0; i < tabuleiro->nPieces[BLACKPAWN]; i++)
	{
		if(tabuleiro->bP[i].peca->eaten == 'N' && tabuleiro->bP[i].peca->row == peca->row && tabuleiro->bP[i].peca->column == peca->column)
		{
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEPAWN]; i++)
	{
		if(tabuleiro->wP[i].peca->eaten == 'N' && tabuleiro->wP[i].peca->row == peca->row && tabuleiro->wP[i].peca->column == peca->column)
		{
			tabuleiro->wP[i].peca->eaten = 'Y';
			tabuleiro->wQ = (queen*)realloc(tabuleiro->wQ, sizeof(queen)*(tabuleiro->nPieces[WHITEQUEEN]+1));
			tabuleiro->wQ[tabuleiro->nPieces[WHITEQUEEN]].init = (*initQueen);
			tabuleiro->wQ[tabuleiro->nPieces[WHITEQUEEN]].init(&tabuleiro->wQ[tabuleiro->nPieces[WHITEQUEEN]], row, column, 'Y', 'N');
			tabuleiro->nPieces[WHITEQUEEN] = tabuleiro->nPieces[WHITEQUEEN] + 1;	
		}	
	}	
}

char *getMove(int row, int column);
/*
	Check to see if a castling occurred, updating if any occurs
	params:
	   - tabuleiro: struct containing chessboard;
	   - peca: piece struct;
	   - row: new row;
	   - column: new column
*/
void updateCastling(chessBoard *tabuleiro, piece *peca, int row, int column)
{
	int i;
	/* Right side castling */
	if(tabuleiro->board[peca->row][peca->column] == 'R' && peca->column-2 >= 0 && tabuleiro->board[peca->row][peca->column-1] == '-' && 
		tabuleiro->board[peca->row][peca->column-2] == '-' && tabuleiro->board[peca->row][peca->column-3] == 'K' && peca->row == 0 && peca->column-2 == column)
	{
		/* Castling king */
		tabuleiro->board[peca->row][peca->column-1] = tabuleiro->board[peca->row][column-1];
		tabuleiro->board[peca->row][column-1] = '-';
		/* Updating piece struct */
		tabuleiro->wK[0].peca->row = peca->row;
		tabuleiro->wK[0].peca->column = peca->column-1;
	}
	/* Left side castling */
	else if(tabuleiro->board[peca->row][peca->column] == 'R' && peca->column+3 < 8 && tabuleiro->board[peca->row][peca->column+1] == '-' && 
		tabuleiro->board[peca->row][peca->column+2] == '-' && tabuleiro->board[peca->row][peca->column+3] == '-' && tabuleiro->board[peca->row][peca->column+4] == 'K' && peca->row == 0 && peca->column+3 == column)
	{
		/* Castling king */
		tabuleiro->board[peca->row][peca->column+2] = tabuleiro->board[peca->row][column+1];
		tabuleiro->board[peca->row][column+2] = '-';
		/* Updating piece struct */
		tabuleiro->wK[0].peca->row = peca->row;
		tabuleiro->wK[0].peca->column = peca->column+2;
	}
	/* Right side castling */
	else if(tabuleiro->board[peca->row][peca->column] == 'r' && peca->column-2 >= 0 && tabuleiro->board[peca->row][peca->column-1] == '-' && 
		tabuleiro->board[peca->row][peca->column-2] == '-' && tabuleiro->board[peca->row][peca->column-3] == 'k' && peca->row == 7 && peca->column-2 == column)
	{
		/* Castling king */
		tabuleiro->board[peca->row][peca->column-1] = tabuleiro->board[peca->row][column-1];
		tabuleiro->board[peca->row][column-1] = '-';
		/* Updating piece struct */
		tabuleiro->bK[0].peca->row = peca->row;
		tabuleiro->bK[0].peca->column = peca->column-1;
	}
	/* Left side castling */
	else if(tabuleiro->board[peca->row][peca->column] == 'r' && peca->column+3 < 8 && tabuleiro->board[peca->row][peca->column+1] == '-' && 
		tabuleiro->board[peca->row][peca->column+2] == '-' && tabuleiro->board[peca->row][peca->column+3] == '-' && tabuleiro->board[peca->row][peca->column+4] == 'k' && peca->row == 7 && peca->column+3 == column)
	{
		/* Castling king */
		tabuleiro->board[peca->row][peca->column+2] = tabuleiro->board[peca->row][column+1];
		tabuleiro->board[peca->row][column+2] = '-';
		/* Updating piece struct */
		tabuleiro->bK[0].peca->row = peca->row;
		tabuleiro->bK[0].peca->column = peca->column+2;
	}
	else if(tabuleiro->board[peca->row][peca->column] == 'K' && peca->column+2 < 8 && tabuleiro->board[peca->row][peca->column+1] == '-' &&
		tabuleiro->board[peca->row][peca->column+2] == '-' && tabuleiro->board[peca->row][peca->column+3] == 'R')
	{
		/* Castling rook */
		tabuleiro->board[peca->row][peca->column+1] = tabuleiro->board[peca->row][peca->column+3];
		tabuleiro->board[peca->row][peca->column+3] = '-';
		/* Updating rook struct */
		for(i = 0; i < tabuleiro->nPieces[WHITEROOK]; i++)
		{
			if(tabuleiro->wR[i].peca->row == peca->row && tabuleiro->wR[i].peca->column == peca->column+3)
			{
				tabuleiro->wR[i].peca->row = peca->row;
				tabuleiro->wR[i].peca->column = peca->column+3;
			}
		}
	}
}

/*
	Update a given piece position
	params:
	   - tabuleiro: struct containing chessboard;
	   - peca: piece struct;
	   - row: new row;
	   - column: new column;
	   - castling: consider castling in update
*/
void updatePiecePosition(chessBoard* tabuleiro, piece *peca, int row, int column, char promotion, int castling)
{
	char piece;
	/* Check castling, if any */
	if(castling)
	{
		updateCastling(tabuleiro, peca, row, column);
	}
	piece = tabuleiro->board[peca->row][peca->column];
	tabuleiro->board[peca->row][peca->column] = '-';
	if(tabuleiro->board[row][column] != '-')
	{
		updateCapture(tabuleiro, peca, row, column);
	}
	/* Check promotion, if any */
	if(piece == 'p' && row == 0)
	{
		updatePromotion(tabuleiro, peca, row, column, promotion);
	}
	else if(piece == 'P' && row == 7)
	{
		updatePromotion(tabuleiro, peca, row, column, promotion);
		piece = 'Q';
	}
	peca->row = row;
	peca->column = column;
	tabuleiro->board[row][column] = piece;
	// if(tabuleiro->turnCount == 5)
	// {
	// 	printf("row: %d\tcolumn: %d\n", row, column);
	// 	printf("print the board\n");
	// 	printMatrix(tabuleiro->board, 8, 8);
	// }
}

/*
	Chessboard struct initialization
	param:
	   - tabuleiro: struct containing chess board
*/
void initChess(chessBoard **tabuleiro)
{
	int i;
	(*tabuleiro) = (chessBoard*)malloc(sizeof(chessBoard));
	(*tabuleiro)->board = NULL;
	(*tabuleiro)->turn = 0;
	(*tabuleiro)->castling = NULL;
	(*tabuleiro)->enPassant = NULL;
	(*tabuleiro)->nPieces = (int*)calloc(12, sizeof(int));
	(*tabuleiro)->materials = 0;
	(*tabuleiro)->halfturn = 0;
	(*tabuleiro)->turnCount = 0;
	(*tabuleiro)->visitedBoard = (int**)calloc(8, sizeof(int*));
	for(i = 0; i < 8; i++) (*tabuleiro)->visitedBoard[i] = (int*)calloc(8, sizeof(int));
	(*tabuleiro)->bP = NULL;
	(*tabuleiro)->wP = NULL;
	(*tabuleiro)->bN = NULL;
	(*tabuleiro)->wN = NULL;
	(*tabuleiro)->bB = NULL;
	(*tabuleiro)->wB = NULL;
	(*tabuleiro)->bQ = NULL;
	(*tabuleiro)->wQ = NULL;
	(*tabuleiro)->bK = NULL;
	(*tabuleiro)->wK = NULL;
	(*tabuleiro)->bR = NULL;
	(*tabuleiro)->wR = NULL;
	(*tabuleiro)->h = (hash*)malloc(sizeof(hash));
	initHash(&(*tabuleiro)->h, 100);
	(*tabuleiro)->updatePosition = (*updatePiecePosition);
	(*tabuleiro)->updateTurn = (*updateTurn);
}

/*
	Free specific chessBoard struct variables
	param:
	   - tabuleiro: struct containing chess board
*/
void freeVariablesChess(chessBoard **tabuleiro)
{
	if((*tabuleiro)->castling) free((*tabuleiro)->castling);
	if((*tabuleiro)->enPassant) free((*tabuleiro)->enPassant);
}

/*
	Set specific chessBoard struct variables to NULL
	param:
	   - tabuleiro: struct containing chess board
*/
void nulifyVariablesChess(chessBoard **tabuleiro)
{
	(*tabuleiro)->castling = NULL;
	(*tabuleiro)->enPassant = NULL;
}

/*
	Free all chessBoard struct memory
	param:
	   - tabuleiro: struct containing chess board
*/
void freeChess(chessBoard **tabuleiro)
{
	int i;
	freeVariablesChess(tabuleiro);
	freeMatrix((*tabuleiro)->board, 8);
	freeMatrix2((*tabuleiro)->visitedBoard, 8);
	for(i = 0; i < (*tabuleiro)->nPieces[BLACKPAWN]; i++)
	{
		free((*tabuleiro)->bP[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[WHITEPAWN]; i++)
	{
		free((*tabuleiro)->wP[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[BLACKKNIGHT]; i++)
	{
		free((*tabuleiro)->bN[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[WHITEKNIGHT]; i++)
	{
		free((*tabuleiro)->wN[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[BLACKBISHOP]; i++)
	{
		free((*tabuleiro)->bB[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[WHITEBISHOP]; i++)
	{
		free((*tabuleiro)->wB[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[BLACKQUEEN]; i++)
	{
		free((*tabuleiro)->bQ[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[WHITEQUEEN]; i++)
	{
		free((*tabuleiro)->wQ[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[BLACKKING]; i++)
	{
		free((*tabuleiro)->bK[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[WHITEKING]; i++)
	{
		free((*tabuleiro)->wK[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[BLACKROOK]; i++)
	{
		free((*tabuleiro)->bR[i].peca);
	}
	for(i = 0; i < (*tabuleiro)->nPieces[WHITEROOK]; i++)
	{
		free((*tabuleiro)->wR[i].peca);
	}


	if((*tabuleiro)->bP) free((*tabuleiro)->bP);
	if((*tabuleiro)->wP) free((*tabuleiro)->wP);
	if((*tabuleiro)->bN) free((*tabuleiro)->bN);
	if((*tabuleiro)->wN) free((*tabuleiro)->wN);
	if((*tabuleiro)->bB) free((*tabuleiro)->bB);
	if((*tabuleiro)->wB) free((*tabuleiro)->wB);
	if((*tabuleiro)->bQ) free((*tabuleiro)->bQ);
	if((*tabuleiro)->wQ) free((*tabuleiro)->wQ);
	if((*tabuleiro)->bK) free((*tabuleiro)->bK);
	if((*tabuleiro)->wK) free((*tabuleiro)->wK);
	if((*tabuleiro)->bR) free((*tabuleiro)->bR);
	if((*tabuleiro)->wR) free((*tabuleiro)->wR);

	if((*tabuleiro)->nPieces) free((*tabuleiro)->nPieces);
	free((*tabuleiro));
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
	i = k->peca->row+2;
	j = k->peca->column;
	if(i < 8 && i >= 0 && (j+1) < 8 && (j-1) >= 0 && (board[i][j+1] == knightPiece || board[i][j-1] == knightPiece))
	{
		check = check + 1;
	}
	/* East */
	i = k->peca->row;
	j = k->peca->column+2;
	if((i+1) < 8 && (i-1) >= 0 && j < 8 && j >= 0 && (board[i+1][j] == knightPiece || board[i-1][j] == knightPiece))
	{
		check = check + 1;
	}
	/* South */
	i = k->peca->row-2;
	j = k->peca->column;
	if(i < 8 && i >= 0 && (j+1) < 8 && (j-1) >= 0 && (board[i][j+1] == knightPiece || board[i][j-1] == knightPiece))
	{
		check = check + 1;
	}
	/* West */
	i = k->peca->row;
	j = k->peca->column-2;
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
	switch(k->peca->isWhite)
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
	for(i = k->peca->row+1, j = k->peca->column; i < 8; i++)
	{
		/* Check enemy king */
		if(i == k->peca->row+1)
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
	for(i = k->peca->row, j = k->peca->column+1; j < 8; j++)
	{
		/* Check enemy king */
		if(j == k->peca->column+1)
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
	for(i = k->peca->row-1, j = k->peca->column; i >= 0; i--)
	{
		/* Check enemy king */
		if(i == k->peca->row-1)
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
	for(i = k->peca->row, j = k->peca->column-1; j >= 0; j--)
	{
		/* Check enemy king */
		if(j == k->peca->column-1)
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
	switch(k->peca->isWhite)
	{
		case 'Y':
			i = k->peca->row+1;
		break;

		case 'N':
			i = k->peca->row-1;
		break;

		default:
		break;
	}
	j = k->peca->column;
	if(i < 8 && i >= 0 && (j+1) < 8 && (j-1) >= 0 && (board[i][j+1] == pawnPiece || board[i][j-1] == pawnPiece))
	{
		check = check + 1;
	}

	/* Check for bishops or queens */
	/* Northeast */
	for(i = k->peca->row+1, j = k->peca->column+1; i < 8 && j < 8; i++, j++)
	{
		/* Check enemy king */
		if(i == k->peca->row+1 && j == k->peca->column+1)
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
	for(i = k->peca->row-1, j = k->peca->column+1; i >= 0 && j < 8; i--, j++)
	{
		/* Check enemy king */
		if(i == k->peca->row-1 && j == k->peca->column+1)
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
	for(i = k->peca->row-1, j = k->peca->column-1; i >= 0 && j >= 0; i--, j--)
	{
		/* Check enemy king */
		if(i == k->peca->row-1 && j == k->peca->column-1)
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
	for(i = k->peca->row+1, j = k->peca->column-1; i < 8 && j >= 0; i++, j--)
	{
		/* Check enemy king */
		if(i == k->peca->row+1 && j == k->peca->column-1)
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
	Check if position is either friendly or enemy
	params:
	   - tabuleiro: struct containing game info;
	   - i: row wise position to be checked;
	   - j: column wise position to be checked;
	   - isWhite: (1) if piece is white, (0) otherwise
	returns:
	   - (2) if friendly position, (1) if enemy
*/
int checkFriendOrFoe(chessBoard *tabuleiro, int i, int j, int isWhite)
{
	char pieces[12] = {'p', 'P', 'n', 'N', 'b', 'B', 'q', 'Q', 'k', 'K', 'r', 'R'};
	switch(isWhite)
	{
		case 1:
			if(tabuleiro->board[i][j] == pieces[BLACKPAWN] || tabuleiro->board[i][j] == pieces[BLACKKNIGHT] ||
				tabuleiro->board[i][j] == pieces[BLACKBISHOP] || tabuleiro->board[i][j] == pieces[BLACKQUEEN] ||
				tabuleiro->board[i][j] == pieces[BLACKKING] || tabuleiro->board[i][j] == pieces[BLACKROOK])
			{
				return 1;
			}
			else if(tabuleiro->board[i][j] == pieces[WHITEPAWN] || tabuleiro->board[i][j] == pieces[WHITEKNIGHT] ||
				tabuleiro->board[i][j] == pieces[WHITEBISHOP] || tabuleiro->board[i][j] == pieces[WHITEQUEEN] ||
				tabuleiro->board[i][j] == pieces[WHITEKING] || tabuleiro->board[i][j] == pieces[WHITEROOK])
			{
				return 2;
			}
		break;

		case 0:
			if(tabuleiro->board[i][j] == pieces[WHITEPAWN] || tabuleiro->board[i][j] == pieces[WHITEKNIGHT] ||
				tabuleiro->board[i][j] == pieces[WHITEBISHOP] || tabuleiro->board[i][j] == pieces[WHITEQUEEN] ||
				tabuleiro->board[i][j] == pieces[WHITEKING] || tabuleiro->board[i][j] == pieces[WHITEROOK])
			{
				return 1;
			}
			else if(tabuleiro->board[i][j] == pieces[BLACKPAWN] || tabuleiro->board[i][j] == pieces[BLACKKNIGHT] ||
				tabuleiro->board[i][j] == pieces[BLACKBISHOP] || tabuleiro->board[i][j] == pieces[BLACKQUEEN] ||
				tabuleiro->board[i][j] == pieces[BLACKKING] || tabuleiro->board[i][j] == pieces[BLACKROOK])
			{
				return 2;
			}
		break;

		default:
			printf("No way! You shouldn't be here.\n");
		break;
	}
	return -2;
}

/*
	Check given position in board
	params:
	   - peca: piece to be checked;
	   - tabuleiro: struct containing game info;
	   - i: row wise position to be checked;
	   - j: column wise position to be checked
	returns:
	   - (2) if friendly position, (1) if enemy position, (0) if empty space, (-1) if not a valid move
*/
int checkPosition(piece *peca, chessBoard *tabuleiro, int i, int j)
{
	if(peca->eaten == 'N' && i < 8 && i >= 0 && j < 8 && j >= 0 && tabuleiro->visitedBoard[i][j] == 0)
	{
		if(tabuleiro->board[i][j] == '-')
		{
			return 0;
		}
		else
		{
			switch(peca->isWhite)
			{
				case 'Y':
					return checkFriendOrFoe(tabuleiro, i, j, 1);
				break;

				case 'N':
					return checkFriendOrFoe(tabuleiro, i, j, 0);
				break;

				default:
					printf("Hey! You're not supposed to be here.\n");
				break;
			}
		}		
	}
	/* Else return -1 */
	return -1;
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
		(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*3);
		(*possibilities)[(*count)][0] = j+97;
		(*possibilities)[(*count)][1] = i+48 + 1;
		(*possibilities)[(*count)][2] = '\0';
		(*count) = (*count) + 1;
	}
	else
	{
		(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
		(*possibilities)[(*count)] = NULL;
		(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
		(*possibilities)[(*count)][0] = j+97;
		(*possibilities)[(*count)][1] = i+48 + 1;
		(*possibilities)[(*count)][2] = isAttack+48;
		(*possibilities)[(*count)][3] = '\0';
		(*count) = (*count) + 1;	
	}
	/*
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
	*/
}

/*
	Get value from a given piece
	params:
	   - piece: piece;
	   - move: (1) white move, (0) black move
	returns:
	   - value
*/
double getPositionValue(char piece, int move)
{
	switch(move)
	{
		case 1:
			if(piece == 'p') return 100.00;
			else if(piece == 'P') return 50.00;
			else if(piece == 'n' || piece == 'b') return 325.00;
			else if(piece == 'N' || piece == 'B') return 162.5;
			else if(piece == 'r') return 550.00;
			else if(piece == 'R') return 275.00;
			else if(piece == 'q') return 1000.00;
			else if(piece == 'Q') return 500.00;
			else if(piece == 'k') return 50000.00;
			else if(piece == 'K') return 25000.00;
		break;

		case 0:
			if(piece == 'P') return 100.00;
			else if(piece == 'p') return 50.00;
			else if(piece == 'N' || piece == 'B') return 325.00;
			else if(piece == 'n' || piece == 'b') return 162.5;
			else if(piece == 'R') return 550.00;
			else if(piece == 'r') return 275.00;
			else if(piece == 'Q') return 1000.00;
			else if(piece == 'q') return 500.00;
			else if(piece == 'K') return 50000.00;
			else if(piece == 'k') return 25000.00;
		break;

		default:
			printf("Square must have a specific value\n");
		break;
	}
	return -1.00;
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
int checkPromotion(pawn *p, chessBoard *tabuleiro, char ***possibilities, int *count, int move)
{
	int i, promotion = 0;
	char pieces[8] = {'n', 'b', 'r', 'q', 'N', 'B', 'R', 'Q'};
	switch(move)
	{
		/* Case 0: move forward */
		case 0:
			switch(p->peca->isWhite)
			{
				case 'Y':
					if(p->peca->row+1 == 7 && checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column) == 0)
					{
						for(i = 4; i < 8; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][2] = pieces[i];
							(*possibilities)[(*count)][3] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				case 'N':
					if(p->peca->row+1 == 2 && checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column) == 0)
					{
						for(i = 0; i < 4; i++)
						{
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][2] = pieces[i];
							(*possibilities)[(*count)][3] = '\0';
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
			switch(p->peca->isWhite)
			{
				case 'Y':
					if(p->peca->row+1 == 7 && checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column-1) > 0)
					{
						for(i = 4; i < 8; i++)
						{
							/*(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->peca->column-1)+97;
							(*possibilities)[(*count)][3] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;*/
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][2] = pieces[i];
							(*possibilities)[(*count)][3] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				case 'N':
					if(p->peca->row+1 == 2 && checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column-1) > 0)
					{
						for(i = 0; i < 4; i++)
						{
							/*(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->peca->column-1)+97;
							(*possibilities)[(*count)][3] = (p->peca->row+1)+48 - 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;*/
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][2] = pieces[i];
							(*possibilities)[(*count)][3] = '\0';
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
			switch(p->peca->isWhite)
			{
				case 'Y':
					if(p->peca->row+1 == 7 && checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column+1) > 0)
					{
						for(i = 4; i < 8; i++)
						{
							/*(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->peca->column)+97 + 1;
							(*possibilities)[(*count)][3] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;*/
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][2] = pieces[i];
							(*possibilities)[(*count)][3] = '\0';
							(*count) = (*count) + 1;
						}
						promotion = 1;
					}
				break;

				case 'N':
					if(p->peca->row+1 == 2 && checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column+1) > 0)
					{
						for(i = 0; i < 4; i++)
						{
							/*(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*6);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = 'x';
							(*possibilities)[(*count)][2] = (p->peca->column)+97 + 1;
							(*possibilities)[(*count)][3] = (p->peca->row+1)+48 - 1;
							(*possibilities)[(*count)][4] = pieces[i];
							(*possibilities)[(*count)][5] = '\0';
							(*count) = (*count) + 1;*/
							(*possibilities) = (char**)realloc((*possibilities), sizeof(char*)*((*count)+1));
							(*possibilities)[(*count)] = NULL;
							(*possibilities)[(*count)] = (char*)realloc((*possibilities)[(*count)], sizeof(char)*4);
							(*possibilities)[(*count)][0] = (p->peca->column)+97;
							(*possibilities)[(*count)][1] = (p->peca->row+1)+48 + 1;
							(*possibilities)[(*count)][2] = pieces[i];
							(*possibilities)[(*count)][3] = '\0';
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


char **checkMovesPawn(pawn *p, king *k, chessBoard *tabuleiro)
{
	int count = 0;
	char **possibilities = NULL;
	switch(p->peca->isWhite)
	{
		case 'Y':
			/* Check if forward is clear */
			if(checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column) == 0 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row+1, p->peca->column)))
			{
				if(!checkPromotion(p, tabuleiro, &possibilities, &count, 0))
				{
					writePossibility(&possibilities, &count, p->peca->row+1, p->peca->column, (p->peca->column)+97, 0);
				}
			}
			/* Check if pawn is at beggining - no need to check promotion */
			if(p->peca->row+1 == 2 && checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column) == 0 && 
				checkPosition(p->peca, tabuleiro, p->peca->row+2, p->peca->column) == 0 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row+2, p->peca->column)))
			{
				writePossibility(&possibilities, &count, p->peca->row+2, p->peca->column, (p->peca->column)+97, 0);
			}
			/* Check pawn attacks */
			/* Left attack */
			if(checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column-1) == 1 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row+1, p->peca->column-1)))
			{
				if(!checkPromotion(p, tabuleiro, &possibilities, &count, 1))
				{
					writePossibility(&possibilities, &count, p->peca->row+1, p->peca->column-1, (p->peca->column)+97, 0);
				}
			}
			/* Right attack */
			if(checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column+1) == 1 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row+1, p->peca->column+1)))
			{
				if(!checkPromotion(p, tabuleiro, &possibilities, &count, 2))
				{
					writePossibility(&possibilities, &count, p->peca->row+1, p->peca->column+1, (p->peca->column)+97, 0);
				}
			}
			/* En Passant - left */
			// if(p->peca->row+1 == 5 && p->peca->column-1 >= 0 && tabuleiro->board[p->peca->row][p->peca->column-1] == 'p' && 
			// 	!(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row, p->peca->column-1)))
			// {
			// 	possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
			// 	possibilities[count] = NULL;
			// 	possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
			// 	possibilities[count][0] = (p->peca->column)+97;
			// 	possibilities[count][1] = 'x';
			// 	possibilities[count][2] = (p->peca->column)+97 - 1;
			// 	possibilities[count][3] = (p->peca->row+1)+48 + 1;
			// 	possibilities[count][4] = 'e';
			// 	possibilities[count][5] = '.';
			// 	possibilities[count][6] = 'p';
			// 	possibilities[count][7] = '.';
			// 	possibilities[count][8] = '\0';
			// }
			/* En Passant - right */
			// if(p->peca->row+1 == 5 && p->peca->column+1 < 8 && tabuleiro->board[p->peca->row][p->peca->column+1] == 'p' &&
			// 	!(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row, p->peca->column+1)))
			// {
			// 	possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
			// 	possibilities[count] = NULL;
			// 	possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
			// 	possibilities[count][0] = (p->peca->column)+97;
			// 	possibilities[count][1] = 'x';
			// 	possibilities[count][2] = (p->peca->column)+97 + 1;
			// 	possibilities[count][3] = (p->peca->row+1)+48 + 1;
			// 	possibilities[count][4] = 'e';
			// 	possibilities[count][5] = '.';
			// 	possibilities[count][6] = 'p';
			// 	possibilities[count][7] = '.';
			// 	possibilities[count][8] = '\0';
			// }
		break;

		case 'N':
			/* Check if pawn is at beggining - no need to check promotion */
			if(p->peca->row+1 == 7 && checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column) == 0 && 
				checkPosition(p->peca, tabuleiro, p->peca->row-2, p->peca->column) == 0 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row-2, p->peca->column)))
			{
				writePossibility(&possibilities, &count, p->peca->row-2, p->peca->column, (p->peca->column)+97, 0);
			}
			/* Check if forward is clear */
			if(checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column) == 0 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row-1, p->peca->column)))
			{
				if(!checkPromotion(p, tabuleiro, &possibilities, &count, 0))
				{
					writePossibility(&possibilities, &count, p->peca->row-1, p->peca->column, (p->peca->column)+97, 0);
				}
			}
			/* Check pawn attacks */
			/* Left attack */
			if(checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column-1) == 1 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row-1, p->peca->column-1)))
			{
				if(!checkPromotion(p, tabuleiro, &possibilities, &count, 1))
				{
					writePossibility(&possibilities, &count, p->peca->row-1, p->peca->column-1, (p->peca->column)+97, 0);
				}
			}
			/* Right attack */
			if(checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column+1) == 1 && !(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row-1, p->peca->column+1)))
			{
				if(!checkPromotion(p, tabuleiro, &possibilities, &count, 2))
				{
					writePossibility(&possibilities, &count, p->peca->row-1, p->peca->column+1, (p->peca->column)+97, 0);
				}
			}
			/* En Passant - left */
			// if(p->peca->row+1 == 4 && p->peca->column-1 >= 0 && tabuleiro->board[p->peca->row][p->peca->column-1] == 'P' &&
			// 	!(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row, p->peca->column-1)))
			// {
			// 	possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
			// 	possibilities[count] = NULL;
			// 	possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
			// 	possibilities[count][0] = (p->peca->column)+97;
			// 	possibilities[count][1] = 'x';
			// 	possibilities[count][2] = (p->peca->column)+97 - 1;
			// 	possibilities[count][3] = (p->peca->row+1)+48 - 1;
			// 	possibilities[count][4] = 'e';
			// 	possibilities[count][5] = '.';
			// 	possibilities[count][6] = 'p';
			// 	possibilities[count][7] = '.';
			// 	possibilities[count][8] = '\0';
			// 	count = count + 1;
			// }
			/* En Passant - right */
			// if(p->peca->row+1 == 4 && p->peca->column+1 < 8 && tabuleiro->board[p->peca->row][p->peca->column+1] == 'P' &&
			// 	!(checkPossibleCheck(k, tabuleiro->board, p->peca->row, p->peca->column, p->peca->row, p->peca->column+1)))
			// {
			// 	possibilities = (char**)realloc(possibilities, sizeof(char*)*(count+1));
			// 	possibilities[count] = NULL;
			// 	possibilities[count] = (char*)realloc(possibilities[count], sizeof(char)*9);
			// 	possibilities[count][0] = (p->peca->column)+97;
			// 	possibilities[count][1] = 'x';
			// 	possibilities[count][2] = (p->peca->column)+97 + 1;
			// 	possibilities[count][3] = (p->peca->row+1)+48 - 1;
			// 	possibilities[count][4] = 'e';
			// 	possibilities[count][5] = '.';
			// 	possibilities[count][6] = 'p';
			// 	possibilities[count][7] = '.';
			// 	possibilities[count][8] = '\0';
			// 	count = count + 1;
			// }
		break;

		default:
			printf("Shouldn't be here!\n");
		break;
	}
	p->peca->possibleMoves = count;
	return possibilities;
}

double getScorePawn(chessBoard *tabuleiro, pawn *p, int turn)
{
	int pos;
	double value = 0.000000;
	switch(p->peca->isWhite)
	{
		case 'Y':
			/* Check pawn attacks */
			/* Left attack */
			pos = checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column-1);
			if(pos == 0)
			{
				value = value + 50;
				tabuleiro->visitedBoard[p->peca->row+1][p->peca->column-1] = 1;
			}
			else if(pos != -1)
			{
				value = value + getPositionValue(tabuleiro->board[p->peca->row+1][p->peca->column-1], turn);
				tabuleiro->visitedBoard[p->peca->row+1][p->peca->column-1] = 1;
			}
			/* Right attack */
			pos = checkPosition(p->peca, tabuleiro, p->peca->row+1, p->peca->column+1);
			if(pos == 0)
			{
				value = value + 50;
				tabuleiro->visitedBoard[p->peca->row+1][p->peca->column+1] = 1;
			}
			else if(pos != -1)
			{
				value = value + getPositionValue(tabuleiro->board[p->peca->row+1][p->peca->column+1], turn);
				tabuleiro->visitedBoard[p->peca->row+1][p->peca->column+1] = 1;
			}
			/* En Passant - left */
			// if(p->peca->row+1 == 5 && p->peca->column-1 >= 0 && tabuleiro->board[p->peca->row][p->peca->column-1] == 'p')
			// {
			// }
			/* En Passant - right */
			// if(p->peca->row+1 == 5 && p->peca->column+1 < 8 && tabuleiro->board[p->peca->row][p->peca->column+1] == 'p')
			// {
			// }
		break;

		case 'N':
			/* Check pawn attacks */
			/* Left attack */
			pos = checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column-1);
			if(pos == 0)
			{
				value = value + 50;
				tabuleiro->visitedBoard[p->peca->row-1][p->peca->column-1] = 1;
			}
			else if(pos != -1)
			{
				value = value + getPositionValue(tabuleiro->board[p->peca->row-1][p->peca->column-1], turn);
				tabuleiro->visitedBoard[p->peca->row-1][p->peca->column-1] = 1;
			}
			/* Right attack */
			pos = checkPosition(p->peca, tabuleiro, p->peca->row-1, p->peca->column+1);
			if(pos == 0)
			{
				value = value + 50;
				tabuleiro->visitedBoard[p->peca->row-1][p->peca->column+1] = 1;
			}
			else if(pos != -1)
			{
				value = value + getPositionValue(tabuleiro->board[p->peca->row-1][p->peca->column+1], turn);
				tabuleiro->visitedBoard[p->peca->row-1][p->peca->column+1] = 1;
			}
			/* En Passant - left */
			// if(p->peca->row+1 == 4 && p->peca->column-1 >= 0 && tabuleiro->board[p->peca->row][p->peca->column-1] == 'P')
			// {
			// }
			/* En Passant - right */
			// if(p->peca->row+1 == 4 && p->peca->column+1 < 8 && tabuleiro->board[p->peca->row][p->peca->column+1] == 'P')
			// {
			// }
		break;

		default:
			printf("Shouldn't be here!\n");
		break;
	}
	if(DEBUG_TEST) printf("value for %c pawn: %.6lf\n", p->peca->isWhite, value);
	return value;	
}

char **checkMovesKnight(knight *k, king *kng, chessBoard *tabuleiro)
{
	int count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	possibilities = NULL;
	/* Knight has 8 possible moves */
	piece = 'N';
	switch(k->peca->isWhite)
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
	if(k->peca->row-1 >= 0 && k->peca->column-2 >= 0 && tabuleiro->board[k->peca->row-1][k->peca->column-2] == '-' && 
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-1, k->peca->column-2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row-1, k->peca->column-2, piece, 0);
	}
	else if(k->peca->row-1 >= 0 && k->peca->column-2 >= 0 && 
		(tabuleiro->board[k->peca->row-1][k->peca->column-2] == pawnPiece || 
		tabuleiro->board[k->peca->row-1][k->peca->column-2] == knightPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column-2] == bishopPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column-2] == queenPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column-2] == rookPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column-2] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-1, k->peca->column-2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row-1, k->peca->column-2, piece, 1);
	}

	/* Check northwest first corner */
	if(k->peca->row+1 < 8 && k->peca->column-2 >= 0 && tabuleiro->board[k->peca->row+1][k->peca->column-2] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+1, k->peca->column-2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row+1, k->peca->column-2, piece, 0);
	}
	else if(k->peca->row+1 < 8 && k->peca->column-2 >= 0 && 
		(tabuleiro->board[k->peca->row+1][k->peca->column-2] == pawnPiece || 
		tabuleiro->board[k->peca->row+1][k->peca->column-2] == knightPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column-2] == bishopPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column-2] == queenPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column-2] == rookPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column-2] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+1, k->peca->column-2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row+1, k->peca->column-2, piece, 0);
	}

	/* Check southwest second corner */
	if(k->peca->row-2 >= 0 && k->peca->column-1 >= 0 && tabuleiro->board[k->peca->row-2][k->peca->column-1] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-2, k->peca->column-1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row-2, k->peca->column-1, piece, 0);
	}
	else if(k->peca->row-2 >= 0 && k->peca->column-1 >= 0 && 
		(tabuleiro->board[k->peca->row-2][k->peca->column-1] == pawnPiece || 
		tabuleiro->board[k->peca->row-2][k->peca->column-1] == knightPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column-1] == bishopPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column-1] == queenPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column-1] == rookPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column-1] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-2, k->peca->column-1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row-2, k->peca->column-1, piece, 0);
	}

	/* Check northwest second corner */
	if(k->peca->row+2 < 8 && k->peca->column-1 >= 0 && tabuleiro->board[k->peca->row+2][k->peca->column-1] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+2, k->peca->column-1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row+2, k->peca->column-1, piece, 0);
	}
	else if(k->peca->row+2 < 8 && k->peca->column-1 >= 0 && 
		(tabuleiro->board[k->peca->row+2][k->peca->column-1] == pawnPiece || 
		tabuleiro->board[k->peca->row+2][k->peca->column-1] == knightPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column-1] == bishopPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column-1] == queenPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column-1] == rookPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column-1] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+2, k->peca->column-1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row+2, k->peca->column-1, piece, 0);
	}

	/* Check southeast first corner */
	if(k->peca->row-2 >= 0 && k->peca->column+1 < 8 && tabuleiro->board[k->peca->row-2][k->peca->column+1] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-2, k->peca->column+1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row-2, k->peca->column+1, piece, 0);
	}
	else if(k->peca->row-2 >= 0 && k->peca->column+1 < 8 && 
		(tabuleiro->board[k->peca->row-2][k->peca->column+1] == pawnPiece || 
		tabuleiro->board[k->peca->row-2][k->peca->column+1] == knightPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column+1] == bishopPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column+1] == queenPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column+1] == rookPiece ||
		tabuleiro->board[k->peca->row-2][k->peca->column+1] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-2, k->peca->column+1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row-2, k->peca->column+1, piece, 0);
	}

	/* Check northeast first corner */
	if(k->peca->row+2 < 8 && k->peca->column+1 < 8 && tabuleiro->board[k->peca->row+2][k->peca->column+1] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+2, k->peca->column+1))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row+2, k->peca->column+1, piece, 0);
	}
	else if(k->peca->row+2 < 8 && k->peca->column+1 < 8 && 
		(tabuleiro->board[k->peca->row+2][k->peca->column+1] == pawnPiece || 
		tabuleiro->board[k->peca->row+2][k->peca->column+1] == knightPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column+1] == bishopPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column+1] == queenPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column+1] == rookPiece ||
		tabuleiro->board[k->peca->row+2][k->peca->column+1] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+2, k->peca->column+1))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row+2, k->peca->column+1, piece, 0);
	}

	/* Check southeast second corner */
	if(k->peca->row-1 >= 0 && k->peca->column+2 < 8 && tabuleiro->board[k->peca->row-1][k->peca->column+2] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-1, k->peca->column+2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row-1, k->peca->column+2, piece, 0);
	}
	else if(k->peca->row-1 >= 0 && k->peca->column+2 < 8 && 
		(tabuleiro->board[k->peca->row-1][k->peca->column+2] == pawnPiece || 
		tabuleiro->board[k->peca->row-1][k->peca->column+2] == knightPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column+2] == bishopPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column+2] == queenPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column+2] == rookPiece ||
		tabuleiro->board[k->peca->row-1][k->peca->column+2] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row-1, k->peca->column+2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row-1, k->peca->column+2, piece, 0);
	}

	/* Check northeast second corner */
	if(k->peca->row+1 < 8 && k->peca->column+2 < 8 && tabuleiro->board[k->peca->row+1][k->peca->column+2] == '-' &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+1, k->peca->column+2))) /* Empty position */
	{
		writePossibility(&possibilities, &count, k->peca->row+1, k->peca->column+2, piece, 0);
	}
	else if(k->peca->row+1 < 8 && k->peca->column+2 < 8 && 
		(tabuleiro->board[k->peca->row+1][k->peca->column+2] == pawnPiece || 
		tabuleiro->board[k->peca->row+1][k->peca->column+2] == knightPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column+2] == bishopPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column+2] == queenPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column+2] == rookPiece ||
		tabuleiro->board[k->peca->row+1][k->peca->column+2] == kingPiece) &&
		!(checkPossibleCheck(kng, tabuleiro->board, k->peca->row, k->peca->column, k->peca->row+1, k->peca->column+2))) /* Occupied position - attack */
	{
		writePossibility(&possibilities, &count, k->peca->row+1, k->peca->column+2, piece, 0);
	}
	k->peca->possibleMoves = count;
	return possibilities;
}

double getScoreKnight(chessBoard *tabuleiro, knight *n, int turn)
{
	int pos;
	double value = 0.000000;
	/* Check southwest first corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row-1, n->peca->column-2);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row-1][n->peca->column-2] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row-1][n->peca->column-2], turn);
		tabuleiro->visitedBoard[n->peca->row-1][n->peca->column-2] = 1;
	}

	/* Check northwest first corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row+1, n->peca->column-2);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row+1][n->peca->column-2] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row+1][n->peca->column-2], turn);
		tabuleiro->visitedBoard[n->peca->row+1][n->peca->column-2] = 1;
	}

	/* Check southwest second corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row-2, n->peca->column-1);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row-2][n->peca->column-1] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row-2][n->peca->column-1], turn);
		tabuleiro->visitedBoard[n->peca->row-2][n->peca->column-1] = 1;
	}

	/* Check northwest second corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row+2, n->peca->column-1);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row+2][n->peca->column-1] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row+2][n->peca->column-1], turn);
		tabuleiro->visitedBoard[n->peca->row+2][n->peca->column-1] = 1;
	}

	/* Check southeast first corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row-2, n->peca->column+1);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row-2][n->peca->column+1] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row-2][n->peca->column+1], turn);
		tabuleiro->visitedBoard[n->peca->row-2][n->peca->column+1] = 1;
	}

	/* Check northeast first corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row+2, n->peca->column+1);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row+2][n->peca->column+1] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row+2][n->peca->column+1], turn);
		tabuleiro->visitedBoard[n->peca->row+2][n->peca->column+1] = 1;
	}

	/* Check southeast second corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row-1, n->peca->column+2);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row-1][n->peca->column+2] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row-1][n->peca->column+2], turn);
		tabuleiro->visitedBoard[n->peca->row-1][n->peca->column+2] = 1;
	}

	/* Check northeast second corner */
	pos = checkPosition(n->peca, tabuleiro, n->peca->row+1, n->peca->column+2);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[n->peca->row+1][n->peca->column+2] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[n->peca->row+1][n->peca->column+2], turn);
		tabuleiro->visitedBoard[n->peca->row+1][n->peca->column+2] = 1;
	}
	if(DEBUG_TEST) printf("value for %c knight: %.6lf\n", n->peca->isWhite, value);
	return value;
}

char **checkMovesBishop(bishop *b, king *k, chessBoard *tabuleiro)
{
	int i, j, count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	possibilities = NULL;
	piece = 'B';
	switch(b->peca->isWhite)
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
	for(i = b->peca->row-1, j = b->peca->column-1; i >= 0 && j >= 0; i--, j--)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check upper left diagonal */
	for(i = b->peca->row+1, j = b->peca->column-1; i < 8 && j >= 0; i++, j--)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check lower right diagonal */
	for(i = b->peca->row-1, j = b->peca->column+1; i >= 0 && j < 8; i--, j++)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check upper right diagonal */
	for(i = b->peca->row+1, j = b->peca->column+1; i < 8 && j < 8; i++, j++)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, b->peca->row, b->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = 9;
		}
	}
	b->peca->possibleMoves = count;
	return possibilities;
}

double getScoreBishop(chessBoard *tabuleiro, bishop *b, int turn)
{
	int i, j, pos;
	double value = 0.000000;
	/* Check lower left diagonal */
	for(i = b->peca->row-1, j = b->peca->column-1; i >= 0 && j >= 0; i--, j--)
	{
		pos = checkPosition(b->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			/* Found a piece; stop */
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = -1;
	}

	/* Check upper left diagonal */
	for(i = b->peca->row+1, j = b->peca->column-1; i < 8 && j >= 0; i++, j--)
	{	
		pos = checkPosition(b->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			/* Found a piece; stop */
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = 9;
	}

	/* Check lower right diagonal */
	for(i = b->peca->row-1, j = b->peca->column+1; i >= 0 && j < 8; i--, j++)
	{
		pos = checkPosition(b->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			/* Found a piece; stop */
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = -1;
	}

	/* Check upper right diagonal */
	for(i = b->peca->row+1, j = b->peca->column+1; i < 8 && j < 8; i++, j++)
	{
		pos = checkPosition(b->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			/* Found a piece; stop */
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = 9;
	}
	if(DEBUG_TEST) printf("value for %c bishop: %.6lf\n", b->peca->isWhite, value);
	return value;
}

char **checkMovesQueen(queen *q, king *k, chessBoard *tabuleiro)
{
	int i, j, count = 0;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	possibilities = NULL;
	piece = 'Q';
	switch(q->peca->isWhite)
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
	for(i = q->peca->row-1, j = q->peca->column-1; i >= 0 && j >= 0; i--, j--)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check left row */
	for(i = q->peca->row, j = q->peca->column-1; j >= 0; j--)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			j = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			j = -1;
		}
	}

	/* Check upper left diagonal */
	for(i = q->peca->row+1, j = q->peca->column-1; i < 8 && j >= 0; i++, j--)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check up column */
	for(i = q->peca->row+1, j = q->peca->column; i < 8; i++)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check lower right diagonal */
	for(i = q->peca->row-1, j = q->peca->column+1; i >= 0 && j < 8; i--, j++)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = -1;
		}
	}

	/* Check right row */
	for(i = q->peca->row, j = q->peca->column+1; j < 8; j++)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			j = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			j = 9;
		}	
	}

	/* Check upper right diagonal */
	for(i = q->peca->row+1, j = q->peca->column+1; i < 8 && j < 8; i++, j++)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check down column */
	for(i = q->peca->row-1, j = q->peca->column; i >= 0; i--)
	{
		if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece)
			&& !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j)))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, q->peca->row, q->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = -1;
		}
	}
	q->peca->possibleMoves = count;
	return possibilities;
}

double getScoreQueen(chessBoard *tabuleiro, queen *q, int turn)
{
	int i, j, pos;
	double value = 0.000000;
	/* Check all possible positions, regardless of sorting; qsort will handle it later */
	/* Check lower left diagonal */
	for(i = q->peca->row-1, j = q->peca->column-1; i >= 0 && j >= 0; i--, j--)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = -1;
	}

	/* Check left row */
	for(i = q->peca->row, j = q->peca->column-1; j >= 0; j--)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			j = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') j = -1;
	}

	/* Check upper left diagonal */
	for(i = q->peca->row+1, j = q->peca->column-1; i < 8 && j >= 0; i++, j--)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = 9;
	}

	/* Check up column */
	for(i = q->peca->row+1, j = q->peca->column; i < 8; i++)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = 9;
	}

	/* Check lower right diagonal */
	for(i = q->peca->row-1, j = q->peca->column+1; i >= 0 && j < 8; i--, j++)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = -1;
	}

	/* Check right row */
	for(i = q->peca->row, j = q->peca->column+1; j < 8; j++)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			j = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') j = 9;
	}

	/* Check upper right diagonal */
	for(i = q->peca->row+1, j = q->peca->column+1; i < 8 && j < 8; i++, j++)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = 9;
	}

	/* Check down column */
	for(i = q->peca->row-1, j = q->peca->column; i >= 0; i--)
	{
		pos = checkPosition(q->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = -1;
	}
	if(DEBUG_TEST) printf("value for %c queen: %.6lf\n", q->peca->isWhite, value);
	return value;
}

char **checkMovesKing(king *k, chessBoard *tabuleiro)
{
	int i, j, l, m, count = 0, /*originalRow, originalColumn,*/ rank;
	char **possibilities, piece, kingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece, castlingPiece /*,friendlyKingPiece*/;
	chessBoard *simulation;
	possibilities = NULL;
	piece = 'K';
	switch(k->peca->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
			castlingPiece = 'R';
			// friendlyKingPiece = 'K';
			rank = 0;
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
			castlingPiece = 'r';
			// friendlyKingPiece = 'k';
			rank = 7;
		break;
	}
	/* Store original positions to avoid unecessary changing */
	// originalRow = k->peca->row;
	// originalColumn = k->peca->column;
	/* Check lower left diagonal */
	i = k->peca->row-1;
	j = k->peca->column-1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->row = i;
		k->peca->column = j;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row+1, k->peca->column+1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row+1, k->peca->column+1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		k->peca->row = i+1;
		k->peca->column = j+1;
	}

	/* Check left */
	i = k->peca->row;
	j = k->peca->column-1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->column = j;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row, k->peca->column+1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row, k->peca->column+1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->column = j+1;
	}

	/* Check upper left diagonal */
	i = k->peca->row+1;
	j = k->peca->column-1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->row = i;
		k->peca->column = j;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row-1, k->peca->column+1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row-1, k->peca->column+1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->row = i-1;
		k->peca->column = j+1;
	}

	/* Check up column */
	i = k->peca->row+1;
	j = k->peca->column;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->row = i;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row-1, k->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row-1, k->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->row = i-1;
	}

	/* Check lower right diagonal */
	i = k->peca->row-1;
	j = k->peca->column+1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->row = i;
		k->peca->column = j;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row+1, k->peca->column-1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row+1, k->peca->column-1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->row = i+1;
		k->peca->column = j-1;
	}

	/* Check right row */
	i = k->peca->row;
	j = k->peca->column+1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->column = j;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row, k->peca->column-1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row, k->peca->column-1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->column = j-1;
	}

	/* Check upper right row */		
	i = k->peca->row+1;
	j = k->peca->column+1;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->row = i;
		k->peca->column = j;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row-1, k->peca->column-1, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row-1, k->peca->column-1, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->row = i-1;
		k->peca->column = j-1;
	}

	/* Check down column */
	i = k->peca->row-1;
	j = k->peca->column;
	if(i >= 0 && i < 8 && j >= 0 && j < 8)
	{
		k->peca->row = i;
		if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, k->peca->row+1, k->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
		|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
		!(checkPossibleCheck(k, tabuleiro->board, k->peca->row+1, k->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			
		}
		k->peca->row = i+1;
	}
	chessBoardStructCopy(&simulation, tabuleiro);
	for(l = 0; l < simulation->nPieces[WHITEKING]; l++)
	{
		if(simulation->wK[l].peca->row == k->peca->row && simulation->wK[l].peca->column == k->peca->column)
		{
			/* Right castling */
			i = simulation->wK[l].peca->row;
			j = simulation->wK[l].peca->column+2;
			if(i < 8 && i >= 0 && j < 8 && j >= 0 && i == rank && simulation->board[i][j-1] == '-' 
				&& simulation->board[i][j] == '-' && simulation->board[i][j+1] == castlingPiece)
			{
				for(m = 0; m < tabuleiro->nPieces[WHITEROOK]; m++)
				{
					if(simulation->wR[m].peca->row == 0 && simulation->wR[m].peca->column == j+1)
					{
						simulation->updatePosition(simulation, simulation->wR[m].peca,
						simulation->wR[m].peca->row, simulation->wR[m].peca->column-2, 0, 0);
						simulation->updatePosition(simulation, simulation->wK[l].peca,
							simulation->wK[l].peca->row, simulation->wK[l].peca->column+2, 0, 0);
						/*if(!(checkPossibleCheck(&simulation->bK[0], simulation->board, simulation->bK[0].peca->row, simulation->bK[0].peca->column,
							simulation->bR[l].peca->row, simulation->bR[l].peca->column+1)))*/
						if(!checkCheck(&simulation->wK[l], simulation->board))
						{
							writePossibility(&possibilities, &count, i, j, piece, 0);
						}
					}
				}
			}
			/* Right castling */
			// freeChess(&simulation);
			// chessBoardStructCopy(&simulation, tabuleiro);
			// i = r->peca->row;
			// j = r->peca->column+3;
			// if(i < 8 && i >= 0 && j < 8 && j >= 0 && i == rank && simulation->bR[l].peca->column == 1 && tabuleiro->board[i][j-2] == '-' 
			// 	&& tabuleiro->board[i][j-1] == '-' && tabuleiro->board[i][j] == '-' && tabuleiro->board[i][j+1] == friendlyKingPiece)
			// {
			// 	simulation->updatePosition(simulation, simulation->bR[l].peca, i, j, 0, 0);
			// 	simulation->updatePosition(simulation, simulation->bK[0].peca, simulation->bR[l].peca->row, simulation->bR[l].peca->column-1, 0, 0);
			// 	/*if(!(checkPossibleCheck(&simulation->bK[0], tabuleiro->board, simulation->bK[0].peca->row, simulation->bK[0].peca->column, 
			// 		simulation->bR[l].peca->row, simulation->bR[l].peca->column-1)))*/
			// 	if(!checkCheck(&simulation->bK[0], simulation->board))
			// 	{
			// 		writePossibility(&possibilities, &count, i, j, piece, 1);
			// 	}
			// }
		}
	}
	freeChess(&simulation);
	/* Right Castling */
	/*if(k->peca->row == rank && k->peca->column+3 < 8 && tabuleiro->board[k->peca->row][k->peca->column+1] == '-' && 
		tabuleiro->board[k->peca->row][k->peca->column+2] == '-' && tabuleiro->board[k->peca->row][k->peca->column+3] == castlingPiece)
	{
		tabuleiro->board[k->peca->row][k->peca->column] = '-';
		tabuleiro->board[k->peca->row][k->peca->column+2] = friendlyKingPiece;
		k->peca->column = k->peca->column+2;
		if(k->peca->column < 8 && !(checkCheck(k, tabuleiro->board)))
		{
			writePossibility(&possibilities, &count, k->peca->row, (k->peca->column+2), piece, 1);
		}
		k->peca->column = k->peca->column-2;
		tabuleiro->board[k->peca->row][k->peca->column] = friendlyKingPiece;
		tabuleiro->board[k->peca->row][k->peca->column+2] = '-';
	}*/
	/* Left Castling */
	/*if(k->peca->row == rank && k->peca->column-4 >= 0 && tabuleiro->board[k->peca->row][k->peca->column-1] == '-' && 
		tabuleiro->board[k->peca->row][k->peca->column-2] == '-' && tabuleiro->board[k->peca->row][k->peca->column-3] == '-' &&
		tabuleiro->board[k->peca->row][k->peca->column-4] == castlingPiece)
	{
		tabuleiro->board[k->peca->row][k->peca->column] = '-';
		tabuleiro->board[k->peca->row][k->peca->column-2] = friendlyKingPiece;
		k->peca->column = k->peca->column-2;
		if(k->peca->column >= 0 && !(checkCheck(k, tabuleiro->board)))
		{
			writePossibility(&possibilities, &count, k->peca->row, (k->peca->column-2), piece, 2);
		}
		k->peca->column = k->peca->column+2;
		tabuleiro->board[k->peca->row][k->peca->column] = friendlyKingPiece;
		tabuleiro->board[k->peca->row][k->peca->column-2] = '-';
	}*/
	/* Restore original row and column positions */
	// k->peca->row = originalRow;
	// k->peca->column = originalColumn;

	k->peca->possibleMoves = count;
	return possibilities;
}

double getScoreKing(chessBoard *tabuleiro, king *k, int turn)
{
	int i, j, pos;
	double value = 0.000000;
	/* Check lower left diagonal */
	i = k->peca->row-1;
	j = k->peca->column-1;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check left */
	i = k->peca->row;
	j = k->peca->column-1;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check upper left diagonal */
	i = k->peca->row+1;
	j = k->peca->column-1;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check up column */
	i = k->peca->row+1;
	j = k->peca->column;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check lower right diagonal */
	i = k->peca->row-1;
	j = k->peca->column+1;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check right row */
	i = k->peca->row;
	j = k->peca->column+1;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check upper right row */		
	i = k->peca->row+1;
	j = k->peca->column+1;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Check down column */
	i = k->peca->row-1;
	j = k->peca->column;
	pos = checkPosition(k->peca, tabuleiro, i, j);
	if(pos == 0)
	{
		value = value + 50;
		tabuleiro->visitedBoard[i][j] = 1;
	}
	else if(pos != -1)
	{
		value = value + getPositionValue(tabuleiro->board[i][j], turn);
		tabuleiro->visitedBoard[i][j] = 1;
	}

	/* Right Castling */
	// if(k->peca->column+3 < 8 && k->peca->row+1 == 1 && board[k->peca->row][k->peca->column+1] == '-' && 
	// 	board[k->peca->row][k->peca->column+2] == '-' && board[k->peca->row][k->peca->column+3] == castlingPiece)
	// {
	// 	k->peca->column = k->peca->column+2;
	// 	if(k->peca->column < 8 && !(checkPossibleCheck(k, board, k->peca->row, k->peca->column-2, k->peca->row, k->peca->column)))
	// 	{
	// 		k->peca->column = k->peca->column-2;
	// 		writePossibility(&possibilities, &count, k->peca->row, (k->peca->column+2), piece, 0);
	// 	}
	// }
	// /* Left Castling */
	// if(k->peca->column-4 >= 0 && k->peca->row+1 == 1 && board[k->peca->row][k->peca->column-1] == '-' && 
	// 	board[k->peca->row][k->peca->column-2] == '-' && board[k->peca->row][k->peca->column-3] == '-' &&
	// 	board[k->peca->row][k->peca->column-4] == castlingPiece)
	// {
	// 	k->peca->column = k->peca->column-2;
	// 	if(k->peca->column >= 0 && !(checkPossibleCheck(k, board, k->peca->row, k->peca->column+2, k->peca->row, k->peca->column)))
	// 	{
	// 		k->peca->column = k->peca->column+2;
	// 		writePossibility(&possibilities, &count, k->peca->row, (k->peca->column-2), piece, 0);
	// 	}
	// }
	if(DEBUG_TEST) printf("value for %c king: %.6lf\n", k->peca->isWhite, value);
	return value;
}

char **checkMovesRook(rook *r, king *k, chessBoard *tabuleiro)
{
	int i, j, l, count = 0, rank;
	char **possibilities, piece, kingPiece, friendlyKingPiece, pawnPiece, knightPiece, bishopPiece, queenPiece, rookPiece;
	chessBoard *simulation = NULL;
	piece = 'R';
	switch(r->peca->isWhite)
	{
		case 'Y':
			kingPiece = 'k';
			pawnPiece = 'p';
			knightPiece = 'n';
			bishopPiece = 'b';
			queenPiece = 'q';
			rookPiece = 'r';
			rank = 0;
			friendlyKingPiece = 'K';
		break;

		case 'N':
			kingPiece = 'K';
			pawnPiece = 'P';
			knightPiece = 'N';
			bishopPiece = 'B';
			queenPiece = 'Q';
			rookPiece = 'R';
			rank = 7;
			friendlyKingPiece = 'k';
		break;
	}
	possibilities = NULL;
	/* Check all four possible positions, regardless of sorting; qsort will handle it later */
	/* Check left row */
	for(i = r->peca->row, j = r->peca->column-1; j >= 0; j--)
	{
		/*if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else */if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			j = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			j = -1;
		}
	}

	/* Check up column */
	for(i = r->peca->row+1, j = r->peca->column; i < 8; i++)
	{
		/*if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else */if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = 9;
		}
	}

	/* Check right row */
	for(i = r->peca->row, j = r->peca->column+1; j < 8; j++)
	{
		/*if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else */if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			j = 9;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			j = 9;
		}	
	}

	/* Check down column */
	for(i = r->peca->row-1, j = r->peca->column; i >= 0; i--)
	{
		/*if((tabuleiro->board[i][j] == '-' || tabuleiro->board[i][j] == kingPiece))
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);	
		}
		else */if(tabuleiro->board[i][j] == '-' && !(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Empty position */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
		}
		else if((tabuleiro->board[i][j] == pawnPiece || tabuleiro->board[i][j] == knightPiece || tabuleiro->board[i][j] == bishopPiece || tabuleiro->board[i][j] == queenPiece
			|| tabuleiro->board[i][j] == kingPiece || tabuleiro->board[i][j] == rookPiece) &&
			!(checkPossibleCheck(k, tabuleiro->board, r->peca->row, r->peca->column, i, j))) /* Occupied - attack */
		{
			writePossibility(&possibilities, &count, i, j, piece, 0);
			/* Since we found a piece, stop */
			i = -1;
		}
		else if((tabuleiro->board[i][j] == reverseCase(pawnPiece) || tabuleiro->board[i][j] == reverseCase(knightPiece) || 
			tabuleiro->board[i][j] == reverseCase(bishopPiece) || tabuleiro->board[i][j] == reverseCase(queenPiece) || 
			tabuleiro->board[i][j] == reverseCase(kingPiece) || tabuleiro->board[i][j] == reverseCase(rookPiece)))/* Friendly piece - stop */
		{
			i = -1;
		}
	}
	chessBoardStructCopy(&simulation, tabuleiro);
	for(l = 0; l < simulation->nPieces[BLACKROOK]; l++)
	{
		if(simulation->bR[l].peca->row == r->peca->row && simulation->bR[l].peca->column == r->peca->column)
		{
			/* Left castling */
			i = simulation->bR[l].peca->row;
			j = simulation->bR[l].peca->column-2;
			if(i < 8 && i >= 0 && j < 8 && j >= 0 && i == rank && simulation->bR[l].peca->column == 7 && simulation->board[i][j+1] == '-' 
				&& simulation->board[i][j] == '-' && simulation->board[i][j-1] == friendlyKingPiece)
			{
				simulation->updatePosition(simulation, simulation->bR[l].peca, i, j, 0, 0);
				simulation->updatePosition(simulation, simulation->bK[0].peca, simulation->bR[l].peca->row, simulation->bR[l].peca->column+1, 0, 0);
				/*if(!(checkPossibleCheck(&simulation->bK[0], simulation->board, simulation->bK[0].peca->row, simulation->bK[0].peca->column,
					simulation->bR[l].peca->row, simulation->bR[l].peca->column+1)))*/
				if(!checkCheck(&simulation->bK[0], simulation->board))
				{
					writePossibility(&possibilities, &count, i, j, piece, 1);
				}
			}
			/* Right castling */
			freeChess(&simulation);
			chessBoardStructCopy(&simulation, tabuleiro);
			i = r->peca->row;
			j = r->peca->column+3;
			if(i < 8 && i >= 0 && j < 8 && j >= 0 && i == rank && simulation->bR[l].peca->column == 1 && tabuleiro->board[i][j-2] == '-' 
				&& tabuleiro->board[i][j-1] == '-' && tabuleiro->board[i][j] == '-' && tabuleiro->board[i][j+1] == friendlyKingPiece)
			{
				simulation->updatePosition(simulation, simulation->bR[l].peca, i, j, 0, 0);
				simulation->updatePosition(simulation, simulation->bK[0].peca, simulation->bR[l].peca->row, simulation->bR[l].peca->column-1, 0, 0);
				/*if(!(checkPossibleCheck(&simulation->bK[0], tabuleiro->board, simulation->bK[0].peca->row, simulation->bK[0].peca->column, 
					simulation->bR[l].peca->row, simulation->bR[l].peca->column-1)))*/
				if(!checkCheck(&simulation->bK[0], simulation->board))
				{
					writePossibility(&possibilities, &count, i, j, piece, 1);
				}
			}
		}
	}
	freeChess(&simulation);
	chessBoardStructCopy(&simulation, tabuleiro);
	freeChess(&simulation);
	r->peca->possibleMoves = count;
	return possibilities;
}

double getScoreRook(chessBoard *tabuleiro, rook *r, int turn)
{
	int i, j, pos;
	double value = 0.000000;
	/* Check all four possible positions, regardless of sorting; qsort will handle it later */
	/* Check left row */
	for(i = r->peca->row, j = r->peca->column-1; j >= 0; j--)
	{
		pos = checkPosition(r->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			j = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') j = -1;
	}

	/* Check up column */
	for(i = r->peca->row+1, j = r->peca->column; i < 8; i++)
	{
		pos = checkPosition(r->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = 9;
	}

	/* Check right row */
	for(i = r->peca->row, j = r->peca->column+1; j < 8; j++)
	{
		pos = checkPosition(r->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			j = 9;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') j = 9;
	}

	/* Check down column */
	for(i = r->peca->row-1, j = r->peca->column; i >= 0; i--)
	{
		pos = checkPosition(r->peca, tabuleiro, i, j);
		if(pos == 0)
		{
			value = value + 50;
			tabuleiro->visitedBoard[i][j] = 1;
		}
		else if(pos != -1)
		{
			value = value + getPositionValue(tabuleiro->board[i][j], turn);
			tabuleiro->visitedBoard[i][j] = 1;
			i = -1;
		}
		else if(pos == -1 && tabuleiro->board[i][j] != '-') i = -1;
	}
	if(DEBUG_TEST) printf("value for %c rook: %.6lf\n", r->peca->isWhite, value);
	return value;
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
    	for(i = n-1; i >= 0; i--)
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

void printMatrix2(int **matrix, int n, int m)
{
    int i, j;
    if(m != -1)
    {
    	for(i = n-1; i >= 0; i--)
	    {
    		for(j = 0; j < m; j++)
	        {
	            printf("%d", matrix[i][j]);
	        }
	        printf("\n");
	    }
    }
}


/*
	Get move row
	param:
	   - move: move to be read
	returns:
	   - row
*/
int getRow(char *move)
{
	return (move[1]-49);
}

/*
	Get move column
	param:
	   - move: move to be read
	returns:
	   - column
*/
int getColumn(char *move)
{
	return (move[0]-97);
}

/*
	Get move promotion
	param:
	   - move: move to be read
	returns:
	   - piece to be promoted
*/
char getPromotion(char *move)
{
	if(strlen(move) == 3) return (move[2]);
	else return 0;
}

/*
	Get a move in algebraic notation
	param:
	   - row: row;
	   - column: column
	returns:
	   - algebraic notation movement
*/
char *getMove(int row, int column)
{
	char *movement = (char*)malloc(sizeof(char)*3);
	movement[0] = column+97;
	movement[1] = row+49;
	movement[2] = '\0';
	return movement;
}

/*
	Read moves' castling
	param:
	   - move: move to be read
	returns:
	   - (2) if left castling, (1) if right castling, (0) if no castling
*/
int getCastling(char *move)
{
	if(strlen(move) == 3)
	{
		if(move[strlen(move)-1] == '1')
		{
			return 1;
		}
		else if(move[strlen(move)-1] == '2')
		{
			return 2;
		}
	}
	return 0;
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
	Initialize chess board
	param:
	   - input: string containing board initialization
	returns:
	   - annotated board
*/
char **initializeBoard(char *input)
{
	int i, j, k, zeroCount;
	char **board;
	zeroCount = 0;
	board = (char**)malloc(sizeof(char*)*8);
	for(i = 0; i < 8; i++)
	{
		board[i] = (char*)malloc(sizeof(char)*8);
	}
	for(i = (8-1), k = 0; i >= 0 && k < (strlen(input)); i--)
	{
		for(j = 0; j < 8; j++)
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
	initialize piece structs
	param:
	   - tabuleiro: struct containing board
*/
void initializePieces(chessBoard **tabuleiro)
{
	int i, j;
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			switch((*tabuleiro)->board[i][j])
			{
				case 'p':
					(*tabuleiro)->bP = (pawn*)realloc((*tabuleiro)->bP, sizeof(pawn)*((*tabuleiro)->nPieces[BLACKPAWN]+1));
					(*tabuleiro)->bP[(*tabuleiro)->nPieces[BLACKPAWN]].init = (*initPawn);
					(*tabuleiro)->bP[(*tabuleiro)->nPieces[BLACKPAWN]].init(&(*tabuleiro)->bP[(*tabuleiro)->nPieces[BLACKPAWN]], i, j, 'N', 'N');
					(*tabuleiro)->nPieces[BLACKPAWN] = (*tabuleiro)->nPieces[BLACKPAWN] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'P':
					(*tabuleiro)->wP = (pawn*)realloc((*tabuleiro)->wP, sizeof(pawn)*((*tabuleiro)->nPieces[WHITEPAWN]+1));
					(*tabuleiro)->wP[(*tabuleiro)->nPieces[WHITEPAWN]].init = (*initPawn);
					(*tabuleiro)->wP[(*tabuleiro)->nPieces[WHITEPAWN]].init(&(*tabuleiro)->wP[(*tabuleiro)->nPieces[WHITEPAWN]], i, j, 'Y', 'N');
					(*tabuleiro)->nPieces[WHITEPAWN] = (*tabuleiro)->nPieces[WHITEPAWN] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'n':
					(*tabuleiro)->bN = (knight*)realloc((*tabuleiro)->bN, sizeof(knight)*((*tabuleiro)->nPieces[BLACKKNIGHT]+1));
					(*tabuleiro)->bN[(*tabuleiro)->nPieces[BLACKKNIGHT]].init = (*initKnight);
					(*tabuleiro)->bN[(*tabuleiro)->nPieces[BLACKKNIGHT]].init(&(*tabuleiro)->bN[(*tabuleiro)->nPieces[BLACKKNIGHT]], i, j, 'N', 'N');
					(*tabuleiro)->nPieces[BLACKKNIGHT] = (*tabuleiro)->nPieces[BLACKKNIGHT] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'N':
					(*tabuleiro)->wN = (knight*)realloc((*tabuleiro)->wN, sizeof(knight)*((*tabuleiro)->nPieces[WHITEKNIGHT]+1));
					(*tabuleiro)->wN[(*tabuleiro)->nPieces[WHITEKNIGHT]].init = (*initKnight);
					(*tabuleiro)->wN[(*tabuleiro)->nPieces[WHITEKNIGHT]].init(&(*tabuleiro)->wN[(*tabuleiro)->nPieces[WHITEKNIGHT]], i, j, 'Y', 'N');
					(*tabuleiro)->nPieces[WHITEKNIGHT] = (*tabuleiro)->nPieces[WHITEKNIGHT] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'b':
					(*tabuleiro)->bB = (bishop*)realloc((*tabuleiro)->bB, sizeof(bishop)*((*tabuleiro)->nPieces[BLACKBISHOP]+1));
					(*tabuleiro)->bB[(*tabuleiro)->nPieces[BLACKBISHOP]].init = (*initBishop);
					(*tabuleiro)->bB[(*tabuleiro)->nPieces[BLACKBISHOP]].init(&(*tabuleiro)->bB[(*tabuleiro)->nPieces[BLACKBISHOP]], i, j, 'N', 'N');
					(*tabuleiro)->nPieces[BLACKBISHOP] = (*tabuleiro)->nPieces[BLACKBISHOP] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'B':
					(*tabuleiro)->wB = (bishop*)realloc((*tabuleiro)->wB, sizeof(bishop)*((*tabuleiro)->nPieces[WHITEBISHOP]+1));
					(*tabuleiro)->wB[(*tabuleiro)->nPieces[WHITEBISHOP]].init = (*initBishop);
					(*tabuleiro)->wB[(*tabuleiro)->nPieces[WHITEBISHOP]].init(&(*tabuleiro)->wB[(*tabuleiro)->nPieces[WHITEBISHOP]], i, j, 'Y', 'N');
					(*tabuleiro)->nPieces[WHITEBISHOP] = (*tabuleiro)->nPieces[WHITEBISHOP] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'q':
					(*tabuleiro)->bQ = (queen*)realloc((*tabuleiro)->bQ, sizeof(queen)*((*tabuleiro)->nPieces[BLACKQUEEN]+1));
					(*tabuleiro)->bQ[(*tabuleiro)->nPieces[BLACKQUEEN]].init = (*initQueen);
					(*tabuleiro)->bQ[(*tabuleiro)->nPieces[BLACKQUEEN]].init(&(*tabuleiro)->bQ[(*tabuleiro)->nPieces[BLACKQUEEN]], i, j, 'N', 'N');
					(*tabuleiro)->nPieces[BLACKQUEEN] = (*tabuleiro)->nPieces[BLACKQUEEN] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'Q':
					(*tabuleiro)->wQ = (queen*)realloc((*tabuleiro)->wQ, sizeof(queen)*((*tabuleiro)->nPieces[WHITEQUEEN]+1));
					(*tabuleiro)->wQ[(*tabuleiro)->nPieces[WHITEQUEEN]].init = (*initQueen);
					(*tabuleiro)->wQ[(*tabuleiro)->nPieces[WHITEQUEEN]].init(&(*tabuleiro)->wQ[(*tabuleiro)->nPieces[WHITEQUEEN]], i, j, 'Y', 'N');
					(*tabuleiro)->nPieces[WHITEQUEEN] = (*tabuleiro)->nPieces[WHITEQUEEN] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'k':
					(*tabuleiro)->bK = (king*)realloc((*tabuleiro)->bK, sizeof(king)*((*tabuleiro)->nPieces[BLACKKING]+1));
					(*tabuleiro)->bK[(*tabuleiro)->nPieces[BLACKKING]].init = (*initKing);
					(*tabuleiro)->bK[(*tabuleiro)->nPieces[BLACKKING]].init(&(*tabuleiro)->bK[(*tabuleiro)->nPieces[BLACKKING]], i, j, 'N', 'N');
					(*tabuleiro)->nPieces[BLACKKING] = (*tabuleiro)->nPieces[BLACKKING] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'K':
					(*tabuleiro)->wK = (king*)realloc((*tabuleiro)->wK, sizeof(king)*((*tabuleiro)->nPieces[WHITEKING]+1));
					(*tabuleiro)->wK[(*tabuleiro)->nPieces[WHITEKING]].init = (*initKing);
					(*tabuleiro)->wK[(*tabuleiro)->nPieces[WHITEKING]].init(&(*tabuleiro)->wK[(*tabuleiro)->nPieces[WHITEKING]], i, j, 'Y', 'N');
					(*tabuleiro)->nPieces[WHITEKING] = (*tabuleiro)->nPieces[WHITEKING] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'r':
					(*tabuleiro)->bR = (rook*)realloc((*tabuleiro)->bR, sizeof(rook)*((*tabuleiro)->nPieces[BLACKROOK]+1));
					(*tabuleiro)->bR[(*tabuleiro)->nPieces[BLACKROOK]].init = (*initRook);
					(*tabuleiro)->bR[(*tabuleiro)->nPieces[BLACKROOK]].init(&(*tabuleiro)->bR[(*tabuleiro)->nPieces[BLACKROOK]], i, j, 'N', 'N');
					(*tabuleiro)->nPieces[BLACKROOK] = (*tabuleiro)->nPieces[BLACKROOK] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				case 'R':
					(*tabuleiro)->wR = (rook*)realloc((*tabuleiro)->wR, sizeof(rook)*((*tabuleiro)->nPieces[WHITEROOK]+1));
					(*tabuleiro)->wR[(*tabuleiro)->nPieces[WHITEROOK]].init = (*initRook);
					(*tabuleiro)->wR[(*tabuleiro)->nPieces[WHITEROOK]].init(&(*tabuleiro)->wR[(*tabuleiro)->nPieces[WHITEROOK]], i, j, 'Y', 'N');
					(*tabuleiro)->nPieces[WHITEROOK] = (*tabuleiro)->nPieces[WHITEROOK] + 1;
					(*tabuleiro)->materials = (*tabuleiro)->materials + 1;
				break;

				default: /* Not a piece; do nothing */
				break;
			}
		}
	}
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
	   - tabuleiro: struct containing game info;
	   - move1: original piece position;
	   - move2: move made from move1
*/
void printFEN(chessBoard *tabuleiro, char *move1, char *move2)
{
	int i, j, emptySpaces, count = 0;
	char *key = NULL;
	for(i = 7; i >= 0; i--)
	{
		for(j = 0; j < 8; j++)
		{
			emptySpaces = 0;
			/* Count number of empty spaces */
			while(j < 8 && tabuleiro->board[i][j] == '-')
			{
				emptySpaces = emptySpaces + 1;
				j = j + 1;
			}
			if(emptySpaces != 0)
			{
				key = (char*)realloc(key, sizeof(char)*(count+1));
				key[count] = (emptySpaces+48);
				count = count + 1;
				printf("%d", emptySpaces);
			}
			if(j < 8 && tabuleiro->board[i][j] != '-')
			{
				key = (char*)realloc(key, sizeof(char)*(count+1));
				key[count] = tabuleiro->board[i][j];
				count = count + 1;
				printf("%c", tabuleiro->board[i][j]);
			}
		}
		if(i != 0)
		{
			key = (char*)realloc(key, sizeof(char)*(count+1));
			key[count] = '/';
			count = count + 1;
			printf("/");
		}
	}
	/* Put in hash for threefold checking */
	key = (char*)realloc(key, sizeof(char)*(count+1));
	key[count] = '\0';
	count = count + 1;
	putHash(tabuleiro->h, key);
	printf(" ");
	printf("%c", tabuleiro->turn);
	printf(" ");
	/* Check possible castlings */
	checkCastling(tabuleiro->board);
	printf(" ");
	/* Check en Passant */
	if(!tabuleiro->enPassant)
	{
		if(tabuleiro->board[getRow(move2)][getColumn(move1)] == 'p' ||
			tabuleiro->board[getRow(move2)][getColumn(move1)] == 'P')
		{
			checkEnPassant(move1, move2, 1);
		}
		else
		{
			checkEnPassant(move1, move2, 0);
		}
	}
	else
	{
		printf("%s", tabuleiro->enPassant);
	}
	printf(" ");
	printf("%d", tabuleiro->halfturn);
	printf(" ");
	printf("%d\n", tabuleiro->turnCount);
	
}

/*
	Read player move
	params:
	   - move: original move;
	   - move1: initial piece position;
	   - move2: final piece position
	returns:
	   - (1) if move was read, (0) otherwise
*/
int readMove(char **move, char **initialPos, char **finalPos)
{
	int success = 0;
	(*move) = NULL;
	(*move) = read(stdin, '\n');
	/* Step 1: Break (*move) string to find piece */
	(*initialPos)  = NULL;
	(*finalPos)  = NULL;
	if(isalpha((*move)[0]))
	{
		/* Check promotion */
		if(strlen((*move)) == 5)
		{
			(*initialPos) = (char*)malloc(sizeof(char)*3);
			(*initialPos)[0] = (*move)[0];
			(*initialPos)[1] = (*move)[1];
			(*initialPos)[2] = '\0';
			(*finalPos) = (char*)malloc(sizeof(char)*4);
			(*finalPos)[0] = (*move)[2];
			(*finalPos)[1] = (*move)[3];
			(*finalPos)[2] = (*move)[4];
			(*finalPos)[3] = '\0';
		}
		else
		{
			(*initialPos) = (char*)malloc(sizeof(char)*3);
			(*initialPos)[0] = (*move)[0];
			(*initialPos)[1] = (*move)[1];
			(*initialPos)[2] = '\0';
			(*finalPos) = (char*)malloc(sizeof(char)*3);
			(*finalPos)[0] = (*move)[2];
			(*finalPos)[1] = (*move)[3];
			(*finalPos)[2] = '\0';
		}
		if(strcmp((*move), "") == 0) free((*move));
		success = 1;
	}
	return success;
}

/*
	Check if any type of draw might occur
	param:
	   - tabuleiro: struct containing game info
	returns:
	   - (1) if draw, (0) otherwise
*/
int checkDraws(chessBoard *tabuleiro)
{
	int i, finish = 0;
	/* Step 1: Check if half-turns aren't over 50 */
	if(tabuleiro->halfturn >= 50)
	{
		endGame("Empate -- Regra dos 50 Movimentos");
		finish = 1;
	}
	/* Step 2: Check lack of materials */
	if(tabuleiro->materials <= 3)
	{
		/* Check if it's king and bishop against king */
		if(tabuleiro->nPieces[BLACKBISHOP] == 1 && tabuleiro->nPieces[BLACKKING] == 1 && tabuleiro->nPieces[WHITEKING] == 1)
		{
			endGame("Empate -- Falta de Material");
			finish = 1;
		}
		else if(tabuleiro->nPieces[WHITEBISHOP] == 1 && tabuleiro->nPieces[WHITEKING] == 1 && tabuleiro->nPieces[BLACKKING] == 1)
		{
			endGame("Empate -- Falta de Material");
			finish = 1;
		}
		/* Check it it's king and knight against king */
		else if(tabuleiro->nPieces[BLACKKNIGHT] == 1 && tabuleiro->nPieces[BLACKKING] == 1 && tabuleiro->nPieces[WHITEKING] == 1)
		{
			endGame("Empate -- Falta de Material");
			finish = 1;
		}
		else if(tabuleiro->nPieces[WHITEKNIGHT] == 1 && tabuleiro->nPieces[WHITEKING] == 1 && tabuleiro->nPieces[BLACKKING]  == 1)
		{
			endGame("Empate -- Falta de Material");
			finish = 1;
		}
		/* Check if it's only king against king */
		else if(tabuleiro->nPieces[BLACKKING] == 1 && tabuleiro->nPieces[WHITEKING] == 1)
		{
			endGame("Empate -- Falta de Material");
			finish = 1;	
		}
	}
	/* Step 3: Check for threefold repetition */
	for(i = 0; i < tabuleiro->h->m; i++)
	{
		// printf("tabuleiro->h->hashNode[i].nElements: %d\n", tabuleiro->h->hashNode[i].nElements);
		// listPrint(tabuleiro->h->hashNode[i].keys);
		if(tabuleiro->h->hashNode[i].endGame == 1)
		{
			endGame("Empate -- Tripla Repeticao");
			finish = 1;	
		}
	}
	return finish;
}

/*
	Compare all possible moves with given move
	params:
	   - position: move made;
	   - possibilities: all possible moves;
	   - possibilitiesSize: possibilities matrix sizeof
	returns:
	   - (1) if is valid, (0) otherwise
*/
int isValid(char *position, char **possibilities, int possibilitiesSize)
{
	int i, valid = 0;
	for(i = 0; i < possibilitiesSize; i++)
	{
		if(strcmp(position, possibilities[i]) == 0)
		{
			valid = 1;
		}
	}
	return valid;
}

/*
	Check if a given move is valid
	params:
	   - tabuleiro: struct containing game info;
	   - position: starting board position;
	   - finalPosition: final board position
	returns:
	   - piece struct which was moved, NULL if invalid
*/
piece *checkValidMove(chessBoard *tabuleiro, char *position, char *finalPosition)
{
	int i, valid = 0;
	char **possibilities = NULL;
	piece *aux = NULL;
	for(i = 0; i < tabuleiro->nPieces[BLACKPAWN]; i++)
	{
		if(tabuleiro->bP[i].peca->eaten == 'N' && tabuleiro->bP[i].peca->row == getRow(position) && tabuleiro->bP[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->bP[i].checkMoves(&tabuleiro->bP[i], &tabuleiro->bK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->bP[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->bP[i].peca;
				valid = 1;
				tabuleiro->halfturn = -1;
			}
			freeMatrix(possibilities, tabuleiro->bP[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEPAWN]; i++)
	{
		if(tabuleiro->wP[i].peca->eaten == 'N' && tabuleiro->wP[i].peca->row == getRow(position) && tabuleiro->wP[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->wP[i].checkMoves(&tabuleiro->wP[i], &tabuleiro->wK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->wP[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->wP[i].peca;
				valid = 1;
				tabuleiro->halfturn = -1;
			}
			freeMatrix(possibilities, tabuleiro->wP[i].peca->possibleMoves);
		}	
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKNIGHT]; i++)
	{
		if(tabuleiro->bN[i].peca->eaten == 'N' && tabuleiro->bN[i].peca->row == getRow(position) && tabuleiro->bN[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->bN[i].checkMoves(&tabuleiro->bN[i], &tabuleiro->bK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->bN[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->bN[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->bN[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEKNIGHT]; i++)
	{
		if(tabuleiro->wN[i].peca->eaten == 'N' && tabuleiro->wN[i].peca->row == getRow(position) && tabuleiro->wN[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->wN[i].checkMoves(&tabuleiro->wN[i], &tabuleiro->wK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->wN[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->wN[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->wN[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKBISHOP]; i++)
	{
		if(tabuleiro->bB[i].peca->eaten == 'N' && tabuleiro->bB[i].peca->row == getRow(position) && tabuleiro->bB[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->bB[i].checkMoves(&tabuleiro->bB[i], &tabuleiro->bK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->bB[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->bB[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->bB[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEBISHOP]; i++)
	{
		if(tabuleiro->wB[i].peca->eaten == 'N' && tabuleiro->wB[i].peca->row == getRow(position) && tabuleiro->wB[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->wB[i].checkMoves(&tabuleiro->wB[i], &tabuleiro->wK[0], tabuleiro);	
			if(isValid(finalPosition, possibilities, tabuleiro->wB[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->wB[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->wB[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKQUEEN]; i++)
	{
		if(tabuleiro->bQ[i].peca->eaten == 'N' && tabuleiro->bQ[i].peca->row == getRow(position) && tabuleiro->bQ[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->bQ[i].checkMoves(&tabuleiro->bQ[i], &tabuleiro->bK[0], tabuleiro);	
			if(isValid(finalPosition, possibilities, tabuleiro->bQ[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->bQ[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->bQ[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEQUEEN]; i++)
	{
		if(tabuleiro->wQ[i].peca->eaten == 'N' && tabuleiro->wQ[i].peca->row == getRow(position) && tabuleiro->wQ[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->wQ[i].checkMoves(&tabuleiro->wQ[i], &tabuleiro->wK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->wQ[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->wQ[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->wQ[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKING]; i++)
	{
		if(tabuleiro->bK[i].peca->eaten == 'N' && tabuleiro->bK[i].peca->row == getRow(position) && tabuleiro->bK[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->bK[i].checkMoves(&tabuleiro->bK[i], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->bK[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->bK[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->bK[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEKING]; i++)
	{
		if(tabuleiro->wK[i].peca->eaten == 'N' && tabuleiro->wK[i].peca->row == getRow(position) && tabuleiro->wK[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->wK[i].checkMoves(&tabuleiro->wK[i], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->wK[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->wK[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->wK[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKROOK]; i++)
	{
		if(tabuleiro->bR[i].peca->eaten == 'N' && tabuleiro->bR[i].peca->row == getRow(position) && tabuleiro->bR[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->bR[i].checkMoves(&tabuleiro->bR[i], &tabuleiro->bK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->bR[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->bR[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->bR[i].peca->possibleMoves);
		}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEROOK]; i++)
	{
		if(tabuleiro->wR[i].peca->eaten == 'N' && tabuleiro->wR[i].peca->row == getRow(position) && tabuleiro->wR[i].peca->column == getColumn(position))
		{
			possibilities = tabuleiro->wR[i].checkMoves(&tabuleiro->wR[i], &tabuleiro->wK[0], tabuleiro);
			if(isValid(finalPosition, possibilities, tabuleiro->wR[i].peca->possibleMoves))
			{
				aux = (piece*)malloc(sizeof(piece));
				aux = tabuleiro->wR[i].peca;
				valid = 1;
			}
			freeMatrix(possibilities, tabuleiro->wR[i].peca->possibleMoves);
		}
	}
	if(valid)
	{
		return aux;
	}
	else
	{
		return NULL;
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
	int moves, i;
	moves = 0;
	(*moveSet) = (int**)calloc(3, sizeof(int*));
	for(i = 0; i < 3; i++)
	{
		(*moveSet)[i] = (int*)calloc(3, sizeof(int));
	}
	/* North */
	if(k->peca->row-1 >= 0 && board[k->peca->row-1][k->peca->column] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[0][1] = 1;
	}
	/* Northeast */
	if(k->peca->row-1 >= 0 && k->peca->column+1 < 8 && board[k->peca->row-1][k->peca->column+1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[1][1] = 1;
	}
	/* East */
	if(k->peca->column+1 < 8 && board[k->peca->row][k->peca->column+1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[1][2] = 1;
	}
	/* Southeast */
	if(k->peca->row+1 < 8 && k->peca->column+1 < 8 && board[k->peca->row+1][k->peca->column+1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[2][2] = 1;
	}
	/* South */
	if(k->peca->row+1 < 8 && board[k->peca->row+1][k->peca->column] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[2][1] = 1;
	}
	/* Southwest */
	if(k->peca->row+1 < 8 && k->peca->column-1 >= 0 && board[k->peca->row+1][k->peca->column-1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[2][0] = 1;
	}
	/* West */
	if(k->peca->column-1 >= 0 && board[k->peca->row][k->peca->column-1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[1][0] = 1;
	}
	/* Northwest */
	if(k->peca->row-1 >= 0 && k->peca->column-1 >= 0 && board[k->peca->row-1][k->peca->column-1] == '-')
	{
		moves = moves + 1;
		if((*moveSet)) (*moveSet)[0][0] = 1;
	}
	return moves;
}

/*
	Check if king can be saved
	params:
	   - tabuleiro: struct containing game info;
	   - k: king struct
	returns:
	   - (1) if no saving, (0) otherwise
*/
int cantBeSaved(chessBoard *tabuleiro, king *k)
{
	int i, j, saved = 1;
	char **possibilities;
	possibilities = NULL;
	switch(k->peca->isWhite)
	{
		case 'Y':
			/* Check all pieces for possible moves */
			for(i = 1; i < 12; i = i + 2)
			{
				if(i == WHITEPAWN)
				{
					for(j = 0; j < tabuleiro->nPieces[WHITEPAWN]; j++)
					{
						if(tabuleiro->wP[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->wP[j].checkMoves(&tabuleiro->wP[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = tabuleiro->nPieces[WHITEPAWN] + 1;
							}
						}
					}
				}
				else if(i == WHITEKNIGHT)
				{
					for(j = 0; j < tabuleiro->nPieces[WHITEKNIGHT]; j++)
					{
						if(tabuleiro->wN[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->wN[j].checkMoves(&tabuleiro->wN[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = tabuleiro->nPieces[WHITEKNIGHT] + 1;
							}
						}
					}
				}
				else if(i == WHITEBISHOP)
				{
					for(j = 0; j < tabuleiro->nPieces[WHITEBISHOP]; j++)
					{
						if(tabuleiro->wB[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->wB[j].checkMoves(&tabuleiro->wB[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = tabuleiro->nPieces[WHITEBISHOP] + 1;
							}
						}
					}
				}
				else if(i == WHITEQUEEN)
				{
					for(j = 0; j < tabuleiro->nPieces[WHITEQUEEN]; j++)
					{
						if(tabuleiro->wQ[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->wQ[j].checkMoves(&tabuleiro->wQ[j], k, tabuleiro);	
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + tabuleiro->nPieces[WHITEQUEEN] + 1;
							}
						}
					}
				}
				else if(i == WHITEKING)
				{
					for(j = 0; j < tabuleiro->nPieces[WHITEKING]; j++)
					{
						if(tabuleiro->wK[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->wK[j].checkMoves(&tabuleiro->wK[j], tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + tabuleiro->nPieces[WHITEKING] + 1;
							}
						}
					}
				}
				else if(i == WHITEROOK)
				{
					for(j = 0; j < tabuleiro->nPieces[WHITEROOK]; j++)
					{
						if(tabuleiro->wR[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->wR[j].checkMoves(&tabuleiro->wR[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + tabuleiro->nPieces[WHITEROOK] + 1;
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
					for(j = 0; j < tabuleiro->nPieces[BLACKPAWN]; j++)
					{
						if(tabuleiro->bP[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->bP[j].checkMoves(&tabuleiro->bP[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = tabuleiro->nPieces[BLACKPAWN] + 1;
							}
						}
					}
				}
				else if(i == BLACKKNIGHT)
				{
					for(j = 0; j < tabuleiro->nPieces[BLACKKNIGHT]; j++)
					{
						if(tabuleiro->bN[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->bN[j].checkMoves(&tabuleiro->bN[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = tabuleiro->nPieces[BLACKKNIGHT] + 1;
							}
						}
					}
				}
				else if(i == BLACKBISHOP)
				{
					for(j = 0; j < tabuleiro->nPieces[BLACKBISHOP]; j++)
					{
						if(tabuleiro->bB[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->bB[j].checkMoves(&tabuleiro->bB[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = tabuleiro->nPieces[BLACKBISHOP] + 1;
							}
						}
					}
				}
				else if(i == BLACKQUEEN)
				{
					for(j = 0; j < tabuleiro->nPieces[BLACKQUEEN]; j++)
					{
						if(tabuleiro->bQ[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->bQ[j].checkMoves(&tabuleiro->bQ[j], k, tabuleiro);	
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + tabuleiro->nPieces[BLACKQUEEN] + 1;
							}
						}
					}
				}
				else if(i == BLACKKING)
				{
					for(j = 0; j < tabuleiro->nPieces[BLACKKING]; j++)
					{
						if(tabuleiro->bK[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->bK[j].checkMoves(&tabuleiro->bK[j], tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + tabuleiro->nPieces[BLACKKING] + 1;
							}
						}
					}
				}
				else if(i == BLACKROOK)
				{
					for(j = 0; j < tabuleiro->nPieces[BLACKROOK]; j++)
					{
						if(tabuleiro->bR[j].peca->eaten == 'N')
						{
							possibilities = tabuleiro->bR[j].checkMoves(&tabuleiro->bR[j], k, tabuleiro);
							if(possibilities != NULL)
							{
								saved = 0;
								i = 13;
								j = j + tabuleiro->nPieces[BLACKROOK] + 1;
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
	Make a copy between pawn structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void pawnStructCopy(pawn *destination, pawn *source)
{
	if(!(destination))
	{
		(destination) = (pawn*)malloc(sizeof(pawn)*1);
	}
	(destination)->init = source->init;
	(destination)->init((destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

/*
	Make a copy between knight structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void knightStructCopy(knight *destination, knight *source)
{
	if(!(destination))
	{
		(destination) = (knight*)malloc(sizeof(knight)*1);
	}
	(destination)->init = source->init;
	(destination)->init((destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

/*
	Make a copy between bishop structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void bishopStructCopy(bishop *destination, bishop *source)
{
	if(!(destination))
	{
		(destination) = (bishop*)malloc(sizeof(bishop)*1);
	}
	(destination)->init = source->init;
	(destination)->init((destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

/*
	Make a copy between queen structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void queenStructCopy(queen *destination, queen *source)
{
	if(!(destination))
	{
		(destination) = (queen*)malloc(sizeof(queen)*1);
	}
	(destination)->init = source->init;
	(destination)->init((destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

/*
	Make a copy between king structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void kingStructCopy(king **destination, king *source)
{
	if(!(*destination))
	{
		(*destination) = (king*)malloc(sizeof(king)*1);
	}
	(*destination)->init = source->init;
	(*destination)->init((*destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

void kingStructCopy2(king *destination, king *source)
{
	if(!(destination))
	{
		(destination) = (king*)malloc(sizeof(king)*1);
	}
	(destination)->init = source->init;
	(destination)->init((destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

/*
	Make a copy between rook structs
	params:
	   - destination: destination struct for copying
	   - source: source struct for copying
*/
void rookStructCopy(rook *destination, rook *source)
{
	if(!(destination))
	{
		(destination) = (rook*)malloc(sizeof(rook)*1);
	}
	(destination)->init = source->init;
	(destination)->init((destination), source->peca->row, source->peca->column, source->peca->isWhite, source->peca->eaten);
}

/*
	Make a copy between chessBoard structs
	params:
	   - destination: destination struct for copying;
	   - source: source struct for copying
*/
void chessBoardStructCopy(chessBoard **destination, chessBoard *source)
{
	int i, j;
	initChess(&(*destination));
	/* Make a copy from structs */
	(*destination)->turn = source->turn;
	if(source->castling)
	{
	 	(*destination)->castling = (char*)malloc(sizeof(char)*(strlen(source->castling)+1));
		strcpy((*destination)->castling, source->castling);
	}
	if(source->enPassant)
	{
		(*destination)->enPassant = (char*)malloc(sizeof(char)*(strlen(source->enPassant)+1));
		strcpy((*destination)->enPassant, source->enPassant);
	}
	for(i = 0; i < 12; i++)
	{
		(*destination)->nPieces[i] = source->nPieces[i];
	}
	(*destination)->materials = source->materials;
	(*destination)->halfturn = source->halfturn;
	(*destination)->turnCount = source->turnCount;
	(*destination)->board = (char**)malloc(sizeof(char*)*8);
	(*destination)->visitedBoard = (int**)malloc(sizeof(int*)*8);
	for(i = 0; i < 8; i++)
	{
		(*destination)->board[i] = (char*)malloc(sizeof(char)*8);
		(*destination)->visitedBoard[i] = (int*)malloc(sizeof(int)*8);
		for(j = 0; j < 8; j++)
		{
			(*destination)->board[i][j] = source->board[i][j];
			(*destination)->visitedBoard[i][j] = source->visitedBoard[i][j];
		}
	}
	(*destination)->bP = (pawn*)malloc(sizeof(pawn)*(*destination)->nPieces[BLACKPAWN]);
	for(i = 0; i < (*destination)->nPieces[BLACKPAWN]; i++)
	{
		pawnStructCopy(&((*destination)->bP[i]), &source->bP[i]);
	}
	(*destination)->wP = (pawn*)malloc(sizeof(pawn)*(*destination)->nPieces[WHITEPAWN]);
	for(i = 0; i < (*destination)->nPieces[WHITEPAWN]; i++)
	{
		pawnStructCopy(&(*destination)->wP[i], &source->wP[i]);
	}
	(*destination)->bN = (knight*)malloc(sizeof(knight)*(*destination)->nPieces[BLACKKNIGHT]);
	for(i = 0; i < (*destination)->nPieces[BLACKKNIGHT]; i++)
	{
		knightStructCopy(&(*destination)->bN[i], &source->bN[i]);
	}
	(*destination)->wN = (knight*)malloc(sizeof(knight)*(*destination)->nPieces[WHITEKNIGHT]);
	for(i = 0; i < (*destination)->nPieces[WHITEKNIGHT]; i++)
	{
		knightStructCopy(&(*destination)->wN[i], &source->wN[i]);
	}
	(*destination)->bB = (bishop*)malloc(sizeof(bishop)*(*destination)->nPieces[BLACKBISHOP]);
	for(i = 0; i < (*destination)->nPieces[BLACKBISHOP]; i++)
	{
		bishopStructCopy(&(*destination)->bB[i], &source->bB[i]);
	}
	(*destination)->wB = (bishop*)malloc(sizeof(bishop)*(*destination)->nPieces[WHITEBISHOP]);
	for(i = 0; i < (*destination)->nPieces[WHITEBISHOP]; i++)
	{
		bishopStructCopy(&(*destination)->wB[i], &source->wB[i]);
	}
	(*destination)->bQ = (queen*)malloc(sizeof(queen)*(*destination)->nPieces[BLACKQUEEN]);
	for(i = 0; i < (*destination)->nPieces[BLACKQUEEN]; i++)
	{
		queenStructCopy(&(*destination)->bQ[i], &source->bQ[i]);
	}
	(*destination)->wQ = (queen*)malloc(sizeof(queen)*(*destination)->nPieces[WHITEQUEEN]);
	for(i = 0; i < (*destination)->nPieces[WHITEQUEEN]; i++)
	{
		queenStructCopy(&(*destination)->wQ[i], &source->wQ[i]);
	}
	(*destination)->bK = (king*)malloc(sizeof(king)*(*destination)->nPieces[BLACKKING]);
	for(i = 0; i < (*destination)->nPieces[BLACKKING]; i++)
	{
		kingStructCopy2(&(*destination)->bK[i], &source->bK[i]);
	}
	(*destination)->wK = (king*)malloc(sizeof(king)*(*destination)->nPieces[WHITEKING]);
	for(i = 0; i < (*destination)->nPieces[WHITEKING]; i++)
	{
		kingStructCopy2(&(*destination)->wK[i], &source->wK[i]);
	}
	(*destination)->bR = (rook*)malloc(sizeof(rook)*(*destination)->nPieces[BLACKROOK]);
	for(i = 0; i < (*destination)->nPieces[BLACKROOK]; i++)
	{
		rookStructCopy(&(*destination)->bR[i], &source->bR[i]);
	}
	(*destination)->wR = (rook*)malloc(sizeof(rook)*(*destination)->nPieces[WHITEROOK]);
	for(i = 0; i < (*destination)->nPieces[WHITEROOK]; i++)
	{
		rookStructCopy(&(*destination)->wR[i], &source->wR[i]);
	}
	(*destination)->h = NULL;
	(*destination)->updatePosition = source->updatePosition;
	(*destination)->updateTurn = source->updateTurn;
}

/*
	Check to see if either checkmate or stalemate occurred
	param:
	   - tabuleiro: struct containing game info
	returns:
	   - (1) if end game, (0) otherwise
*/
int checkmateStalemate(chessBoard *tabuleiro)
{
	int i, j, k, check = 0, finish = 0, moves = 0;
	int **moveSet = NULL;
	char isWhite = 'Y';
	king *aux;
	chessBoard *simulation = NULL;
	aux = NULL;
	switch(tabuleiro->turn)
	{
		case 'w':
			kingStructCopy(&aux, tabuleiro->wK);
		break;

		case 'b':
			isWhite = 'N';
			kingStructCopy(&aux, tabuleiro->bK);
		break;

		default:
		break;
	}
	moves = checkNumberOfMoves(aux, tabuleiro->board, &moveSet);
	for(i = 0; i < moves; i++)
	{
		for(j = 0; j < 3; j++)
		{
			for(k = 0; k < 3; k++)
			{
				switch(tabuleiro->turn)
				{
					case 'w':
						kingStructCopy(&aux, tabuleiro->wK);
					break;

					case 'b':
						isWhite = 'N';
						kingStructCopy(&aux, tabuleiro->bK);
					break;

					default:
					break;
				}
				if(moveSet[j][k] != 0)
				{
					chessBoardStructCopy(&simulation, tabuleiro);
					simulation->updatePosition(simulation, aux->peca, aux->peca->row+(j-1), aux->peca->column+(k-1), 0, 0);
					check = check + checkCheck(aux, simulation->board);
					// check = check + checkPossibleCheck(aux, simulation->board, aux->peca->row, aux->peca->column, aux->peca->row, aux->peca->column);
					freeChess(&simulation);
					simulation = NULL;
				}
			}
		}
	}
	if(i != 0 && check >= i)
	{
		if(checkCheck(aux, tabuleiro->board) && cantBeSaved(tabuleiro, aux))
		{
			if(isWhite == 'Y')
			{
				endGame("Xeque-mate -- Vitoria: PRETO");
				finish = 1;
			}
			else
			{
				endGame("Xeque-mate -- Vitoria: BRANCO");
				finish = 1;
			}
		}
		/* Check stalemate */
		else if(!checkCheck(aux, tabuleiro->board) && cantBeSaved(tabuleiro, aux))
		{
			endGame("Empate -- Afogamento");
			finish = 1;
		}
	}
	free(aux);
	return finish;
}

/*
	Check either win or draw conditions
	param:
	   - tabuleiro: struct containing game info
	returns:
	   - (1) if end game, (0) otherwise
*/
int checkConditions(chessBoard *tabuleiro)
{
	int finish = 0;

	/* Step 1: First, win conditions: either check or checkmate */
	finish = checkmateStalemate(tabuleiro);

	/* Step 2: Check drawing conditions */
	if(!finish) finish = checkDraws(tabuleiro);
	return finish;
}

/*
	Reset visitedBoard int matrix
	param:
	   - tabuleiro: struct containing game info
*/
void resetVisitedBoard(chessBoard *tabuleiro)
{
	int i, j;
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			tabuleiro->visitedBoard[i][j] = 0;
		}
	}
}

/*
	Get value of a given move from board
	param:
	   - tabuleiro: struct containing game info
	returns:
	   - move value
*/
double getValue(chessBoard *tabuleiro)
{
	int i;
	double value = 0.000000, denValue = 0.000000;
	for(i = 0; i < tabuleiro->nPieces[BLACKPAWN]; i++)
	{
		if(tabuleiro->bP[i].peca->eaten == 'N'){ value = value + tabuleiro->bP[i].getScore(tabuleiro, &tabuleiro->bP[i], 0);}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKNIGHT]; i++)
	{
		if(tabuleiro->bN[i].peca->eaten == 'N'){ value = value + tabuleiro->bN[i].getScore(tabuleiro, &tabuleiro->bN[i], 0);}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKBISHOP]; i++)
	{
		if(tabuleiro->bB[i].peca->eaten == 'N'){ value = value + tabuleiro->bB[i].getScore(tabuleiro, &tabuleiro->bB[i], 0);}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKROOK]; i++)
	{
		if(tabuleiro->bR[i].peca->eaten == 'N'){ value = value + tabuleiro->bR[i].getScore(tabuleiro, &tabuleiro->bR[i], 0);}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKQUEEN]; i++)
	{
		if(tabuleiro->bQ[i].peca->eaten == 'N'){ value = value + tabuleiro->bQ[i].getScore(tabuleiro, &tabuleiro->bQ[i], 0);}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKING]; i++)
	{
		if(tabuleiro->bK[i].peca->eaten == 'N'){ value = value + tabuleiro->bK[i].getScore(tabuleiro, &tabuleiro->bK[i], 0);}
	}
	resetVisitedBoard(tabuleiro);
	for(i = 0; i < tabuleiro->nPieces[WHITEPAWN]; i++)
	{
		if(tabuleiro->wP[i].peca->eaten == 'N'){ denValue = denValue + tabuleiro->wP[i].getScore(tabuleiro, &tabuleiro->wP[i], 1);}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEKNIGHT]; i++)
	{
		if(tabuleiro->wN[i].peca->eaten == 'N'){ denValue = denValue + tabuleiro->wN[i].getScore(tabuleiro, &tabuleiro->wN[i], 1);}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEBISHOP]; i++)
	{
		if(tabuleiro->wB[i].peca->eaten == 'N'){ denValue = denValue + tabuleiro->wB[i].getScore(tabuleiro, &tabuleiro->wB[i], 1);}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEROOK]; i++)
	{
		if(tabuleiro->wR[i].peca->eaten == 'N'){ denValue = denValue + tabuleiro->wR[i].getScore(tabuleiro, &tabuleiro->wR[i], 1);}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEQUEEN]; i++)
	{
		if(tabuleiro->wQ[i].peca->eaten == 'N'){ denValue = denValue + tabuleiro->wQ[i].getScore(tabuleiro, &tabuleiro->wQ[i], 1);}
	}
	for(i = 0; i < tabuleiro->nPieces[WHITEKING]; i++)
	{
		if(tabuleiro->wK[i].peca->eaten == 'N'){ denValue = denValue + tabuleiro->wK[i].getScore(tabuleiro, &tabuleiro->wK[i], 1);}
	}
	resetVisitedBoard(tabuleiro);
	if(DEBUG_TEST) printf("value: %.6lf\tdenValue: %.6lf\n", value, denValue);
	return (value/(denValue+1));
}

/*
	Print a list of all possible moves
	params:
	   - tabuleiro: struct containing game info;
	   - rank: possibleMove struct;
	   - size: rank size
*/
void printPossibleMoves(chessBoard *tabuleiro, possibleMove *rank, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		printf("rank[%d].peca: %c\trank[%d].value: %.6lf\n", i, tabuleiro->board[rank[i].peca->row][rank[i].peca->column], i, rank[i].value);
	}
}

/*
	AI to make move
	param:
	   - tabuleiro: struct containing game info
	returns:
	   - move information
*/
possibleMove *AIMove(chessBoard *tabuleiro)
{
	/* Proof of correctness: AI will always be black pieces */
	int i, j, count = 0, row, column;
	char **possibilities = NULL;
	possibleMove *rank = NULL;
	chessBoard *simulation = NULL;
	/* Check all possible black moves */
	for(i = 0; i < tabuleiro->nPieces[BLACKPAWN]; i++)
	{
		if(tabuleiro->bP[i].peca->eaten == 'N')
		{
			if(tabuleiro->turnCount == 5)
			{
				printf("gdb\n");
			}
			possibilities = tabuleiro->bP[i].checkMoves(&tabuleiro->bP[i], &tabuleiro->bK[0], tabuleiro);
			qsort(possibilities, tabuleiro->bP[i].peca->possibleMoves, sizeof(char*), cmp2);
			for(j = 0; j < tabuleiro->bP[i].peca->possibleMoves; j++)
			{
				if(DEBUG_TEST) printf("pawn move: %s\n", possibilities[j]);
				rank = (possibleMove*)realloc(rank, sizeof(possibleMove)*(count+1));
				initPossibleMove(&rank[count], tabuleiro->bP[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]));
				row = tabuleiro->bP[i].peca->row;
				column = tabuleiro->bP[i].peca->column;
				chessBoardStructCopy(&simulation, tabuleiro);
				simulation->updatePosition(simulation, simulation->bP[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]), 0, 0);
				if(EAT) tabuleiro->bP[i].peca->eaten = 'Y';
				rank[count].value = getValue(simulation);
				if(EAT) tabuleiro->bP[i].peca->eaten = 'N';
				freeChess(&simulation);
				simulation = NULL;
				if(DEBUG_TEST) printf("rank[count].value = %.6lf\n", rank[count].value);
				count = count + 1;
			}
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKNIGHT]; i++)
	{
		if(tabuleiro->bN[i].peca->eaten == 'N')
		{
			possibilities = tabuleiro->bN[i].checkMoves(&tabuleiro->bN[i], &tabuleiro->bK[0], tabuleiro);
			qsort(possibilities, tabuleiro->bN[i].peca->possibleMoves, sizeof(char*), cmp2);
			for(j = 0; j < tabuleiro->bN[i].peca->possibleMoves; j++)
			{
				if(DEBUG_TEST) printf("knight move: %s\n", possibilities[j]);
				rank = (possibleMove*)realloc(rank, sizeof(possibleMove)*(count+1));
				initPossibleMove(&rank[count], tabuleiro->bN[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]));
				row = tabuleiro->bN[i].peca->row;
				column = tabuleiro->bN[i].peca->column;
				chessBoardStructCopy(&simulation, tabuleiro);
				simulation->updatePosition(simulation, simulation->bN[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]), 0, 0);
				if(EAT) simulation->bN[i].peca->eaten = 'Y';
				rank[count].value = getValue(simulation);
				if(EAT) simulation->bN[i].peca->eaten = 'N';
				freeChess(&simulation);
				simulation = NULL;
				if(DEBUG_TEST) printf("rank[count].value = %.6lf\n", rank[count].value);
				count = count + 1;
			}
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKBISHOP]; i++)
	{
		if(tabuleiro->bB[i].peca->eaten == 'N')
		{
			possibilities = tabuleiro->bB[i].checkMoves(&tabuleiro->bB[i], &tabuleiro->bK[0], tabuleiro);
			qsort(possibilities, tabuleiro->bB[i].peca->possibleMoves, sizeof(char*), cmp2);
			for(j = 0; j < tabuleiro->bB[i].peca->possibleMoves; j++)
			{
				if(DEBUG_TEST) printf("bishop move: %s\n", possibilities[j]);
				rank = (possibleMove*)realloc(rank, sizeof(possibleMove)*(count+1));
				initPossibleMove(&rank[count], tabuleiro->bB[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]));
				row = tabuleiro->bB[i].peca->row;
				column = tabuleiro->bB[i].peca->column;
				chessBoardStructCopy(&simulation, tabuleiro);
				simulation->updatePosition(simulation, simulation->bB[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]), 0, 0);
				if(EAT) simulation->bB[i].peca->eaten = 'Y';
				rank[count].value = getValue(simulation);
				if(EAT) simulation->bB[i].peca->eaten = 'N';
				freeChess(&simulation);
				simulation = NULL;
				if(DEBUG_TEST) printf("rank[count].value = %.6lf\n", rank[count].value);
				count = count + 1;
			}
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKQUEEN]; i++)
	{
		if(tabuleiro->bQ[i].peca->eaten == 'N')
		{
			possibilities = tabuleiro->bQ[i].checkMoves(&tabuleiro->bQ[i], &tabuleiro->bK[0], tabuleiro);	
			qsort(possibilities, tabuleiro->bQ[i].peca->possibleMoves, sizeof(char*), cmp2);
			for(j = 0; j < tabuleiro->bQ[i].peca->possibleMoves; j++)
			{
				if(DEBUG_TEST) printf("queen move: %s\n", possibilities[j]);
				rank = (possibleMove*)realloc(rank, sizeof(possibleMove)*(count+1));
				initPossibleMove(&rank[count], tabuleiro->bQ[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]));
				row = tabuleiro->bQ[i].peca->row;
				column = tabuleiro->bQ[i].peca->column;
				chessBoardStructCopy(&simulation, tabuleiro);
				simulation->updatePosition(simulation, simulation->bQ[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]), 0, 0);
				if(EAT) simulation->bQ[i].peca->eaten = 'Y';
				rank[count].value = getValue(simulation);
				if(EAT) simulation->bQ[i].peca->eaten = 'N';
				freeChess(&simulation);
				simulation = NULL;
				if(DEBUG_TEST) printf("rank[count].value = %.6lf\n", rank[count].value);
				count = count + 1;
			}
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKKING]; i++)
	{
		if(tabuleiro->bK[i].peca->eaten == 'N')
		{
			possibilities = tabuleiro->bK[i].checkMoves(&tabuleiro->bK[i], tabuleiro);
			qsort(possibilities, tabuleiro->bK[i].peca->possibleMoves, sizeof(char*), cmp2);
			for(j = 0; j < tabuleiro->bK[i].peca->possibleMoves; j++)
			{
				if(DEBUG_TEST) printf("king move: %s\n", possibilities[j]);
				rank = (possibleMove*)realloc(rank, sizeof(possibleMove)*(count+1));
				initPossibleMove(&rank[count], tabuleiro->bK[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]));
				row = tabuleiro->bK[i].peca->row;
				column = tabuleiro->bK[i].peca->column;
				chessBoardStructCopy(&simulation, tabuleiro);
				simulation->updatePosition(simulation, simulation->bK[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]), 0, getCastling(possibilities[j]));
				if(EAT) simulation->bK[i].peca->eaten = 'Y';
				rank[count].value = getValue(simulation);
				if(EAT) simulation->bK[i].peca->eaten = 'N';
				freeChess(&simulation);
				simulation = NULL;
				if(DEBUG_TEST) printf("rank[count].value = %.6lf\n", rank[count].value);
				count = count + 1;
			}
		}
	}
	for(i = 0; i < tabuleiro->nPieces[BLACKROOK]; i++)
	{
		if(tabuleiro->bR[i].peca->eaten == 'N')
		{
			possibilities = tabuleiro->bR[i].checkMoves(&tabuleiro->bR[i], &tabuleiro->bK[0], tabuleiro);
			qsort(possibilities, tabuleiro->bR[i].peca->possibleMoves, sizeof(char*), cmp2);
			for(j = 0; j < tabuleiro->bR[i].peca->possibleMoves; j++)
			{
				if(DEBUG_TEST) printf("rook move: %s\n", possibilities[j]);
				rank = (possibleMove*)realloc(rank, sizeof(possibleMove)*(count+1));
				initPossibleMove(&rank[count], tabuleiro->bR[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]));
				rank[count].castling = getCastling(possibilities[j]);
				row = tabuleiro->bR[i].peca->row;
				column = tabuleiro->bR[i].peca->column;
				chessBoardStructCopy(&simulation, tabuleiro);
				simulation->updatePosition(simulation, simulation->bR[i].peca, getRow(possibilities[j]), getColumn(possibilities[j]), 0, getCastling(possibilities[j]));
				if(EAT) simulation->bR[i].peca->eaten = 'Y';
				rank[count].value = getValue(simulation);
				if(EAT) simulation->bR[i].peca->eaten = 'N';
				freeChess(&simulation);
				simulation = NULL;
				if(DEBUG_TEST) printf("rank[count].value = %.6lf\n", rank[count].value);
				count = count + 1;
			}
		}
	}
	/* Sort values to obtain maximum value */
	qsort(rank, count, sizeof(possibleMove), cmp);
	if(DEBUG_TEST) printPossibleMoves(tabuleiro, rank, count);

	/* First value contains next move; update board and execute move */
	if(count != 0)
	{
		/* Update halfturn */
		if(tabuleiro->board[rank[0].peca->row][rank[0].peca->column] == 'p' ||
			tabuleiro->board[rank[0].row][rank[0].column] != '-')
		{
			tabuleiro->halfturn = -1;
		}
		row = rank[0].peca->row;
		column = rank[0].peca->column;
		// printf("before: peca->row: %d\tpeca->column: %d\n", rank[0].peca->row, rank[0].peca->column);
		tabuleiro->updatePosition(tabuleiro, rank[0].peca, rank[0].row, rank[0].column, 0, rank[0].castling);
		// printf("after: peca->row: %d\tpeca->column: %d\n", rank[0].peca->row, rank[0].peca->column);
		rank[0].row = row;
		rank[0].column = column;
	}
	return rank;
}

int main(void)
{
	int finish = 0;
	char *input, *move1, *move2, *move;
	piece *valid;
	chessBoard *tabuleiro;
	possibleMove *movement;

	/* Step 1: read board */
	input = read(stdin, ' ');
	initChess(&tabuleiro);
	tabuleiro->board = initializeBoard(input);
	initializePieces(&tabuleiro);

	/* Step 2: read turn */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	tabuleiro->turn = input[0];

	/* Step 3: read castling */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	tabuleiro->castling = (char*)malloc(sizeof(char)*2);
	if(tabuleiro->turn == 'w')
	{
		tabuleiro->castling[0] = input[0];
		tabuleiro->castling[1] = input[1];
	}
	else
	{
		tabuleiro->castling[0] = input[2];
		tabuleiro->castling[1] = input[3];	
	}

	/* Step 4: read en passant */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	tabuleiro->enPassant = (char*)malloc(sizeof(char)*3);
	strcpy(tabuleiro->enPassant, input);

	/* Step 5: read half-turns */
	free(input);
	input = NULL;
	input = read(stdin, ' ');
	tabuleiro->halfturn = toNum(input);

	/* Step 6: read turn */
	free(input);
	input = NULL;
	input = read(stdin, '\n');
	tabuleiro->turnCount = toNum(input);
	move1 = NULL;
	move2 = NULL;

	printFEN(tabuleiro, move1, move2);

	/* Step 7: check possible draws */
	finish = checkConditions(tabuleiro);

	/* Step 8: keep reading until game ends */
	while(!finish)
	{
		movement = NULL;
		// freeVariablesChess(&tabuleiro);
		nulifyVariablesChess(&tabuleiro);
		/* Step 8.1: Check if move is valid */
		if(tabuleiro->turn == 'w')
		{
			if(readMove(&move, &move1, &move2))
			{
				valid = checkValidMove(tabuleiro, move1, move2);
				if(DEBUG_TEST) printf("move1: %s\tmove2: %s\n", move1, move2);
				if(valid)
				{
					if(DEBUG_TEST) printf("Movimento valido. Testando...\n");
					tabuleiro->updatePosition(tabuleiro, valid, getRow(move2), getColumn(move2), getPromotion(move2), 1);
					/* Step 8.?: Update turn */
					tabuleiro->updateTurn(tabuleiro);
					printFEN(tabuleiro, move1, move2); // "b"
					/* Step 8.?: Check if move caused any check */
					finish = checkConditions(tabuleiro);
					if(!finish)
					{
						/* Step 8.?: AI move */
						movement = AIMove(tabuleiro);
						/* Step 8.?: Update turn */
						tabuleiro->updateTurn(tabuleiro);
						printFEN(tabuleiro, getMove(movement->row, movement->column), getMove(movement->peca->row, movement->peca->column)); // "w"
						finish = checkConditions(tabuleiro);
					}

				}
				else
				{
					printf("Movimento invalido. Tente novamente\n");
				}
				/* Step 8.?: check possible draws */
				// finish = checkDraws(tabuleiro);
			}
			else
			{
				finish = 1;
			}
		}
		else
		{
			/* Step 8.?: AI move */
			movement = AIMove(tabuleiro);
			/* Step 8.?: Update turn */
			tabuleiro->updateTurn(tabuleiro);
			finish = checkConditions(tabuleiro);
			printFEN(tabuleiro, getMove(movement->row, movement->column), getMove(movement->peca->row, movement->peca->column)); // "w"
		}
		freeMovement(&movement);
	}

	/* Step ???: free variables */
	free(input);
	free(move);
	free(move1);
	free(move2);
	freeChess(&tabuleiro);
	return 0;
}