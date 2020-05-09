
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

struct nnode {
    
    // Puntatore al figlio destro del nodo
    struct nnode *dx;
    // Puntatore al figlio sinistro
    struct nnode *sx;
    // Puntatore al node padre
    struct nnode *parent;
    // Oggetto incapsulato nel nodo
    struct {

        unsigned int key;

        void* value;

    };

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

    assert(code != NULL);

    struct node *newnode=smalloc(sizeof(struct node));

    newnode->prev = code->last;
    newnode->item = item;
    newnode->next = NULL;

    if (code->last != NULL) 
        code->last->next = newnode;
    
    if (code->first == NULL) 
        code->first = newnode;

    code->last = newnode;

}

void* dequeue(struct code *code){

    struct node *first, *next;

    assert(code != NULL);

    // Se il primo elemento della coda non è NULL
    if (!isCodeEmpty(code)) {   


        // Estrae l'oggetto contenuto nel primo nodo della coda
        void* item = (first = code->first)->item;
        //printf("Valore del nodo successivo al nodo attuale",((struct nnode*)first->next->item)->key);
        // Assegna come nuovo primo nodo della coda il nodo successivo al nodo estratto
        code->first = (next = first->next);

        if (next != NULL) 
            next->prev = NULL;

        if (next == NULL) 
            code->last = NULL;
        
        // Dealloca lo spazio riservato al nodo
        free(first);
        // Restituisce l'oggetto
        return item;

    }
    
    // Se la coda è vuota restituisce NULL
    return NULL;

}

void* first(struct code *code){

    // Verifica che la coda non sia nulla
    assert(code != NULL);

    return code->first != NULL ? code->first->item : NULL;

}

void* last(struct code *code){

    // Verifica che la coda non sia nulla
    assert(code != NULL);
    // Restituisce l'oggetto contenuto nel primo nodo se il primo nodo non è NULL
    return code->last != NULL ? code->last->item : NULL;

}

int isCodeEmpty(struct code *code){

    assert(code != NULL);

    return code->first == NULL ? TRUE : FALSE;

}

void destroyCode(struct code *code){

    assert(code != NULL);

    struct node *next=code->first, *this=next;

    if (!isCodeEmpty(code)){
        
        while ((next = next->next) != NULL) {
        
            free(this);

            this = next;

        }

    }

    free(code);

}
