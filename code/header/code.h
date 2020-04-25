#ifndef CODE_H
#define CODE_H

typedef struct node node;

typedef struct code { struct node *first, *last; } *Code;

struct code* newcode();

void enqueue(struct code *code, void *item);

void* dequeue(struct code *code);

void* first(struct code *code);

void* last(struct code *code);

int isCodeEmpty(struct code* code);

void destroyCode(struct code* code);


#endif
