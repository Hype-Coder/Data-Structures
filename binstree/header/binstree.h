

#ifndef BINSTREE_H
#define BINSTREE_H


#define DEPTH_PREORDER_VISIT 0
#define DEPTH_POSTORDER_VISIT 1
#define DEPTH_INORDER_VISIT 2
#define BREADTH_FIRST_VISIT 3

typedef struct node node;

typedef struct binstree binstree;

typedef struct pair { 
    
    unsigned int key; 
    
    void *value; 
    
} *Pair;

typedef struct generator generator;

struct binstree* newtree();

void append(struct binstree *tree, unsigned int key, void* value);

struct pair* max(struct binstree *tree);

struct pair* min(struct binstree *tree);

unsigned int nnodes(struct binstree *tree);

struct binstree* subtree(struct binstree *tree, unsigned int key);

void* search(struct binstree *tree, unsigned int key);

int isin(struct binstree *tree, unsigned int key);

struct pair* parent(struct binstree *tree, unsigned int key);

void delete(struct binstree *tree, unsigned int key);

struct pair* pred(struct binstree *tree, unsigned int key);

void destroyTree(struct binstree *tree); 

int maxdepth(struct binstree *tree);

void balance(struct binstree *tree);

void LLRotation(struct binstree *tree);

void LRRotation(struct binstree *tree);

void RRRotation(struct binstree *tree);

void RLRotation(struct binstree *tree);

struct generator* depth_inorder_iter(struct binstree *tree);

struct generator* depth_preorder_iter(struct binstree *tree);

struct generator* depth_postorder_iter(struct binstree *tree);

struct generator* breadth_first_iter(struct binstree *tree);

struct pair* genNext(struct generator *gen);

struct pair* genCurr(struct generator *gen);

void destroyGenerator(struct generator *gen);

unsigned int* keys(struct binstree *tree, int visit);

void* values(struct binstree *tree, int visit);

struct pair** items(struct binstree *tree, int visit);


#endif
