/*
	Program that reads either a .txt or .huff extension file and executes huffman compression or decompression, respectively.
	Author: Diego S. Cintra
	USP Number: 10094043
	Date: 18/05/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _freqSymbol{
	int frequency;
	char *symbol;
}freqSymbol;

void initFQ(freqSymbol *fq)
{
	(fq)->frequency = 0;
	(fq)->symbol = NULL;
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
	if((*(freqSymbol*)a).frequency < (*(freqSymbol*)b).frequency) 
	{
		return -1;
	}
	else if((*(freqSymbol*)a).frequency == (*(freqSymbol*)b).frequency) 
	{
		if((*(freqSymbol*)a).symbol && (*(freqSymbol*)b).symbol && strcmp( (*(freqSymbol*)a).symbol, (*(freqSymbol*)b).symbol ) < 0)
		{
			return -1;
		}
		else if((*(freqSymbol*)a).symbol && (*(freqSymbol*)b).symbol && strcmp( (*(freqSymbol*)a).symbol, (*(freqSymbol*)b).symbol ) == 0)
		{
			return 0;
		}
		else if((*(freqSymbol*)a).symbol && (*(freqSymbol*)b).symbol && strcmp( (*(freqSymbol*)a).symbol, (*(freqSymbol*)b).symbol ) > 0)
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
		if(strcmp(root->symbol, n->symbol) == 0 && root->frequency == n->frequency)
		{
			return root;
		}
		return findNode(root->left, n);
		return findNode(root->right, n);
	}
	return NULL;

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
	   - fq: freqSymbol struct;
	   - size: fq size;
	   - tree: tree to be inserted;
*/
void huffman(freqSymbol *fq, int size, binaryTree *tree, char **bits)
{
	int i;
	char c;
	freqSymbol *fq2;
	node *n, *n2;
	/* If size == 0, no more elements to merge - start binaryTree insertion */
	if(size == 1)
	{
		insertBinaryTree(&tree->root, fq[0].frequency, fq[0].symbol, "", 0);
		printf("begin inOrder\n");
		inOrder(tree->root);
		printf("end inOrder\n");
		return;
	}
	/* Merge both lowest rank elements */
	fq2 = (freqSymbol*)malloc(sizeof(freqSymbol)*(size-1));
	fq2[0].frequency = fq[0].frequency + fq[1].frequency;
	fq2[0].symbol = (char*)malloc(sizeof(char)*(strlen(fq[0].symbol)+strlen(fq[1].symbol)+1));
	strcpy(fq2[0].symbol, fq[0].symbol);
	strcat(fq2[0].symbol, fq[1].symbol);

	/* Copy remaining content */
	for(i = 1; i < size-1; i++)
	{
		fq2[i].frequency = fq[i+1].frequency;
		fq2[i].symbol = (char*)malloc(sizeof(char)*(strlen(fq[i+1].symbol)+1));
		strcpy(fq2[i].symbol, fq[i+1].symbol);
	}

	/* Sort new array */
	qsort(fq2, (size-1), sizeof(freqSymbol), cmp);
	/*printf("begin printf\n");
	for(i = 0; i < (size-1); i++)
	{
		printf("fq2[i].symbol: %s\n", fq2[i].symbol);
	}
	printf("end printf\n");*/

	/* Recursively call huffman */
	huffman(fq2, (size-1), tree, bits);
	n = (node*)malloc(sizeof(node));
	initNode(&n, fq2[0].frequency, fq2[0].symbol, NULL);
	n2 = findNode(tree->root, n);
	c = i + 48;
	n->bits = (char*)realloc(n->bits, sizeof(char)*(strlen((n2->bits))+3));
	strcat(n->bits, &c);

	/* Insert both left and right nodes */
	for(i = 0; i < 2; i++)
	{
		n = NULL;
		insertBinaryTree(&tree->root, fq[i].frequency, fq[i].symbol, (*bits), 0);
		printf("begin inOrder\n");
		inOrder(tree->root);
		printf("end inOrder\n");
		freeNode(n);
	}
	for(i = 0; i < (size-1); i++)
	{
		free(fq2[i].symbol);
	}
	free(fq2);
	/*n = (node*)malloc(sizeof(node));
	initNode(&n, fq[1].frequency, fq[1].symbol);
	n2 = getParent(tree->root, n);
	bits = (char*)malloc(sizeof(char)*(strlen(n2->symbol)+1));
	strcat(bits, "1");
	insertBinaryTree(tree->root, fq[1].frequency, fq[1].symbol, bits, 0);
	freeNode(&n);
	freeNode(&n2);
	free(bits);*/
}

/*
	Performs a huffman compression
	param:
	   - file: file to be read
*/
void huffmanCompression(FILE *file)
{
	int i, j, freqSymbolSize;
	freqSymbol *fq, *fq2;
	char c, *bits = NULL;
	binaryTree *tree = NULL;
	createBinaryTree(&tree);
	fq = (freqSymbol*)calloc(128, sizeof(freqSymbol));
	for(i = 0; i < 128; i++) initFQ(&fq[i]);
	while(fscanf(file, "%c", &c) != EOF)
	{
		if(fq[(int)c].frequency == 0)
		{
			fq[(int)c].symbol = (char*)malloc(sizeof(char)*2);
			fq[(int)c].symbol[0] = c;
			fq[(int)c].symbol[1] = '\0';
			/*printf("fq[(int)c].symbol: %s\n", fq[(int)c].symbol);
			strcpy(fq[(int)c].symbol, &c);*/
			fq[(int)c].frequency = 1;
		}
		else
		{
			fq[(int)c].frequency = fq[(int)c].frequency + 1;
		}
	}
	qsort(fq, 128, sizeof(freqSymbol), cmp);
	for(i = 0; fq[i].frequency == 0; i++){}
	fq2 = (freqSymbol*)malloc(sizeof(freqSymbol)*(128-i));
	freqSymbolSize = (128-i);
	for(j = 0; j < freqSymbolSize; j++, i++)
	{
		fq2[j].frequency = fq[i].frequency;
		fq2[j].symbol = (char*)malloc(sizeof(char)*2);
		strcpy(fq2[j].symbol, fq[i].symbol);
	}
	/* Recursively call huffman */
	bits = (char*)calloc(2, sizeof(char));
	huffman(fq2, freqSymbolSize, tree, &bits);
	free(bits);
	for(i = 0; i < 128; i++)
	{
		free(fq[i].symbol);
	}
	free(fq);
	for(i = 0; i < freqSymbolSize; i++)
	{
		free(fq2[i].symbol);
	}
	free(fq2);
	freeBinaryTree(tree->root);
	free(tree);
}

/*
	Performs a huffman decompression
*/
void huffmanDecompression()
{

}

int main(void)
{
	char *fileName, *extension;
	FILE *file;

	/* Step 1: Read file name and open */
	fileName = read(stdin, '\n');
	file = fopen(fileName, "r");
	if(file)
	{
		/* Step 2: Get file extension */
		extension = split(fileName, '.', 1);

		/* Step 3: Check if compression or decompression */
		/* Compression */
		if(strcmp(extension, "txt") == 0)
		{
			huffmanCompression(file);
		}
		/* Decompression */
		else if(strcmp(extension, "huff") == 0)
		{
			huffmanDecompression();
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