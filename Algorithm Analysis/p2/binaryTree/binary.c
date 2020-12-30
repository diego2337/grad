#include <stdio.h>
#include <stdlib.h>

typedef struct _node{
	int value;
	struct _node *left, *right;
}node;

void binaryInsert(node **root, int value)
{
	if((*root) == NULL)
	{
		(*root) = (node*)calloc(1, sizeof(node));
		(*root)->value = value;
	}
	else if((*root)->value > value)
	{
		binaryInsert(&(*root)->left, value);
	}
	else if((*root)->value < value)
	{
		binaryInsert(&(*root)->right, value);
	}
}

void preOrder(node *root)
{
	if(root != NULL)
	{
		printf("%d\n", root->value);
		preOrder(root->left);
		preOrder(root->right);
	}	
}

void inOrder(node *root)
{
	if(root != NULL)
	{
		inOrder(root->left);
		printf("%d\n", root->value);
		inOrder(root->right);
	}	
}

void posOrder(node *root)
{
	if(root != NULL)
	{
		posOrder(root->left);
		posOrder(root->right);
		printf("%d\n", root->value);
	}	
}

int binarySearch(node *root, int value)
{
	if(root)
	{
		if(root->value == value)
		{
			return root->value;
		}
		else
		{
			if(root->value > value)	binarySearch(root->left, value);
			if(root->value < value)	binarySearch(root->right, value);
		}
	}
	return -1;
}

void binaryFree(node **root)
{
	if((*root) != NULL)
	{
		binaryFree(&(*root)->left);
		binaryFree(&(*root)->right);
		free((*root));
	}	
}

int main(void)
{
	node *root = NULL;
	binaryInsert(&root, 6);
	binaryInsert(&root, 5);
	binaryInsert(&root, 8);
	binaryInsert(&root, 7);
	binaryInsert(&root, 9);
	inOrder(root);
	printf("%d\n", binarySearch(root, 8));
	binaryFree(&root);
	return 0;
}
