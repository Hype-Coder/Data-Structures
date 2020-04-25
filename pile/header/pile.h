#ifndef PILE_H
#define PILE_H

typedef struct node node;

typedef struct pile { struct node *last; } *Pile;

struct pile* newpile();

void* pop(struct pile *pile);

void push(struct pile *pile, void* item);

void* top(struct pile *pile);

int isPileEmpty(struct pile *pile);

void destroyPile(struct pile *pile);

#endif
