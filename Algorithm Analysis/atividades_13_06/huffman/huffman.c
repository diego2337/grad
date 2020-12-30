/*
	Program that reads either a .txt or .huff extension file and executes huffman compression or decompression, respectively.
	Author: Diego S. Cintra
	USP Number: 10094043
	Date: 18/05/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

enum
{
	FIRST,
	SECOND
};

typedef struct _freqSymbol{
	int frequency;
	char symbol, *path;
}freqSymbol;

freqSymbol *findFqBySymbol(freqSymbol *fq, int count, char symbol)
{
	int i;
	for(i = 0; i < count; i++)
	{
		if(fq[i].symbol == symbol)
		{
			return &fq[i];
		}
	}
	return NULL;
}

/*void initFQ(freqSymbol *fq)
{
	(fq)->frequency = 0;
	(fq)->symbol = NULL;
}*/

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
	if((*(freqSymbol*)a).frequency < (*(freqSymbol*)b).frequency) 
	{
		return -1;
	}
	else if((*(freqSymbol*)a).frequency == (*(freqSymbol*)b).frequency) 
	{
		if((*(freqSymbol*)a).symbol < (*(freqSymbol*)b).symbol)
		{
			return -1;
		}
		else if((*(freqSymbol*)a).symbol == (*(freqSymbol*)b).symbol)
		{
			return 0;
		}
		else if((*(freqSymbol*)a).symbol > (*(freqSymbol*)b).symbol)
		{
			return 1;
		}
	}
	return 1;
}

/*
	Struct for node
*/
typedef struct _node{
	int frequency;
	char *symbol, *bits;
	struct _node *left, *right;
}node;

void initNode(node **n, int frequency, char *symbol, char *bits)
{
	if(!n)
	{
		(*n) = (node*)calloc(1, sizeof(node));
	}
	(*n)->frequency = frequency;
	(*n)->symbol = (char*)calloc((strlen(symbol)+1), sizeof(char));
	strcpy((*n)->symbol, symbol);
	if(bits)
	{
		(*n)->bits = (char*)calloc((strlen(bits)+1), sizeof(char));
	}
	else
	{
		(*n)->bits = (char*)calloc(2, sizeof(char));
		(*n)->bits[0] = ' ';
		(*n)->bits[1] = '\0';
	}
	(*n)->left = NULL;
	(*n)->right = NULL;
}

void freeNode(node *n)
{
	if(n && (n)->symbol) free((n)->symbol);
	if(n && (n)->bits) free((n)->bits);
	if(n && (n)->left) free((n)->left);
	if(n && (n)->right) free((n)->right);
	free((n));
}

/*
	Struct for binary tree
*/
typedef struct _binaryTree{
	node *root;
	int n;
}binaryTree;

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
	if((*(binaryTree*)a).root->frequency < (*(binaryTree*)b).root->frequency)
	{
		return -1;
	}
	else if((*(binaryTree*)a).root->frequency == (*(binaryTree*)b).root->frequency)
	{
		if((*(binaryTree*)a).root->symbol && (*(binaryTree*)b).root->symbol && strcmp( (*(binaryTree*)a).root->symbol, (*(binaryTree*)b).root->symbol ) < 0)
		{
			return -1;
		}
		else if((*(binaryTree*)a).root->symbol && (*(binaryTree*)b).root->symbol && strcmp( (*(binaryTree*)a).root->symbol, (*(binaryTree*)b).root->symbol ) == 0)
		{
			return 0;
		}
		return 1;
	}
	return 1;
}

/*
	Compare function for qsort
	params:
	   - a: a element to be compared
	   - b: b element to be compared
	returns:
	   - result of comparison between a and b
*/
int cmp3(const void *a, const void *b)
{
	if((*(freqSymbol*)a).symbol < (*(freqSymbol*)b).symbol)
	{
		return -1;
	}
	else if((*(freqSymbol*)a).symbol == (*(freqSymbol*)b).symbol)
	{
		return 0;
	}
	return 1;
}

void createBinaryTree(binaryTree **tree)
{
	if(!(*tree))
	{
		(*tree) = (binaryTree*)malloc(sizeof(binaryTree));
	}
	(*tree)->root = NULL;
	(*tree)->n = 0;
}

void insertBinaryTree(node **root, int frequency, char *symbol, char *bits, int count)
{
	if(!(*root))
	{
		(*root) = (node*)malloc(sizeof(node));
		(*root)->frequency = frequency;
		(*root)->symbol = (char*)malloc(sizeof(char)*(strlen(symbol)+1));
		strcpy((*root)->symbol, symbol);
		(*root)->bits = (char*)malloc(sizeof(char)*(strlen(bits)+1));
		strcpy((*root)->bits, bits);
		(*root)->left = NULL;
		(*root)->right = NULL;
	}
	else if(bits[count] == '0')
	{
		insertBinaryTree(&(*root)->left, frequency, symbol, bits, count+1);
	}
	else
	{
		insertBinaryTree(&(*root)->right, frequency, symbol, bits, count+1);
	}
}

void insertBinaryTreeNode(node **root, node *node, int left)
{
	if(!(*root))
	{
		(*root) = node;
	}
	else if(left) insertBinaryTreeNode(&(*root)->left, node, 1);
	else insertBinaryTreeNode(&(*root)->right, node, 0);
}

void freeBinaryTree(node *root)
{
	if(root)
	{
		if(root->left) freeBinaryTree(root->left);
		else if(root->right) freeBinaryTree(root->right);
		free(root->symbol);
		free(root->bits);
		free(root);
	}
}

node *findNode(node *root, node *n)
{
	if(root)
	{
		/*if(strcmp(root->symbol, n->symbol) == 0 && root->frequency == n->frequency)*/
		if(strstr(root->symbol, n->symbol) && (root->left == NULL) && (root->right == NULL))
		{
			return root;
		}
		return findNode(root->left, n);
		return findNode(root->right, n);
	}
	return NULL;

}

void findNodeBySymbol(node *root, char *symbol, char *bits, char **path)
{
	char *bits2 = NULL;
	if(strcmp(root->symbol, symbol) == 0)
	{	
		printf("%c - %s\n", symbol[FIRST], bits);
		(*path) = (char*)calloc((strlen(bits)+1), sizeof(char));
		strcpy((*path), bits);
	}
	if(root->left)
	{
		bits2 = NULL;
		bits2 = (char*)realloc(bits2, sizeof(strlen(bits)+1));
		strcpy(bits2, bits);
		strcat(bits2, "0");
		findNodeBySymbol(root->left, symbol, bits2, path);
		free(bits2);
	}
	if(root->right)
	{
		bits2 = NULL;
		bits2 = (char*)realloc(bits2, sizeof(strlen(bits)+1));
		strcpy(bits2, bits);
		strcat(bits2, "1");
		findNodeBySymbol(root->right, symbol, bits2, path);
		free(bits2);
	}
}

void inOrder(node *node)
{
	if(node)
	{
		inOrder(node->left);
		printf("%d - %s\n", node->frequency, node->symbol);
		inOrder(node->right);
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
	Find first occurrence of a delimiter and return either string until delimiter or string after delimiter
	params:
	   - str: string to be searched;
	   - delimiter: character that delimits reading;
	   - after: (1) will copy string from delimiter, (0) up until delimiter
	returns:
	   - str up to occurrence of first delimiter. If no occurrence, returns NULL
*/
char *split(char *str, char delimiter, int after)
{
	int i, j, strLen = 0;
	char *newStr = NULL;
	strLen = strlen(str);
	for(i = 0; i < strLen; i++)
	{
		if(str[i] == delimiter && !after)
		{
			str[i] = '\0';
			newStr = (char*)malloc(sizeof(char)*((i+1)+1));
			strcpy(newStr, str);
			i = strLen + 1;
		}
		else if(str[i] == delimiter && after)
		{
			newStr = (char*)malloc(sizeof(char)*((strLen-i)+1));
			i = i + 1;
			for(j = 0; i < strLen; j++)
			{
				newStr[j] = str[i];
				i = i + 1;
			}
			newStr[j] = '\0';
			i = strLen + 1;
		}
	}
	return newStr;
}

/*
	Get file size
	param:
	   - file: file to be read
	returns:
	   - size of file, in bytes
*/
int getFileSize(FILE *file)
{
	int fileSize = 0;
	/* Get bytes of file size */
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	return fileSize;
}

/*
	Recursively use huffman compression
	params:
	   - trees: trees to be merged;
	   - treeSize: number of trees
	returns:
	   - huffman tree
*/
binaryTree *huffman(binaryTree *trees, int treeSize)
{
	int i;
	char *symbol;
	binaryTree *trees2;
	if(treeSize == 1)
	{
		return trees;
	}
	trees2 = (binaryTree*)calloc((treeSize-1), sizeof(binaryTree));
	symbol = (char*)malloc(sizeof(char)*(strlen(trees[FIRST].root->symbol)+strlen(trees[SECOND].root->symbol)+1));
	strcpy(symbol, trees[FIRST].root->symbol);
	strcat(symbol, trees[SECOND].root->symbol);
	/* Merge first two nodes from binaryTree list */
	insertBinaryTree(&trees2[FIRST].root, (trees[FIRST].root->frequency+trees[SECOND].root->frequency), symbol, "", 0);
	insertBinaryTreeNode(&trees2[FIRST].root, trees[FIRST].root, 1);
	insertBinaryTreeNode(&trees2[FIRST].root, trees[SECOND].root, 0);
	/*insertBinaryTree(&trees2[FIRST].root, trees[FIRST].root->frequency, trees[FIRST].root->symbol, "0", 0);
	insertBinaryTree(&trees2[FIRST].root, trees[SECOND].root->frequency, trees[SECOND].root->symbol, "1", 0);*/
	/* Copy remaining trees */
	for(i = 1; i < (treeSize-1); i++)
	{
		memcpy(&trees2[i], &trees[i+1], sizeof(binaryTree));
	}
	/* TODO - free */
	for(i = 0; i < (treeSize-1); i++)
	{
		// freeBinaryTree(trees2[i].root);
	}
	free(symbol);
	/* Sort list of trees */
	qsort(trees2, (treeSize-1), sizeof(binaryTree), cmp2);
	/*for(i = 0; i < (treeSize-1); i++)
	{
		printf("trees2[%d]\n", i);
		inOrder(trees2[i].root);
		printf("end of inOrder\n");
	}
	printf("\n");*/
	/* Recursively construct tree */
	return huffman(trees2, (treeSize-1));
}

/*
	Performs a huffman compression
	params:
	   - file: file to be read;
	   - fq2: freqSymbol struct to store all characters for later printing;
	   - count: fq2 size
	returns:
	   - huffmanTree
*/
binaryTree *huffmanCompression(FILE *file, freqSymbol **fq2, int *count)
{
	int i, j;
	char c;
	freqSymbol *fq;
	binaryTree *trees, *huffmanTree = NULL;
	fq = (freqSymbol*)calloc(128, sizeof(freqSymbol));
	while(fscanf(file, "%c", &c) != EOF)
	{
		if(fq[(int)c].frequency == 0)
		{
			(*count) = (*count) + 1;
			fq[(int)c].symbol = c;
		}
		fq[(int)c].frequency = fq[(int)c].frequency + 1;
	}
	qsort(fq, 128, sizeof(freqSymbol), cmp);
	(*fq2) = (freqSymbol*)calloc((*count), sizeof(freqSymbol));
	for(i = 127, j = (*count)-1; i >= (127-(*count)) && j >= 0; i--, j--)
	{
		(*fq2)[j].frequency = fq[i].frequency;
		(*fq2)[j].symbol = fq[i].symbol;
	}
	rewind(file);
	/* Create a list of trees */
	trees = (binaryTree*)calloc((*count), sizeof(binaryTree));
	// void insertBinaryTree(node **root, int frequency, char *symbol, char *bits, int (*count))
	for(i = 0; i < (*count); i++)
	{
		insertBinaryTree(&trees[i].root, (*fq2)[i].frequency, &(*fq2)[i].symbol, "", 0);
	}
	/* Recursively call huffman */
	huffmanTree = huffman(trees, (*count));

	/* Free variables */
	// for(i = 0; i < count; i++)
	// {
	// 	freeBinaryTree(trees[i].root);
	// }
	// free(trees);
	free(fq);
	return huffmanTree;
}

/*
	Print all possible paths from a given tree
	params:
	   - huffmanTree: tree to traverse;
	   - fq: freqSymbol struct containing all characters;
	   - count: fq size
*/
void printPaths(binaryTree *huffmanTree, freqSymbol *fq, int count)
{
	int i;
	char *symbol;
	/*printf("huffmanTree\n");
	inOrder(huffmanTree->root);
	printf("end of huffmanTree\n");*/
	qsort(fq, count, sizeof(freqSymbol), cmp3);
	for(i = 0; i < count; i++)
	{
		symbol = NULL;
		symbol = (char*)realloc(symbol, sizeof(char)*(2));
		symbol[0] = fq[i].symbol;
		symbol[1] = '\0';
		fq[i].path = NULL;
		findNodeBySymbol(huffmanTree->root, symbol, "", &fq[i].path);
		free(symbol);
	}
}

/*
	Get a sequence of bits' value
	param:
	   - byte: str string representing 8 bits
	returns:
	   - int value of 8 bits
*/
int byteToValue(char *byte)
{
	int i, value = 0;
	for(i = 7; i >= 0; i--)
	{
		if(byte[i] == '1')
		{
			value = value + pow(2, (7-i));
		}
	}
	return value;
}

/*
	Add shifting bits to fit into byte
	params:
	   - byte: char * string representing 1 byte;
	   - nBits: number of bits to be added
*/
void addBits(char **byte, int nBits)
{
	int i;
	while(nBits >= 0)
	{
		i = strlen((*byte))+2;
		(*byte) = (char*)realloc((*byte), sizeof(char)*i);
		(*byte)[i-1] = '\0';
		strcat((*byte), "0");
		nBits = nBits - 1;
	}
}

/*
	Print byte values from conversion to binary
	params:
	   - file: file;
	   - fq: freqSymbol struct containing all characters;
	   - count: fq size
*/
void printBytes(FILE *file, freqSymbol *fq, int count)
{
	int i, j, byteSize, addedBits = 0;
	char c, *byte;
	freqSymbol *aux;
	byte = (char*)malloc(sizeof(char)*1);
	byte[0] = '\0';
	byteSize = 0;
	while(fscanf(file, "%c", &c) != EOF)
	{
		aux = findFqBySymbol(fq, count, c);
		byte = (char*)realloc(byte, sizeof(char)*(strlen(byte)+strlen(aux->path)+1));
		strcat(byte, aux->path);
		byteSize = byteSize + strlen(aux->path);
		while(byteSize < 8 && fscanf(file, "%c", &c) != EOF)
		{
			aux = findFqBySymbol(fq, count, c);
			byte = (char*)realloc(byte, sizeof(char)*(strlen(byte)+strlen(aux->path)+1));
			strcat(byte, aux->path);
			byteSize = byteSize + strlen(aux->path);
		}
		if(byteSize < 8)
		{
			addBits(&byte, (8-byteSize));
			addedBits = 1;
		}
		printf("%d\n", byteToValue(byte));
		if(byteSize == 8)
		{
			free(byte);
			byte = NULL;
			byte = (char*)calloc(SECOND, sizeof(char));
			byte[FIRST] = '\0';
			byteSize = 0;
		}
		else if(byteSize > 8)
		{
			i = 0;
			j = byteSize - (byteSize-8);
			while(byteSize > 8)
			{
				byte[i] = byte[j];
				byteSize = byteSize - 1;
				i = i + 1;
				j = j + 1;
			}
			byte = (char*)realloc(byte, sizeof(char)*(i+1));
			byte[i] = '\0';
			byteSize = i;
		}
	}
	/* Print how many bits were actually coded from last byte */
	if(byteSize < 8)
	{
		if(!addedBits)
		{
			addBits(&byte, (8-byteSize));
			printf("%d\n", byteToValue(byte));
			free(byte);
		}
		printf("%d\n", byteSize);
	}
}

/*
	Find string in vector of strings
	params:
	   - bits: vector of strings;
	   - bit: string to be found
	returns:
	   - (index)position of string, (0) otherwise
*/
int match(char **bits, char *bit)
{
	int i;
	for(i = 0; i < 128; i++)
	{
		if(bits[i] && strcmp(bits[i], bit) == 0)
		{
			return i;
		}
	}
	return -1;
}

/*
	Performs a huffman decompression
	param:
	   - file: file to be read
*/
void huffmanDecompression(FILE *file)
{
	int i, j, strLen, count = 0, size = 0/*, value*/;
	char **bits, c, c2, c3, c4, *bit = NULL, *encryptedMessage = NULL, *message = NULL;
	bits = (char**)calloc(128, sizeof(char*));
	i = 0;
	while(fread(&c, sizeof(char), 1, file) > 0 && i >= 0)
	{
		if(i == 0 || c == '\n')
		{
			if(c == '\n')
			{
				fread(&c, sizeof(char), 1, file);
			}
			if(c != '-')
			{
				i = (int)c;
				strLen = 0;
				bits[(int)c] = (char*)realloc(bits[(int)c], sizeof(char)*2);
				strcpy(bits[(int)c], "");
				/* Skip next three elements - "\32-\32" */
				fread(&c, sizeof(char), 1, file);
				fread(&c, sizeof(char), 1, file);
				fread(&c, sizeof(char), 1, file);
			}
			else
			{
				i = -1;
			}	
		}
		else if(i > 0)
		{
			bits[i] = (char*)realloc(bits[i], sizeof(char)*(strLen+2));
			bits[i][strLen+1] = '\0';
			bits[i][strLen] = c;
			strLen = strLen + 1;
		}
	}
	/* Start decoding message */
	bit = (char*)calloc(2, sizeof(char));
	strcpy(bit, "");
	message = (char*)calloc(2, sizeof(char));
	strcpy(message, "");
	count = strlen(message);
	while(fread(&c, sizeof(char), 1, file) > 0)
	{
		encryptedMessage = (char*)realloc(encryptedMessage, sizeof(char)*(size+1));
		encryptedMessage[size] = c;
		size = size + 1;
	}
	encryptedMessage = (char*)realloc(encryptedMessage, sizeof(char)*(size+1));
	encryptedMessage[size] = '\0';
	for(j = 0; j < strlen(encryptedMessage)-1; j++)
	{
		if(j+1 == strlen(encryptedMessage)-1)
		{
			size = encryptedMessage[j+1];
		}
		else
		{
			size = 8;
		}
		c = encryptedMessage[j];
		/* Decode byte */
		for(i = 0, c2 = 0, c3 = 0; i < size; i++)
		{
			c2 = c << i;
			c3 = c2 >> 7;
			if(c3 == 0)
			{
				bit = (char*)realloc(bit, sizeof(char)*(strlen(bit)+2));
				strcat(bit, "0");
			}
			else
			{
				bit = (char*)realloc(bit, sizeof(char)*(strlen(bit)+2));
				strcat(bit, "1");
			}
			c4 = match(bits, bit);
			if(c4 != -1)
			{
				message = (char*)realloc(message, sizeof(char)*(count+1));
				message[count] = c4;
				count = count + 1;
				/*printf("%c\n", c4);*/
				free(bit);
				bit = NULL;
				bit = (char*)calloc(2, sizeof(char));
				strcpy(bit, "");
			}
			/*value = value + (c3 << 7);*/
		}
	}
	/* Discard last byte */
	message[count] = '\0';
	printf("%s\n", message);
}

int main(void)
{
	int count = 0;
	char *fileName, *extension;
	FILE *file;
	freqSymbol *fq;
	binaryTree *huffmanTree;

	/* Step 1: Read file name and open */
	fileName = read(stdin, '\n');
	file = fopen(fileName, "rb");
	if(file)
	{
		/* Step 2: Get file extension */
		extension = split(fileName, '.', 1);

		/* Step 3: Check if compression or decompression */
		/* Compression */
		if(strcmp(extension, "txt") == 0)
		{
			/* Build huffman tree */
			huffmanTree = huffmanCompression(file, &fq, &count);
			/* Print path */
			printPaths(huffmanTree, fq, count);
			printf("-\n");
			/* Print bytes */
			printBytes(file, fq, count);
			free(fq);
		}
		/* Decompression */
		else if(strcmp(extension, "huff") == 0)
		{
			huffmanDecompression(file);
		}
		/* Wrong! */
		else
		{
			printf("Shouldn't be here\n");
		}
		fclose(file);
		free(extension);
	}

	/* Step ???: Free variables */
	free(fileName);
	return 0;
}