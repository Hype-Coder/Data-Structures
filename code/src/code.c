
#include "../header/code.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0


struct node {

    void* item;

    struct node *next, *prev;

};


// Define static safe malloc that prevents from memory allocations error
static void* smalloc(unsigned int size){

    void* object = malloc(size);

    if (object == NULL) {
        
        fprintf(stderr, "Memory allocation error\n");

        exit(0);

    }

    return object;

}


struct code* newcode(){

    struct code* code = (struct code*) smalloc(sizeof(code));

    code->first=NULL, code->last=NULL;

    return code;

}

void enqueue(struct code *code, void* item){

    assert(code != NULL && item != NULL);

    struct node* to_insert = (struct node*) smalloc(sizeof(struct node));

    to_insert->prev=code->last, to_insert->item=item, to_insert->next=NULL;

    if (code->last != NULL) code->last->next = to_insert;
    
    if (code->first == NULL) code->first = to_insert;

    code->last = to_insert;

}


void* dequeue(struct code *code){

    struct node *first;

    assert(code != NULL);

    if ((first = code->first) != NULL) {

        void* item = first->item;

        code->first = first->next;

        free(first);

        return item;

    }

}

void* first(struct code *code){

    assert(code != NULL);

    return code->first->item;

}

void* last(struct code *code){

    assert(code != NULL);

    return code->last->item;

}

int isCodeEmpty(struct code *code){

    assert(code != NULL);

    return code->first == NULL ? TRUE : FALSE;

}

void destroyCode(struct code *code){

    struct node *next=code->first, *this=next;

    if (!isCodeEmpty(code)){
        
        while ((next = next->next) != NULL) {
        
            free(this);

            this = next;

        }

    }

    free(code);

}
