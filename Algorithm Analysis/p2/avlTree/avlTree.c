#include <stdio.h>
#include <stdlib.h>

typedef struct _node{
	int value, leftSubtree, rightSubtree;
	struct _node *left, *right, *parent;
}node;

void rightBalance(node **root)
{
	node *aux = (*root)->right;
	(*root)->right = aux->left;
	aux->left = (*root);
	aux->leftSubtree = aux->parent->leftSubtree + 1;
	(*root)->rightSubtree = (*root)->rightSubtree + 1;
	if((*root)->parent != NULL)
	{
		(*root)->parent->left = aux;
	}
	(*root)->parent = aux;
}

void leftBalance(node **root)
{
	node *aux = (*root)->left;
	(*root)->left = aux->right;
	aux->right = (*root);
	aux->rightSubtree = aux->parent->rightSubtree + 1;
	(*root)->leftSubtree = (*root)->leftSubtree + 1;
	if((*root)->parent != NULL)
	{
		(*root)->parent->right = aux;
	}
	(*root)->parent = aux;
}

void balance(node **root)
{
	if((*root)->leftSubtree < (*root)->rightSubtree) rightBalance(root);
	else if((*root)->rightSubtree < (*root)->leftSubtree) leftBalance(root);
}

void avlInsert(node **root, node *parent, int value)
{
	if((*root) == NULL)
	{
		(*root) = (node*)calloc(1, sizeof(node));
		(*root)->value = value;
		(*root)->parent = parent;
		(*root)->left = NULL;
		(*root)->right = NULL;
	}
	else if((*root)->value >= value)
	{
		avlInsert(&(*root)->left, (*root), value);
		(*root)->leftSubtree = (*root)->leftSubtree + 1;
	}
	else if((*root)->value < value)
	{
		avlInsert(&(*root)->right, (*root), value);
		(*root)->rightSubtree = (*root)->rightSubtree + 1;
	}
	if(abs((*root)->leftSubtree - (*root)->rightSubtree) > 1)
	{
		balance(root);
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

void avlFree(node **root)
{
	if((*root) != NULL)
	{
		avlFree(&(*root)->left);
		avlFree(&(*root)->right);
		free((*root));
	}	
}

int main(void)
{
	node *root = NULL;
	avlInsert(&root, NULL, 9);
	avlInsert(&root, NULL, 27);
	avlInsert(&root, NULL, 50);
	inOrder(root);
	return 0;
}
