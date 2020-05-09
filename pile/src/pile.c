

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../header/pile.h"

#define TRUE 1
#define FALSE 0

// Struttura interna per gestire i nodi
struct node {
    
    // Oggetto incapsulato nel nodo
    void *item;
    // Puntatore al nodo precedente
    struct node* prev;

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

struct pile* newpile(){

    struct pile* pile = (struct pile*) smalloc(sizeof(struct pile));

    pile->last=NULL;

    return pile;
    
}

void push(struct pile *pile, void *item){

    // Verifica che il puntatore a pile non sia nullo
    assert(pile != NULL);

    struct node* to_insert = smalloc(sizeof(struct node));
    
    to_insert->prev = pile->last;
    to_insert->item = item;

    pile->last = to_insert;

}

void* pop(struct pile *pile){

    // Verifica che il puntatore a pile non sia nullo
    assert(pile != NULL);
    
    struct node *last = pile->last;

    // Se l'ultimo valore è nullo allora la pila è vuota, altrimenti
    if (!isPileEmpty(pile)){

        void *object = last->item;
        // Assegna come ultimo elemento della pila, il nodo precedente a quello estratto
        pile->last = last->prev;
        // Libera lo spazio di memoria riservato al nodo
        free(last);
        // Restituisce l'oggetto contenuto nel nodo
        return object;

    }

    // Se la pila è vuota restituisce NULL
    return NULL;

}

void* top(struct pile *pile){

    // Verifica che il puntatore a pile non sia nullo
    assert(pile != NULL);
    // Restituisce l'ultimo elemento della pila senza deallocarlo
    return pile->last != NULL ? pile->last->item : NULL;

}

int isPileEmpty(struct pile *pile){

    // Verifica che il puntatore a pile non sia nullo
    assert(pile != NULL);
    // Restituisce 1 se il puntatore all'ultimo elemento è nullo, altrimenti restituisce 0
    return pile->last == NULL ? TRUE : FALSE;

}

void destroyPile(struct pile *pile){

    // Verifica che il puntatore a pile non sia nullo
    assert(pile != NULL);

    struct node *prev, *curr=pile->last;
    // Se la radice non è nulla
    if (curr != NULL){
        // Finchè non svuola la pila
        while((prev = curr->prev) != NULL) {
            // Libera lo spazio allocato per il nodo corrente
            free(curr);
            // Sposta il puntatore al nodo precedente il nodo corrente
            curr = prev;

        }

    }

    // Libera lo spazio allocato per la pila
    free(pile);
    
}
