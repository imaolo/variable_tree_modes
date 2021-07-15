#include <stdlib.h>
#include <stdio.h>
#include <openssl/sha.h>
#define init_node(node) \
	&node->hash = NULL;\
	&node->data = NULL;\
	&node->parent = NULL;\
	&node->hash = NULL;\
	node->index = -1;\
typedef unsigned char* (*Hash_Function) (const unsigned char *, size_t  , unsigned char *);
struct node {
	unsigned char *hash;
	unsigned char *data;
	struct node *parent;
	struct node *children;
	int index;
} default_m_node = {NULL,NULL,NULL,NULL,-1};
typedef struct node m_node;

typedef struct {
	size_t n;
    const size_t tree_height;
    const size_t *arities;
    const size_t data_blocks;
    const size_t data_block_size;
    const size_t hash_size;
    unsigned char **data;
    Hash_Function hash_function;
    m_node *root;
} m_tree;



// creates the skeleton for the tree and places the data in the leaf nodes
void buildTree (m_node *root, m_tree *tree, int h); 
// computes the merkle tree using a top down approach
void hashTreeTD(m_node *root, m_tree *tree, int h);
// computes the merkle tree using the bottom up approach
void hashTreeBU(m_node *nodes, m_tree *tree, int h);
// prints the trees hash nodes, will not work unless the hashes have been computed
void printTree (m_node *root, m_tree *tree, int h);
// free the tree's memory
void freeTree  (m_node *root, m_tree *tree, int h);

int main(int argc, char **argv){
	size_t h = 2;
	size_t arities[3] = {3,3};
	size_t data_blocks = 1;
	for (int i=0;i<h;i++)
		data_blocks *= arities[i];
	m_node root = default_m_node;
	unsigned char **data = malloc(sizeof(unsigned char *)*data_blocks);
	for (int i=0;i<data_blocks;i++)
		data[i] = "0";
	m_tree tree = {0,h,arities,data_blocks,1,SHA_DIGEST_LENGTH,data,SHA1,&root};
	
	
	printf("building tree\n");
	buildTree(&root,&tree,h);

	printf("processing tree\n");
	hashTreeTD(&root,&tree,h);

	printf("printing tree\n");
	printTree(&root,&tree,h);

	printf("freeing tree\n");
	freeTree(&root,&tree,h);


	free(data);
	return 0;
}


void buildTree(m_node *curr, m_tree *tree, int h){
	//curr->index = h;
	// curr->children = NULL;
	// curr->data = NULL;
	curr->hash = (unsigned char *)malloc(sizeof(unsigned char)*tree->hash_size);

	if (h == 0){
		curr->data = tree->data[tree->n];
		tree->n++;
		return;
	}

	int arity = tree->arities[h-1];
	curr->children = malloc(arity*sizeof(m_node)); 
	for (int i=0;i<arity;i++){
		curr->children[i] = default_m_node;
		curr->children[i].parent = curr;
		buildTree(&curr->children[i],tree,h-1);
	}

	return;
}
void hashTreeTD(m_node *curr, m_tree *tree, int h){
	if (h == 0){
		tree->hash_function(curr->data,tree->data_block_size,curr->hash);
		return;
	}

	int arity = tree->arities[h-1];
	unsigned char data[tree->hash_size*arity];
	for (int i=0;i<arity;i++){
		hashTreeTD(&curr->children[i],tree,h-1);
		for (int j=0;j<tree->hash_size;j++)
			data[i*tree->hash_size+j] = curr->children[i].hash[j];
	}
	tree->hash_function(data,tree->hash_size*arity,curr->hash);

	return;
}
void hashTreeBU(m_node *nodes, m_tree *tree, int h){
}
void printTree(m_node *curr, m_tree *tree, int h){
	printf("%d: ",curr->index);
	for (int i=0;i<tree->hash_size;i++)
		printf("%02x",curr->hash[i]);
	printf("\n");

	if (h == 0)
		return;

	int arity = tree->arities[h-1];
	for (int i=0;i<arity;i++)
		printTree(&curr->children[i],tree,h-1);

	return;
}
void freeTree (m_node *curr, m_tree *tree, int h){
	free(curr->hash);
	if (h == 0)
		return;

	int arity = tree->arities[h-1];
	for (int i=0;i<arity;i++)
		freeTree(&curr->children[i],tree,h-1);

	free(curr->children);
	return;
}