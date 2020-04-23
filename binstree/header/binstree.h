#ifndef BINSTREE_H
#define BINSTREE_H


#define DEPTH_PREORDER_VISIT 0
#define DEPTH_POSTORDER_VISIT 1
#define DEPTH_INORDER_VISIT 2


typedef struct node node;

typedef struct binstree { struct node *radix; } *BSTree;

typedef struct pair { unsigned int key; void *value; } *Pair;

typedef struct generator generator;

struct binstree* newtree();

void append(struct binstree *tree, unsigned int key, void* value);

struct pair* max(struct binstree *tree);

struct pair* min(struct binstree *tree);

struct binstree* subtree(struct binstree *tree, unsigned int key);

void* search(struct binstree *tree, unsigned int key);

int isin(struct binstree *tree, unsigned int key);

struct pair* parent(struct binstree *tree, unsigned int key);

void delete(struct binstree *tree, unsigned int key);

struct pair* pred(struct binstree *tree, unsigned int key);

void destroyTree(struct binstree *tree); 

struct generator* depth_inorder_iter(struct binstree *tree);

struct generator* depth_preorder_iter(struct binstree *tree);

struct generator* depth_postorder_iter(struct binstree *tree);

struct pair* genNext(struct generator *gen);

struct pair* genCurr(struct generator *gen);

int isGeneratorEmpty(struct generator *gen);

void destroyGenerator(struct generator *gen);

#endif
