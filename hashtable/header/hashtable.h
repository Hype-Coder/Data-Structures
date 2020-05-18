typedef struct hasht hashtable;


struct hasht* newHashTable(unsigned int size, unsigned int (*hash)(void*), unsigned int (*equal)(void*, void*));

void insertKey(struct hasht* hashtable, void *key, void* object);

void deleteKey(struct hasht* hashtable, void* key);

unsigned int searchKey(struct hasht* hashtable, void* key);

void* getObject(struct hasht *hashtable, void *key);

void destroyHashTable(struct hasht* hashtable);
