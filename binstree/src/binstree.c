
#include "../../pile/header/pile.h"  // Require use of pile structure (you can find it inside repo)
#include "../../code/header/code.h"  // Require use of code structure (you can also find it inside this repo)
#include "../header/binstree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define TRUE 1
#define FALSE 0


typedef struct binstree { 
    
    // Contiene il nodo radice dell'albero
    struct node *radix; 
    // Contiene il numero di nodi
    unsigned int nnodes;
    
} *Bstree;


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

    tree->radix=NULL;
    // Restituisce il puntatore all'albero
    return tree;

}

unsigned int nnodes(struct binstree *tree){

    assert(tree != NULL);

    return tree->nnodes;

}

void append(struct binstree *tree, unsigned int key, void* value){
    
    // Verifica che l'oggetto e la funzione non siano nulli
    assert(tree != NULL);

    struct node *ptr, *parent, **ptrn=&(tree->radix), *newnode;

    // Il ciclo prosegue fino a che non raggiunge una foglia dell'albero ossia quando ottiene un puntatore NULL
    while ((ptr = *ptrn) != NULL){

        // Alla fine del ciclo, ptr sarà il puntatore alla foglia e sarà quindi NULL, parent invece
        // sarà il valore del padre
        parent = ptr;
        // Se la chiave del nodo corrente è maggiore della chiave passata come parametro
        if (ptr->key > key) ptrn = &(ptr->sx);
        // Se la chiave del nodo corrente è minore o uguale della chiave passata come parametro
        else if (ptr->key <= key) ptrn = &(ptr->dx);

    }

    *(newnode = smalloc(sizeof(struct node))) = (struct node) {
        .dx=NULL, .sx=NULL, .parent=parent, 
        .key=key, .value=value
    };

    *ptrn = newnode;

    tree->nnodes++;

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
        else if (ptop->key > key) ptop = ptop->sx;

        // Se il valore di result è -1, prosegue verso il figlio destro
        else if (ptop->key < key) ptop = ptop->dx;

    }

    return NULL;

}

void* search(struct binstree* tree, unsigned int key){

    struct node *n;
    struct binstree *sbt;

    assert(tree != NULL);
    // Se il sottoalbero è NULL, allora significa che la chiave non appartiene all'albero
    if ((sbt = subtree(tree, key)) != NULL && (n = sbt->radix) != NULL) {
        // Estrae il valore contenuto nella coppia del nodo
        void* value = n->value;
        // Dealloco lo spazio riservato per il sottoalbero generato dalla funzione subtree
        free(sbt);
        // Resituisce il valore
        return value;

    }

    return NULL;

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

    // Se sbttree è NULL significa che il nodo cercato non appartiene all'albero originario
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

            node->key=predec->key;
            node->value=predec->value;

            free(predec);

        }

        // Dealloca lo spazio per la struttura del sottoalbero generata per la ricerca
        free(sbttree);
        // Diminuisce di uno il n. di nodi dell'albero
        tree->nnodes--;

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
            ppred->key=parent->key;
            ppred->value=parent->value;

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

    /*  
        Richiede: struttura dati albero binario non nulla, chiave di ricerca (unsigned int)
        Effetto: Restituisce il padre del nodo a cui è associata la chiave passata come parametro;
                    se la chiave non esiste restituisce NULL.
    */
    
    struct pair *pair;
    struct node *n;
    struct binstree* sbt;

    assert(tree != NULL);

    // In questo caso subtree funziona come una search, restituisce però il sottoalbero
    // alla cui radice appartiene la chiave passata come parametro della funzione.
    // Nota che se subtree è nullo, significa che la chiave non esiste.
    if ((sbt = subtree(tree, key)) != NULL) {

        *(pair = smalloc(sizeof(struct pair))) = (struct pair){
            .key=sbt->radix->key,
            .value=sbt->radix->value
        };

        // Dealloco lo spazio del sottoalbero generato dalla funzione subtree
        free(sbt);

        // Restituisce la coppia chiave-valore del nodo padre
        return pair;

    }

    return NULL;

}

struct pair* max(struct binstree *tree){

    /* 
        Richiesto: struttura dati albero binario non nulla
        Effetto: Restituisce la struttura coppia chiave-valore i cui valori corrispondono alla 
                    massima chiave contenuta nell'albero e il relativo valore.
                    Nota che la chiave massima dell'albero è per definizione di 
                    albero binario di ricerca, il nodo destro più profondo del sottoalbero destro.
    */
    
    // ptru contiene l'indirizzo in memoria del PUNTATORE alla radice dell'albero
    struct node *ptru;
    struct pair *p;

    assert(tree != NULL);

    if((ptru = tree->radix) != NULL){
    
        // Finchè non raggiunge la profondità massima del sottoalbero sinistro continua
        while (ptru->dx != NULL) ptru = ptru->dx;

        // Crea la struttura dati contenente la coppia chiave-valore
        *(p = smalloc(sizeof(struct pair))) = (struct pair){
            .key=ptru->key,
            .value=ptru->value
        };

        return p;

    }

    return NULL;

}

struct pair* min(struct binstree *tree){

    /* 
        Richiesto: struttura dati albero binario non nulla
        Effetto: Restituisce la struttura coppia chiave-valore i cui valori corrispondono alla 
                    minima chiave contenuta nell'albero e il relativo valore.
                    Nota che la chiave minima dell'albero è per definizione di 
                    albero binario di ricerca, il nodo sinistro più profondo del sottoalbero sinistro.
    */

    // node_pointer contiene l'indirizzo in memoria del PUNTATORE alla radice dell'albero
    struct node *ptru;
    struct pair *p;

    assert(tree != NULL);

    if ((ptru = tree->radix) != NULL){

        // Finchè non raggiunge la profondità massima del sottoalbero sinistro continua
        while (ptru->sx != NULL) ptru = ptru->sx;

        // Crea la struttura dati contenente la coppia chiave-valore
        *(p = smalloc(sizeof(struct pair))) = (struct pair){
            .key=ptru->key,
            .value=ptru->value
        };

        return p;

    }

    return NULL;

}

void destroyTree(struct binstree *tree){

    /* 
        Richiede: struttura dati albero binario non nulla
        Effetto: dealloca lo spazio riservato a ciascun nodo dell'albero
                    e alla struttura dati stessa.

        Attenzione: non dealloca lo spazio riservato al valore contenuto in ciascun nodo!
    */
    
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

int maxdepth(struct binstree *tree){

    assert(tree != NULL);

    struct code *level = newcode();
    struct node *temp = tree->radix, *last = temp;
    int depth=0;

    if (temp != NULL) enqueue(level, temp);

    while(!isCodeEmpty(level)){

        temp = dequeue(level);

        if (last == NULL) last = temp;

        if (temp->key == last->key){

            if (temp->sx != NULL) enqueue(level, (last = temp->sx));

            if (temp->dx != NULL) enqueue(level, (last = temp->dx));

            if (temp->sx == NULL && temp->dx == NULL) last = NULL;

            depth++;

        }

        else{
            
            if (temp->sx != NULL) enqueue(level, temp->sx);

            if (temp->dx != NULL) enqueue(level, temp->dx);

        }

    }

    destroyCode(level);

    return depth;

}

void balance(struct binstree* tree){

    /*
        Richiesto: struttura dati albero binario non nullo
        Effetto: modifica l'albero bilanciando ricorsivamente ciascun sottoalbero.
                    Dato un nodo (u), radice dell'albero e rispettivamento il sottoalbero
                    sinistro (sx) la cui radice è il figlio sinistro di u e il sottoalbero destro (dx)
                    la cui radice è il figlio destro di u, questi si considerano bilanciati se la differenza
                    della profondita tra il sottoalbero sinistro e il sottoalbero destro è compresa tra -1 e +1.

                    -1 <= depth(sx) - depth(dx) <= +1; altrimenti l'albero è sbilanciato.
        
        Attenzione: questo metodo fa uso della ricorsione!
    */
    
    struct binstree *subtree_sx, *subtree_dx, *sub2tree_sx, *sub2tree_dx;
    int balanced_f;

    assert (tree != NULL);

    // Inizializzo i sottoalberi sinistro e destro della radice
    // Sono necessari ai fini della ricorsione poichè vengono passati come parametri
    // per le funzioni alberi e non nodi
    (subtree_sx = newtree())->radix = tree->radix->sx;
    (subtree_dx = newtree())->radix = tree->radix->dx;

    // Recursion
    if (subtree_sx->radix != NULL) balance(subtree_sx);
    if (subtree_dx->radix != NULL) balance(subtree_dx);

    // La modifica sul bilanciamento ha effetto sui sottoalberi ma il puntatore al figlio
    // sinistro o destro della radice continua a puntare al valore iniziale
    // Per questo assegno come nuovi puntatori i valori delle radici del sottoalbero
    tree->radix->sx = subtree_sx->radix;
    tree->radix->dx = subtree_dx->radix;

    // Reserve memory (Questi sottoalberi servono per determinare il tipo di rotazione)
    sub2tree_dx = newtree();
    sub2tree_sx = newtree();
    
    // Il ciclo prosegue finchè i due sottoalberi non hanno un fattore di bilanciamento compreso tra -1 e 1
    while(abs((balanced_f = maxdepth(subtree_sx) - maxdepth(subtree_dx))) > 1){
        
        // Sottoalbero sbilanciato a sinistra
        if (balanced_f > 1){

            sub2tree_sx->radix = subtree_sx->radix->sx;
            sub2tree_dx->radix = subtree_sx->radix->dx;

            // Rotazione semplice RR (right-right)
            if (maxdepth(sub2tree_sx) >= maxdepth(sub2tree_dx)) RRRotation(tree);
            // Rotazione doppia LR (left-right)
            else LRRotation(tree);


        }

        // Sottoalbero sbilanciato a destra
        else{

            sub2tree_sx->radix = subtree_dx->radix->sx;
            sub2tree_dx->radix = subtree_dx->radix->dx;

            // Rotazione semplice LL (left-left)
            if (maxdepth(sub2tree_sx) <= maxdepth(sub2tree_dx)) LLRotation(tree);
            // Rotazione doppia RL (right-left)
            else RLRotation(tree);

        }

        // Stesso principio di prima
        subtree_sx->radix = tree->radix->sx;
        subtree_dx->radix = tree->radix->dx;

    }

    // Libero la memoria allocata per le due strutture
    free(sub2tree_dx);
    free(sub2tree_sx);

    // Libero la memoria allocata per le due strutture
    free(subtree_sx);
    free(subtree_dx);

}

void RRRotation(struct binstree *tree){

    /* 
        Richiesto: Struttura albero binario non nulla
        Effetto: Modifica l'albero passato come parametro effettuando una rotazione 
                    a destra della radice e del figlio sinistro della radice

                    e.g. albero iniziale:   8       albero dopo RRRotation:   5
                                           /                                 / \
                                          5                                 1   8
                                         / \                                   /
                                        1   6                                 6
    */

    struct node **topnode, *lchild;

    assert(tree != NULL);

    // Estraggo la radice e il figlio sinistro della radice
    topnode = &(tree->radix), lchild = (*topnode)->sx;

    // Assegno come nuovo figlio sinistro della radice il figlio destro di lchild
    (*topnode)->sx = lchild->dx;
    // Assegno come nuovo figlio destro a lchild il nodo radice
    lchild->dx = *topnode;
    // La nuova radice dell'albero diventa lchild
    *topnode = lchild;

}

void LLRotation(struct binstree *tree){

    /*
        Richiesto: Struttura albero binario non nulla
        Effetto: Modifica l'albero passato come parametro effettuando una rotazione
                    a sinistra della radice e del figlio destro della radice

                    e.g. albero iniziale:   7        albero dopo LLRotation:   9
                                             \                                / \
                                              9                              7  10
                                             / \                              \
                                            8  10                              8
     */

    struct node **topnode, *rchild;

    assert(tree != NULL);

    // Estraggo la radice e il figlio destro della radice
    topnode = &(tree->radix), rchild = (*topnode)->dx;

    // Assegno come nuovo figlio destro della radice il figlio sinistro di rchild
    (*topnode)->dx = rchild->sx;
    // Assegno come nuovo figlio sinistro di rchild la radice
    rchild->sx = *topnode;
    // La nuova radice dell'albero diventa rchild
    *topnode = rchild;

}

void RLRotation(struct binstree *tree){

    /*
        Richiesto: struttura albero binario non nulla
        Effetto: Modifica l'albero passato come parametro effettuando una rotazione doppia
    */

    struct binstree *subtree_dx;

    assert(tree != NULL);

    if ((subtree_dx = subtree(tree, tree->radix->dx->key)) != NULL){

        RRRotation(subtree_dx);

        tree->radix->dx = subtree_dx->radix;

        LLRotation(tree);

        free(subtree_dx);

    }

}

void LRRotation(struct binstree *tree){

    struct binstree *subtree_sx;

    assert(tree != NULL);

    if ((subtree_sx = subtree(tree, tree->radix->sx->key)) != NULL){

        LLRotation(subtree_sx);

        tree->radix->sx = subtree_sx->radix;

        RRRotation(tree);

        free(subtree_sx);

    }

}

struct generator* depth_inorder_iter(struct binstree *tree){

    assert(tree != NULL);

    struct code *code = newcode();
    struct pile *support = newpile();
    struct node *temp = tree->radix;
    struct generator *gen = smalloc(sizeof(struct generator));

    // Continua finchè la pila non è vuota
    while(!isPileEmpty(support) || temp != NULL){

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

    gen->code = code;
    gen->current_node = dequeue(code);
    gen->visit = DEPTH_INORDER_VISIT;

    return gen;

}

struct generator* depth_preorder_iter(struct binstree *tree){

    /*
        Richiede: struttura dati albero binario non nulla
        Effetto: restituisce una struttura generatore tale per cui, ad ogni chiamata di genNext()
                    si otterrà una visita in preordine.
                    La visita in preordine visita prima la radice poi il figlio sinistro
                    e il figlio destro rispettivamente

    */

    struct node *temp;
    struct pile *pile;
    struct generator *gen;

    assert(tree != NULL);

    if ((temp=tree->radix) != NULL){

        pile = newpile();
        
        if (temp->dx != NULL) push(pile, temp->dx);

        if (temp->sx != NULL) push(pile, temp->sx);

    }

    *(gen = smalloc(sizeof(struct generator))) = (struct generator) {
        .pila=pile,
        .visit=DEPTH_PREORDER_VISIT, 
        .current_node=temp
    };

    return gen;

}

struct generator* depth_postorder_iter(struct binstree *tree){

    assert(tree != NULL);

    struct node* temp = tree->radix;
    struct pile *pile = newpile(), *support;
    struct generator *gen = smalloc(sizeof(struct generator));

    push((support = newpile()), temp);

    while(!isPileEmpty(support)){

        push(pile, (temp = pop(support)));

        if (temp->dx != NULL) push(support, temp->dx);

        if (temp->sx != NULL) push(support, temp->sx);

    }

    destroyPile(support);
       
    gen->pila = pile;
    gen->current_node=pop(pile);
    gen->visit=DEPTH_POSTORDER_VISIT;

    return gen;

}

struct generator* breadth_first_iter(struct binstree *tree){

    struct code *code;
    struct node *temp;
    struct generator *gen;

    assert(tree != NULL);

    if ((temp = tree->radix) != NULL){
        
        code = newcode();

        if (temp->sx != NULL) enqueue(code, temp->sx);

        if (temp->dx != NULL) enqueue(code, temp->dx);

    }

    *(gen = smalloc(sizeof(struct generator))) = (struct generator){
        .code=code, 
        .visit=BREADTH_FIRST_VISIT, 
        .current_node=temp
    };

    return gen;

}

struct pair* genNext(struct generator *gen){

    /*
        Richiede: struttura dati generatore, restituito da una delle funzioni di ricerca
        Effetto: restituisce il nodo successivo all'attuale nodo contenuto nel generatore,
                    rispettando il tipo di visita richiesto.
                    Il nodo successivo all'ultimo nodo dell'albero da visitare è NULL, quindi
                    una volta visitato l'intero albero questa funzione restituisce NULL.
    */

    struct node* temp;
    struct pile *pile_t;
    struct code *code_t;
    struct pair *pair;

    assert(gen != NULL);

    switch (gen->visit){

        case DEPTH_PREORDER_VISIT:
            
            gen->current_node = (temp = pop((pile_t = gen->pila)));

            if (temp != NULL){
            
                if (temp->dx != NULL) push(pile_t, temp->dx);

                if (temp->sx != NULL) push(pile_t, temp->sx);

            }

            break;

        case DEPTH_POSTORDER_VISIT:

            // Estra l'ultimo elemento della pila e lo assegna come nodo corrente
            gen->current_node = pop(gen->pila);

            break;

        case DEPTH_INORDER_VISIT:
            
            // Estrae il primo elemento della coda e lo assegna come nodo corrente
            gen->current_node = dequeue(gen->code);

            break;

        case BREADTH_FIRST_VISIT:

            gen->current_node = (temp = dequeue((code_t = gen->code)));

            if (temp != NULL){

                if (temp->dx != NULL) enqueue(code_t, temp->dx);

                if (temp->sx != NULL) enqueue(code_t, temp->sx);

            }

            break;

    }

    if (gen->current_node != NULL){
        
        *(pair = smalloc(sizeof(struct pair))) = (struct pair){ 
            .key=gen->current_node->key, 
            .value=gen->current_node->value
        };

        return pair;

    }

    return NULL;

}

struct pair* genCurr(struct generator *gen){

    /*
        Richiede: struttura dati generatore, restituito da una delle funzioni di ricerca
        Effetto: restituisce il nodo corrente contenuto nel generatore
    */
    
    // Verifica che il generatore non sia nullo
    assert(gen != NULL);

    struct pair *p = smalloc(sizeof(struct pair));

    p->key=gen->current_node->key;
    p->value=gen->current_node->value;

    // Restituisce il valore corrente del generatore
    return p;

}

void destroyGenerator(struct generator *gen){

    /*
        Richiede: struttura dati generatore, restituito da una delle funzioni di ricerca
        Effetto: dealloca lo spazio riservato alla struttura dati del generatore.
                    Una volta chiamata questa funzione, il generatore non è più utilizzabile,
                    di conseguenza anche le funzione genCurr e genNext non saranno valide.
    */

    assert(gen != NULL);

    switch(gen->visit){
        
        case DEPTH_POSTORDER_VISIT | DEPTH_PREORDER_VISIT:
            destroyPile(gen->pila);
            break;

        case BREADTH_FIRST_VISIT | DEPTH_INORDER_VISIT:
            destroyCode(gen->code);
            break;

        free(gen);

    }

}

unsigned int* keys(struct binstree *tree, int visit){

    struct generator *gen;
    unsigned int *keys;
    unsigned int i=0;

    assert(tree != NULL);

    switch(visit){

        case BREADTH_FIRST_VISIT:
            gen = breadth_first_iter(tree);
            break;

        case DEPTH_POSTORDER_VISIT:
            gen = depth_postorder_iter(tree);
            break;

        case DEPTH_PREORDER_VISIT:
            gen = depth_preorder_iter(tree);
            break;

            default:
                gen = depth_inorder_iter(tree);

    }

    keys = smalloc(sizeof(int) * tree->nnodes);

    do{ keys[i++] = genCurr(gen)->key; } while(genNext(gen) != NULL);

    return keys;

}

void* values(struct binstree *tree, int visit){

    struct generator *gen;
    void **values;
    unsigned int i=0;

    assert(tree != NULL);

    switch(visit){

        case BREADTH_FIRST_VISIT:
            gen = breadth_first_iter(tree);
            break;

        case DEPTH_POSTORDER_VISIT:
            gen = depth_postorder_iter(tree);
            break;

        case DEPTH_PREORDER_VISIT:
            gen = depth_preorder_iter(tree);
            break;

            default:
                gen = depth_inorder_iter(tree);

    }

    values = smalloc(sizeof(void*) * tree->nnodes);

    do{ values[i++] = genCurr(gen)->value; } while(genNext(gen) != NULL);

    return values;

}

struct pair** items(struct binstree *tree, int visit){

    struct generator *gen;
    struct pair **items;
    struct pair *temp;
    unsigned int i=0;

    assert(tree != NULL);

    switch(visit){

        case BREADTH_FIRST_VISIT:
            gen = breadth_first_iter(tree);
            break;

        case DEPTH_POSTORDER_VISIT:
            gen = depth_postorder_iter(tree);
            break;

        case DEPTH_PREORDER_VISIT:
            gen = depth_preorder_iter(tree);
            break;

            default:
                gen = depth_inorder_iter(tree);

    }

    items = smalloc(sizeof(struct pair*) * tree->nnodes);

    do{ 
        
        *(temp = smalloc(sizeof(struct pair))) = (struct pair){
            .key = genCurr(gen)->key,
            .value = genCurr(gen)->value
        };
        
        items[i++] = temp; 
        
    } while(genNext(gen) != NULL);

    return items;

}
