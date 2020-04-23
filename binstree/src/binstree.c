#include "../../pile/header/pile.h"
#include "../../code/header/code.h" 
#include "../header/bstree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0


typedef struct node {
    
    // Puntatore al figlio destro del nodo
    struct node *dx;
    // Puntatore al figlio sinistro
    struct node *sx;
    // Puntatore al node padre
    struct node *parent;
    // Oggetto incapsulato nel nodo
    struct {

        unsigned int key;

        void* value;

    };

} *Node;


typedef struct generator {

    int visit;

    struct node *current_node;

    union {
        
        struct pile *pila;

        struct code *code;

    };

} *Generator;


// Define static safe malloc that prevents from memory allocations error
static void* smalloc(unsigned int size){

    void* object = malloc(size);

    if (object == NULL) {
        
        fprintf(stderr, "Memory allocation error\n");

        exit(0);

    }

    return object;

}


struct binstree* newtree(){

    // Alloco spazio per la struttura dati albero
    struct binstree *tree = (struct binstree*) smalloc(sizeof(struct binstree));
    // Assegna la radice come puntatore NULL
    tree->radix=NULL;
    // Restituisce il puntatore all'albero
    return tree;

}

void append(struct binstree *tree, unsigned int key, void* value){
    
    // Verifica che l'oggetto e la funzione non siano nulli
    assert(tree != NULL);

    struct node *ptr, *parent, **ptrn=&(tree->radix), *newnode=smalloc(sizeof(struct node));

    // Il ciclo prosegue fino a che non raggiunge una foglia dell'albero ossia quando ottiene un puntatore NULL
    while ((ptr = *ptrn) != NULL){

        // Alla fine del ciclo, ptr sarà il puntatore alla foglia e sarà quindi NULL, parent invece
        // sarà il valore del padre
        parent = ptr;
        // Raggiunta la foglia aggiunge il nuovo nodo al puntatore sinistro o destro
        // In base al risultato del confronto tra la chiave padre e la chiave del nuovo nodo
        ptrn = ptr->key > key ? &(ptr->sx) : &(ptr->dx);

    }

    // Assegna il nodo padre al nodo da aggiungere all'albero
    newnode->parent=parent, newnode->key=key, newnode->value=value, newnode->sx=NULL, newnode->dx=NULL;
    // Aggiunge il nuovo nodo come foglia dell'albero
    *ptrn = newnode;

}

// Restituisce il sottoalbero la cui radice corrisponde al valore passato come parametro
struct binstree* subtree(struct binstree *tree, unsigned int key){
    
    struct node *ptop = tree->radix;
    struct binstree *subtree;

    // Verifica che l'oggetto da cercare non sia nullo
    assert(tree != NULL);

    while (ptop != NULL){

        // Se il valore di result è 0, restituisce il nodo cercato
        if (ptop->key == key){

            // Assegna come radice del sottoalbero il nodo il cui valore è item
            (subtree = newtree())->radix = ptop;
            // Restituisce il sottoalbero creato
            return subtree;

        }  

        // Se il valore di result è 1, prosegue verso il figlio sinistro
        else if (ptop->key > key) 
            // Assegna al puntatore il figlio sinistro del nodo
            ptop = ptop->sx;

        // Se il valore di result è -1, prosegue verso il figlio destro
        else if (ptop->key < key) 
            // Assegna al puntatore il figlio destro del nodo
            ptop = ptop->dx;

    }

}

void* search(struct binstree* tree, unsigned int key){

    struct node *n;
    struct binstree *sbt;

    assert(tree != NULL);
    // Se il sottoalbero è NULL, allora significa che la chiave non appartiene all'albero
    if ((sbt = subtree(tree, key)) != NULL && (n = sbt->radix) != NULL) {
        // Estae il valore contenuto nella coppia del nodo
        void* value = n->value;
        // Dealloco lo spazio riservato per il sottoalbero generato dalla funzione subtree
        free(sbt);
        // Resituisce il valore
        return value;

    }

}

int isin(struct binstree *tree, unsigned int key){

    struct binstree *sbt;

    assert(tree != NULL);

    // Se il sottoalbero è NULL, allora significa che la chiave non appartiene all'albero
    if ((sbt = subtree(tree, key)) != NULL) {

        // Dealloco lo spazio riservato per il sottoalbero generato dalla funzione subtree
        free(sbt);
        // Resituisce il valore
        return TRUE;

    }
    
}

void delete(struct binstree *tree, unsigned int key){

    struct binstree *sbttree;
    struct node *node, *child, *parent;

    assert(tree != NULL);

    // Se sbttree è NULL significa che il nodo cercato non appartiene all'albero originario; solleva quindi un'eccezione
    if ((sbttree = subtree(tree, key)) != NULL) {

        // Verifica se figlio sinistro e destro sono nulli (in questo caso il nodo cercato corrisponde ad una foglia)
        if ((node=sbttree->radix)->sx == NULL && node->dx == NULL) {

            if ((parent = node->parent)->key > node->key) parent->sx = NULL;
            
            else parent->dx = NULL;

            free(node);
        
        }

        // Se il nodo ha un solo figlio
        else if ((child=node->sx) != NULL && node->dx == NULL || (child=node->dx) != NULL && node->sx == NULL) {

            // Se il nodo cercato è la radice dell'albero allora il figlio non nullo diventa la radice dell'albero
            if ((parent = node->parent) == NULL) 
                // Assegna come radice dell'albero il figlio non nullo del nodo trovato
                tree->radix = child;

            // Altrimenti l'arco padre-radice diventa arco padre-figlio non nullo della radice
            // Presta sempre attenzione al fatto che la radice in questo caso corrisponde alla radice del sottoalbero e quindi è il nodo cercato
            else {
            
                // Se il padre è maggiore della radice allora l'arco diventa figlio sinistro del padre-figlio non nullo della radice
                if (parent->key > node->key) parent->sx = child;
            
                // Altrimenti l'arco diventa figlio destro del padre - figlio non nullo della radice
                else parent->dx = child;

            }

            free(node);

        }

        else {

            // Individuo il predecessore del nodo cercato che in questo caso sarà il massimo del sottoalbero sinistro 
            // Nota che essendo il massimo non può avere 2 figli altrimenti non sarebbe il massimo
            struct pair* predec = pred(tree, node->key);

            if (predec != NULL) delete(tree, predec->key);

            node->key=predec->key, node->value=predec->value;

            free(predec);

        }

        free(sbttree);

    }

}

struct pair* pred(struct binstree* tree, unsigned int key){

    struct binstree *subt, *temp;
    struct node *parent, *ptru;
    struct pair *ppred;

    assert(tree != NULL);

    if ((subt = subtree(tree, key)) != NULL) {

        if ((ptru = subt->radix)->sx == NULL){

            while((parent = ptru->parent) != NULL && ptru->key < key) 
            
                ptru = parent;
            
            ppred = smalloc(sizeof(struct pair));

            ppred->key=parent->key, ppred->value=parent->value;

        }

        else {

            (temp = newtree())->radix = ptru->sx;

            ppred = max(temp);

            free(temp);

        }

        free(subt);

        return ppred;

    }

}

struct pair* parent(struct binstree* tree, unsigned int key){
    
    struct pair *pair;
    struct node *n;
    struct binstree* sbt;

    assert(tree != NULL);

    if ((sbt = subtree(tree, key)) != NULL && (n = sbt->radix) != NULL) {

        pair = smalloc(sizeof(struct pair));
        
        pair->key=n->key, pair->value=n->value;
        // Dealloco lo spazio del sottoalbero generato dalla funzione subtree
        free(sbt);
        // Restituisce la coppia chiave-valore del nodo padre
        return pair;

    }

}

struct pair* max(struct binstree *tree){
    
    // ptru contiene l'indirizzo in memoria del PUNTATORE alla radice dell'albero
    struct node *ptru;
    struct pair *p;

    assert(tree != NULL);

    ptru = tree->radix;
    // Finchè non raggiunge la profondità massima del sottoalbero sinistro continua
    while (ptru->dx != NULL) ptru = ptru->dx;

    p = smalloc(sizeof(struct pair));

    p->key=ptru->key, p->value=ptru->value;

    return p;

}

struct pair* min(struct binstree *tree){

    // node_pointer contiene l'indirizzo in memoria del PUNTATORE alla radice dell'albero
    struct node *ptru;
    struct pair *p;

    assert(tree != NULL);

    ptru = tree->radix;

    // Finchè non raggiunge la profondità massima del sottoalbero sinistro continua
    while (ptru->sx != NULL) ptru = ptru->sx;

    p = smalloc(sizeof(struct pair));

    p->key=ptru->key, p->value=ptru->value;

    return p;

}

void destroyTree(struct binstree *tree){
    
    struct node *temp;
    struct pile *pile;
    
    // Verifica che la struttura dati passata non sia nulla
    assert(tree != NULL);
    // Inizializza la pila e inserisce la radice dell'albero
    push((pile = newpile()), tree->radix);

    // Continua finchè la pila non è vuota
    while(!isPileEmpty(pile)){

        // Se l'elemento estratto non è nullo
        if ((temp = pop(pile)) != NULL){

            // Inserisco il figlio destro
            push(pile, temp->dx);
            // Inserisco il figlio sinistro
            push(pile, temp->sx);
            // Ricorda che l'ultimo elemento inserito è il primo ad essere estratto!            
            free(temp);

        }

    }

    // Libera lo spazio di memoria allocato per la pila di scorrimento
    destroyPile(pile);
    // Libera lo spazio di memoria allocato per la struttura albero
    free(tree);

}

struct generator* depth_inorder_iter(struct binstree *tree){

    assert(tree != NULL && tree->radix != NULL);

    struct code *code = newcode();
    struct pile *support;
    struct node *temp = tree->radix;
    struct generator *gen = malloc(sizeof(struct generator));

    // Inserisco il nodo radice nella pila
    push((support = newpile()), temp);
    // Continua finchè la pila non è vuota
    while(!isPileEmpty(support)){

        // Se il nodo non è nullo
        if (temp != NULL){

            // Inserisco nella pila l'attuale nodo
            push(support, temp);
            // Imposta il nodo attuale al nodo figlio sinistro dell'attuale nodo
            temp = temp->sx;
            
        }

        else{

            // Inserisce l'oggetto nell'array alla posizione indicata dalla variabile position
            enqueue(code, (temp = pop(support)));
            // Imposta il nodo attuale al nodo figlio destro dell'attuale nodo
            temp = temp->dx;

        }

    }

    destroyPile(support);

    gen->code=code, gen->current_node=dequeue(code), gen->visit=DEPTH_INORDER_VISIT;

    return gen;

}

struct generator* depth_preorder_iter(struct binstree *tree){

    struct node* temp = tree->radix;
    struct pile *pile = newpile();
    struct generator *gen = malloc(sizeof(struct generator));
            
    if (temp->dx != NULL) push(pile, temp->dx);

    if (temp->sx != NULL) push(pile, temp->sx);

    push(pile, temp);

    gen->pila=pile, gen->current_node=pop(pile), gen->visit=DEPTH_PREORDER_VISIT;

    return gen;

}

struct generator* depth_postorder_iter(struct binstree *tree){

    struct node* temp = tree->radix;
    struct pile *pile = newpile(), *support;
    struct generator *gen = malloc(sizeof(struct generator));

    push((support = newpile()), temp);

    while(!isPileEmpty(support)){

        push(pile, (temp = pop(support)));

        if (temp->dx != NULL) push(support, temp->dx);

        if (temp->sx != NULL) push(support, temp->sx);

    }

    destroyPile(support);
       
    gen->pila=pile, gen->current_node=pop(pile), gen->visit=DEPTH_POSTORDER_VISIT;

    return gen;

}

struct pair* genNext(struct generator *gen){

    struct node* temp;
    struct pile *pile;

    assert(gen != NULL);

    switch (gen->visit){

        case DEPTH_PREORDER_VISIT:

            gen->current_node = (temp = pop((pile = gen->pila)));

            if (temp->dx != NULL) push(pile, temp->dx);

            if (temp->sx != NULL) push(pile, temp->sx);

            break;

        case DEPTH_POSTORDER_VISIT:

            gen->current_node = pop(gen->pila);

            break;

        case DEPTH_INORDER_VISIT:

            gen->current_node = dequeue(gen->code);

            break;

    }

    struct pair *p = smalloc(sizeof(struct pair));

    p->key=gen->current_node->key, p->value=gen->current_node->value;

    return p;

}

struct pair* genCurr(struct generator *gen){
    
    // Verifica che il generatore non sia nullo
    assert(gen != NULL);

    struct pair *p = smalloc(sizeof(struct pair));

    p->key=gen->current_node->key, p->value=gen->current_node->value;

    // Restituisce il valore corrente del generatore
    return p;

}

int isGeneratorEmpty(struct generator *gen){

    assert(gen != NULL);

    if (gen->visit == DEPTH_INORDER_VISIT) return isCodeEmpty(gen->code);

    else return isPileEmpty(gen->pila);

}

void destroyGenerator(struct generator *gen){

    // Nel caso della visita simmetrica, libera lo spazio allocato per la coda
    if (gen->visit == DEPTH_INORDER_VISIT) destroyCode(gen->code);
    // Altrimenti libera lo spazio allocato per la pila
    else destroyPile(gen->pila);
    // Dealloca lo spazio occupato per la struttura del generatore
    free(gen);  

}

