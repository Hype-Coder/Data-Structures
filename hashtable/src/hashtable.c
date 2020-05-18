#include "../header/hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0

struct node {

    // In caso di collisione punta al nodo successivo (lista di collisione)
    struct node *next;

    // Questo valore contiene la chiave originale
    void *key;

    // Questo valore potrebbe essere l'oggetto associato ad una chiave in un dizionario
    void* object;

};

typedef struct hasht {

    struct {
    
        struct node **array;

        unsigned int size;

    };

    // Questa è la funzione che converte l'oggetto in un intero hash
    unsigned int (*hash)(void*);

    // Questa funzione permette di effettuare dei confronti sulle chiavi originali
    // e verificare se gli oggetti sono presenti o meno nelle liste di collisione
    unsigned int (*equal)(void*, void*);

} *HashTable;


// Define static safe malloc that prevents from memory allocations error
static void* smalloc(unsigned int size){

    void* object = malloc(size);

    if (object == NULL) {
        
        fprintf(stderr, "Memory allocation error\n");

        exit(0);

    }

    return object;

}

struct hasht* newHashTable(unsigned int size, unsigned int (*hashcode)(void*), unsigned int (*equal)(void*, void*)){

    assert(hashcode != NULL);

    struct hasht* hashtable = smalloc(sizeof(struct hasht));
    hashtable->array = smalloc(size * sizeof(struct node*));
    hashtable->size = size;
    hashtable->hash = hashcode;
    hashtable->equal = equal;

    // Inizializzo i puntatori dell'array a NULL
    for (int i=0; i<size; i++) *(hashtable->array + i) = NULL;

    return hashtable;

}

void insertKey(struct hasht* hashtable, void *key, void* object){

    struct node *plist, *newnode;
    unsigned int hashedkey;

    assert(hashtable != NULL);
    // Verifica che la funzione hash restituisca un valore intero positivo
    assert((hashedkey = hashtable->hash(key)) >= 0);

    // Creo un nuovo nodo da inserire nella tabella hash
    newnode = smalloc(sizeof(struct node));
    newnode->next = NULL;
    newnode->key = key;
    newnode->object = object;

    if (hashtable->array[hashedkey] == NULL)

        hashtable->array[hashedkey] = newnode;

    else {

        for (plist = hashtable->array[hashedkey]; plist->next != NULL; (plist = plist->next));

        plist->next = newnode;
        
    }

}

void deleteKey(struct hasht *hashtable, void* key){

    unsigned int hashedkey;
    struct node **plist, *temp;

    assert(hashtable != NULL);

    assert((hashedkey = hashtable->hash(key)) >= 0);

    // Verifica se esiste una lista di collisioni
    if ((plist = &(hashtable->array[hashedkey])) != NULL){

        do {

            // Cerca se il valore è contenuto nella lista delle collisioni
            if (hashtable->equal((temp = (*plist))->key, key)){
                
                // Sostituisce il nodo da eliminare con il nodo successivo
                *plist = temp->next;

                free(temp);

                break;

            }

        } while((plist = &(temp->next)) != NULL);

    }

}

unsigned int searchKey(struct hasht *hashtable, void* key){

    /*
        Richiede: struttura hashtable non nulla && funzione hash che restituisce un valore intero >= 0
        Effetto: restituisce True se vi è un nodo contenuto nella hashtable la cui chiave
                    corrisponde alla chiave passata come parametro
                    Nota che la chiave può essere di qualsiasi tipo, verrà confrontata
                    tra le chiavi presenti nei nodi attraverso la funzione equal definita
                    nella struct hasht
    */

    struct node *plist;
    unsigned int hashedkey;

    assert(hashtable != NULL);

    assert((hashedkey = hashtable->hash(key)) >= 0);

    if ((plist = *(hashtable->array + hashedkey)) != NULL){

        do{
            
            if (hashtable->equal(plist->key,key))

                return TRUE;

        } while((plist = plist->next) != NULL);

    }

    return FALSE;

}

void* getObject(struct hasht *hashtable, void *key){

    /*
        Richiede: struttura hashtable non nulla && funzione hash che restituisce un valore intero >= 0
        Effetto: restituisce l'oggetto contenuto nel nodo la cui chiave
                    corrisponde alla chiave passata come parametro
                    Nota che la chiave può essere di qualsiasi tipo, verrà confrontata
                    tra le chiavi presenti nei nodi attraverso la funzione equal definita
                    nella struct hasht
    */


    struct node *plist;
    unsigned int hashedkey;

    assert(hashtable != NULL);

    assert((hashedkey = hashtable->hash(key)) >= 0);

    if ((plist = *(hashtable->array + hashedkey)) != NULL){

        do{
            
            if (hashtable->equal(plist->key,key))

                return plist->object;

        } while((plist = plist->next) != NULL);

    }

    return NULL;

}

void destroyHashTable(struct hasht* hashtable){

    /*
        Richiede: struttura hashtable non nulla
        Effetto: libera lo spazio allocato per ciascun nodo e lo spazio riservato
                    a contenere la struttura della tabella stessa.
                    Attenzione che non libera lo spazio riservato alla chiave e all'oggetto
                    contenuti in ciascun nodo.
    */

    struct node **nnode, *temp, *n;
    int counter;

    assert(hashtable != NULL);

    for (nnode=hashtable->array, counter=0; counter++ < hashtable->size; nnode = (nnode + 1))

        for (temp=*nnode, n=temp; temp != NULL && (n = n->next) != NULL; temp=n)

            free(temp);

    free(hashtable);

}
