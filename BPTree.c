#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const int MAX_NUMBER_KEYS = 4;
const int SIZE_ID = 10;

typedef enum KeyType{
    ID = 0,
    AVG_VOTE = 10,
    NUM_VOTES = 20
} KeyType;

typedef struct Record {
    char* id;
    float averageRating;
    int numVotes;
    struct Record* next;
} Record;

typedef struct Key {
    // Key value can be Record or one of the values in record
    void* value;
    // For duplicate purposes, index each value of key
    int index;
} Key;

typedef struct BPNode {
    // Current number of keys in the node
    int numKeys;
    // Use this to point to value to check it
    Key** keys;
    // Links to the other children if there are any
    void** pointers;
    // Leaf node or not
    bool isLeaf;

    // Hold the parent to move backward
    struct BPNode* parent;

    // Hold the nodes after (Linked list)
    struct BPNode* next;
} BPNode;

BPNode* queue = NULL;

// Declare all functions here first to compile it properly
// Utility for B+ tree
Record* makeRecord(char* id, float averageRating, int numVotes);
Key* makeKey(KeyType keyType);
Key* setKey(KeyType keyType, Key* key, Record* record);
Key* createKey(KeyType keyType, Record* record);
BPNode* makeNode(KeyType keyType);
BPNode* makeLeaf(KeyType keyType);
BPNode* startNewTree(Key* key, KeyType keyType, Record* record);
BPNode* findNode(BPNode* root, Key* key, KeyType keyType, bool verbose, BPNode** leafOut);
Record* findRecord(BPNode* root, Key* key, KeyType keyType, bool verbose, BPNode** leafOut);
BPNode* findLeaf(BPNode* const root, Key* key, KeyType keyType, bool verbose);
void enqueue(BPNode* newNode);
BPNode* dequeue();
void printTree(BPNode* const root, KeyType keyType, bool verbose);
void printLeaves(BPNode* const root, KeyType keyType, bool verbose);
int height(BPNode* const root);
int pathToLeaves(BPNode* const root, BPNode* child);
int cut(int numKeys);

// Insert functionality
BPNode* insert(BPNode* root, Key* key, KeyType keyType, Record* record, bool verbose);
BPNode *insertIntoLeaf(BPNode *leaf, Key* key, KeyType keyType, Record* record);
BPNode* insertIntoLeafAfterSplitting(BPNode* root, BPNode* leaf, Key* key, KeyType keyType, Record* record);
BPNode* insertIntoParent(BPNode* root, BPNode* left, BPNode* right, Key* key, KeyType keyType);
BPNode* insertIntoNewRoot(BPNode* left, BPNode* right, Key* key, KeyType keyType);
BPNode* insertIntoNode(BPNode* root, BPNode* node, BPNode* right, int leftIndex, Key* key, KeyType keyType);
BPNode* insertIntoNodeAfterSplitting(BPNode* root, BPNode* oldNode, BPNode* right, int leftIndex, Key* key, KeyType keyType);

int getLeftIndex(BPNode* parent, BPNode* left);
bool whileLoopInfiniteCheck(int* counter, int limit);
bool compareEqualKeyType(KeyType keyType, void* value1, void* value2);
bool compareGTEByKeyType(KeyType keyType, void* value1, void* value2);
bool compareLTEByKeyType(KeyType keyType, void* value1, void* value2);
bool compareLTByKeyType(KeyType keyType, void* value1, void* value2);
bool compareGTByKeyType(KeyType keyType, void* value1, void* value2);
void printValue(KeyType keyType, void* value);

// Add to current queue
void enqueue(BPNode* newNode)
{
    BPNode* cur;
    if (queue == NULL)
    {
        queue = newNode;
        queue->next = NULL;
    }
    else
    {
        cur = queue;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }

        cur->next = newNode;
        newNode->next = NULL;
    }
}

// Dequeue from current queue
BPNode* dequeue()
{
    BPNode* node = queue;
    queue = queue->next;

    node->next = NULL;
    return node;
}

// Print the leaves
void printLeaves(BPNode* const root, KeyType keyType, bool verbose)
{
    if (root == NULL)
    {
        printf("Empty tree.\n");
        return;
    }

    int i;
    BPNode* c = root;

    while (!c->isLeaf)
    {
        c = c->pointers[0];
    }
    while (true)
    {
        for (i = 0; i < c->numKeys; i++)
        {
            if (verbose)
            {
                printf("%p ", c->pointers[i]);
                printValue(keyType, c->keys[i]->value);
            }
        }

        if (verbose)
        {
            printf("%p ", c->pointers[MAX_NUMBER_KEYS]);
        }
        if (c->pointers[MAX_NUMBER_KEYS] != NULL)
        {
            printf(" | ");
            c = c->pointers[MAX_NUMBER_KEYS];
        }
        else
        {
            break;
        }
    }
    printf("\n");
}

// Calculate height
int height(BPNode* const root)
{
    int h = 0;
    BPNode* c = root;

    while (!c->isLeaf)
    {
        c = c->pointers[0];
        h++;
    }

    return h;
}

// Get path to root
int pathToLeaves(BPNode* const root, BPNode* child)
{
    int length = 0;

    BPNode* c = child;
    while (c != root)
    {
        c = c->parent;
        length++;
    }

    return length;
}

// Print the tree
void printTree(BPNode* const root, KeyType keyType, bool verbose)
{
    BPNode* node = NULL;
    int i = 0;
    int rank = 0;
    int newRank = 0;

    if (root == NULL)
    {
        printf("Empty tree.\n");
        return;
    }

    queue = NULL;
    enqueue(root);

    while (queue != NULL)
    {
        node = dequeue();
        if (node->parent != NULL && node == node->parent->pointers[0])
        {
            newRank = pathToLeaves(root, node);
            if (newRank != rank)
            {
                rank = newRank;
                printf("\n");
            }
        }
        if (verbose)
        {
            printf("(%p)", node);
        }
        for (i = 0; i < node->numKeys; i++)
        {
            if (verbose)
            {
                printf("%p ", node->pointers[i]);
            }
            printValue(keyType, node->keys[i]->value);
        }
        if (!node->isLeaf)
        {
            for (i = 0; i <= node->numKeys; i++)
            {
                enqueue(node->pointers[i]);
            }
        }

        if (verbose)
        {
              if (node->isLeaf)
              {
                    printf("%p ", node->pointers[MAX_NUMBER_KEYS]);
              }
              else
              {
                    printf("%p ", node->pointers[node->numKeys]);
              }
        }
        printf("| ");
    }
    printf("\n");
}

// Creates a new record
Record* makeRecord(char* id, float averageRating, int numVotes)
{
    Record* newRecord = (Record*)malloc(sizeof(Record));
    // If it fails to allocate memory, fail immediately
    if (newRecord == NULL)
    {
        perror("New record could not be allocated memory");
        exit(EXIT_FAILURE);
    }
    else
    {
        newRecord->id = id;
        newRecord->averageRating = averageRating;
        newRecord->numVotes = numVotes;
        newRecord->next = NULL;
    }

    return newRecord;
}

Key* makeKey(KeyType keyType)
{
    Key* newKey;
    newKey = malloc(sizeof(Key));

    switch (keyType)
    {
    case ID:
        // Use Id
        newKey->value = malloc(sizeof(char*));
        break;
    case AVG_VOTE:
        // Use Average rating
        newKey->value = malloc(sizeof(float));
        break;
    case NUM_VOTES:
        // Use Number of votes
        newKey->value = malloc(sizeof(int));
        break;
    }
    // Throw an error if not able to create the keys
    if (newKey->value == NULL)
    {
        perror("Failed allocate memory to create keys");
        exit(EXIT_FAILURE);
    }
    newKey->index = 0;

    return newKey;
}

Key* setKey(KeyType keyType, Key* key, Record* record)
{
    switch (keyType)
    {
    case ID:
        // Use Id
        key->value = &(record->id);
        break;
    case AVG_VOTE:
        // Use Average rating
        key->value = &(record->averageRating);
        break;
    case NUM_VOTES:
        // Use Number of votes
        key->value = &(record->numVotes);
        break;
    }
}

// Use this for an easy way to create key
Key* createKey(KeyType keyType, Record* record)
{
    Key* newKey;
    newKey = makeKey(keyType);
    newKey = setKey(keyType, newKey, record);

    return newKey;
}

// Creates a new B+ Tree node
BPNode* makeNode(KeyType keyType)
{
    BPNode* newNode;
    newNode = malloc(sizeof(BPNode));

    if (newNode == NULL)
    {
        perror("Failed allocate memory to create a new node");
        exit(EXIT_FAILURE);
    }

    // Create the key value pointer to dynamically assign keys
    Key* key = makeKey(keyType);
    newNode->keys = malloc(MAX_NUMBER_KEYS * sizeof(key));
    if (newNode->keys == NULL)
    {
        perror("Keys were not able to be created");
        exit(EXIT_FAILURE);
    }

    // Create the pointer for children if there are any assigned
    newNode->pointers = malloc((MAX_NUMBER_KEYS + 1) * sizeof(void*));
    // Throw an error if not able to create child nodes
    if (newNode->pointers == NULL)
    {
        perror("Failed allocate memory to create child nodes pointer");
        exit(EXIT_FAILURE);
    }

    // Initialize everything else
    newNode->isLeaf = false;
    newNode->numKeys = 0;
    newNode->parent = NULL;
    newNode->next = NULL;

    return newNode;
}

// Create a leaf node
BPNode* makeLeaf(KeyType keyType)
{
    // Creating leaf node means its records being stored
    BPNode* leaf = makeNode(keyType);
    leaf->isLeaf = true;
    return leaf;
}

// Starts a new B+ tree
BPNode* startNewTree(Key* key, KeyType keyType, Record* record)
{
    // Create the root node as a leaf node since currently, layer = 1
    BPNode* root = makeLeaf(keyType);
    root->keys[0] = key;
    root->pointers[0] = record;

    // Set other pointers to null as there are no values
    for (int i = 1; i < MAX_NUMBER_KEYS; i++)
    {
        root->pointers[i] = NULL;
    }

    // Since no parent set to null and increase number of keys
    root->parent = NULL;
    root->numKeys++;

    return root;
}

// Find the Node required
BPNode* findNode(BPNode* root, Key* key, KeyType keyType, bool verbose, BPNode** leafOut)
{
    // No tree was created, return immediately
    if (root == NULL)
    {
        // Leaf is null just continue to set null
        if (leafOut != NULL)
        {
            *leafOut = NULL;
        }
        return NULL;
    }

    int i = 0;
    BPNode* leaf = NULL;

    // Search for leaf that contains the key value
    leaf = findLeaf(root, key, keyType, verbose);

    // Check each keys in leaf to determine which is the same
    for(i = 0; i < leaf->numKeys; i++)
    {
        if (compareEqualKeyType(keyType, leaf->keys[i]->value, key->value) && leaf->keys[i]->index == key->index)
        {
            break;
        }

        if (leafOut != NULL)
        {
            *leafOut = leaf;
        }
    }

    // if leaf does not have the specific key return null
    if (i == leaf->numKeys)
    {
        return NULL;
    }

    return leaf;
}

// Find the Node required
Record* findRecord(BPNode* root, Key* key, KeyType keyType, bool verbose, BPNode** leafOut)
{
    // No tree was created, return immediately
    if (root == NULL)
    {
        // Leaf is null just continue to set null
        if (leafOut != NULL)
        {
            *leafOut = NULL;
        }
        return NULL;
    }

    int i = 0;
    BPNode* leaf = NULL;

    // Search for leaf that contains the key value
    leaf = findLeaf(root, key, keyType, verbose);

    // Check each keys in leaf to determine which is the same
    for(i = 0; i < leaf->numKeys; i++)
    {
        if (compareEqualKeyType(keyType, leaf->keys[i]->value, key->value) && leaf->keys[i]->index == key->index)
        {
            break;
        }

        if (leafOut != NULL)
        {
            *leafOut = leaf;
        }
    }

    // if leaf does not have the specific key return null
    if (i == leaf->numKeys)
    {
        return NULL;
    }

    return (Record*)leaf->pointers[i];
}

// Find specific leaf with key value
BPNode* findLeaf(BPNode* const root, Key* key, KeyType keyType, bool verbose)
{
    // Check if root is null, in cases that it somehow bypasses all conditions
    if (root == NULL)
    {
        if (verbose)
        {
            printf("Empty Tree");
        }

        return root;
    }

    int i = 0;
    BPNode* c = root;
    int count = 0;
    while (!c->isLeaf)
    {
        if (whileLoopInfiniteCheck(&count, MAX_NUMBER_KEYS * 10000))
        {
            perror("Exceeded 10000 retries on finding leaf nodes.");
            exit(EXIT_FAILURE);
        }
        count++;
        if (verbose)
        {
            printf("[");
            for (i = 0; i < c->numKeys - 1; i++)
            {
                printValue(keyType, c->keys[i]->value);
            }
            printValue(keyType, c->keys[i]->value);
            printf("]");
        }

        i = 0;
        int count2 = 0;
        while (i < c->numKeys)
        {
            // While loop handler: any infinite loop will be handled by this
            if (whileLoopInfiniteCheck(&count2, MAX_NUMBER_KEYS * 10000))
            {
                perror("Exceeded 10000+ retries on check number of keys when finding leaf.");
                exit(EXIT_FAILURE);
            }
            count2++;

            // If key is found, break the loop
            if (compareGTEByKeyType(keyType, key->value, c->keys[i]->value))
            {
                i++;
            }
            else
                break;
        }

        if (verbose)
        {
            printf("%d -> \n", i);
        }

        // Set the node to pointer with value found at
        c = (BPNode*) c->pointers[i];
    }

    if (verbose)
    {
        printf("leaf [");
        for (i = 0; i < c->numKeys - 1; i++)
        {
            printValue(keyType, c->keys[i]->value);
        }

        printValue(keyType, c->keys[i]->value);
        printf("] -> \n");
    }

    return c;
}

// Error handling for any while loop so its not stuck infinitely
bool whileLoopInfiniteCheck(int* counter, int limit)
{
    if (*counter >= limit)
    {
        return true;
    }

    return false;
}

// Compare record value for Equality condition
bool compareEqualKeyType(KeyType keyType, void* value1, void* value2)
{
    switch(keyType)
    {
    case ID:
        break;
    case AVG_VOTE:
        if (*(float*) value1 == *(float*) value2)
        {
            return true;
        }
        break;
    case NUM_VOTES:
        if (*(int*) value1 == *(int*) value2)
        {
            return true;
        }
        break;
    }

    return false;
}

// Compare the record value for Greater than or equal condition
bool compareGTEByKeyType(KeyType keyType, void* value1, void* value2)
{
    switch(keyType)
    {
    case ID:
        break;
    case AVG_VOTE:
        if (*(float*) value1 >= *(float*) value2)
        {
            return true;
        }
        break;
    case NUM_VOTES:
        if (*(int*) value1 >= *(int*) value2)
        {
            return true;
        }
        break;
    }

    return false;
}

// Compare the record value for Greater than or equal condition
bool compareLTEByKeyType(KeyType keyType, void* value1, void* value2)
{
    switch(keyType)
    {
    case ID:
        break;
    case AVG_VOTE:
        if (*(float*) value1 <= *(float*) value2)
        {
            return true;
        }
        break;
    case NUM_VOTES:
        if (*(int*) value1 <= *(int*) value2)
        {
            return true;
        }
        break;
    }

    return false;
}

// Compare the record value for Greater than or equal condition
bool compareLTByKeyType(KeyType keyType, void* value1, void* value2)
{
    switch(keyType)
    {
    case ID:
        break;
    case AVG_VOTE:
        if (*(float*) value1 < *(float*) value2)
        {
            return true;
        }
        break;
    case NUM_VOTES:
        if (*(int*) value1 < *(int*) value2)
        {
            return true;
        }
        break;
    }

    return false;
}

// Compare the record value for Greater than or equal condition
bool compareGTByKeyType(KeyType keyType, void* value1, void* value2)
{
    switch(keyType)
    {
    case ID:
        break;
    case AVG_VOTE:
        if (*(float*) value1 > *(float*) value2)
        {
            return true;
        }
        break;
    case NUM_VOTES:
        if (*(int*) value1 > *(int*) value2)
        {
            return true;
        }
        break;
    }

    return false;
}

// Prints the record value
void printValue(KeyType keyType, void* value)
{
    switch(keyType)
    {
    case ID:
        break;
    case AVG_VOTE:
        printf("%9.2f ", *(float*)value);
        break;
    case NUM_VOTES:
        printf("%d ", *(int*)value);
        break;
    }
}

// Insert record into a leaf node
BPNode *insertIntoLeaf(BPNode *leaf, Key* key, KeyType keyType, Record* record)
{
    int i, insertionPoint;
    insertionPoint = 0;

    // Find for place to slot in the pointer/keys
    while (insertionPoint < leaf->numKeys && compareLTByKeyType(keyType, leaf->keys[insertionPoint]->value, key->value))
    {
        insertionPoint++;
    }

    // Move the values forward if need to make space inbetween
    for (i = leaf->numKeys; i > insertionPoint; i--)
    {
        leaf->keys[i] = leaf->keys[i-1];
        leaf->pointers[i] = leaf->pointers[i-1];
    }

    leaf->keys[insertionPoint] = key;
    leaf->pointers[insertionPoint] = record;
    leaf->numKeys++;

    return leaf;
}

// Split the leaf node to insert record (condition when leaf node is full)
BPNode* insertIntoLeafAfterSplitting(BPNode* root, BPNode* leaf, Key* key, KeyType keyType, Record* record)
{
    BPNode* newLeaf;
    Key** tempKeys;
    void** tempPointers;
    Key* newKey;
    int insertionIndex, split, i, j;

    newLeaf = makeLeaf(keyType);

    tempKeys = malloc((MAX_NUMBER_KEYS + 1) * sizeof(key));
    if (tempKeys == NULL)
    {
        perror("Failed to allocate temporary key memory");
        exit(EXIT_FAILURE);
    }

    tempPointers = malloc((MAX_NUMBER_KEYS + 1) * sizeof(void*));
    if (tempPointers == NULL)
    {
        perror("Failed to allocate temporary pointer memory");
        exit(EXIT_FAILURE);
    }

    // Find the insertion index to insert to
    insertionIndex = 0;
    while (insertionIndex < MAX_NUMBER_KEYS && compareLTByKeyType(keyType, leaf->keys[insertionIndex]->value, key->value))
    {
        insertionIndex++;
    }

    // Set the temporary memory first
    for (i = 0, j = 0; i < leaf->numKeys; i++, j++)
    {
        if (j == insertionIndex)
        {
            j++;
        }

        tempKeys[j] = leaf->keys[i];
        tempPointers[j] = leaf->pointers[i];
    }

    tempKeys[insertionIndex] = key;
    tempPointers[insertionIndex] = record;

    leaf->numKeys = 0;

    split = cut(MAX_NUMBER_KEYS);

    // Split the keys and pointers by the floor function
    for (i = 0; i < split; i++)
    {
        leaf->pointers[i] = tempPointers[i];
        leaf->keys[i] = tempKeys[i];
        leaf->numKeys++;
    }

    for (i = split, j = 0; i < MAX_NUMBER_KEYS + 1; i++, j++)
    {
        newLeaf->pointers[j] = tempPointers[i];
        newLeaf->keys[j] = tempKeys[i];
        newLeaf->numKeys++;
    }

    // Free the memory
    free(tempKeys);
    free(tempPointers);

    newLeaf->pointers[MAX_NUMBER_KEYS] = leaf->pointers[MAX_NUMBER_KEYS];
    leaf->pointers[MAX_NUMBER_KEYS] = newLeaf;

    for (i = leaf->numKeys; i < MAX_NUMBER_KEYS; i++)
    {
        leaf->pointers[i] = NULL;
    }
    for (i = newLeaf->numKeys; i < MAX_NUMBER_KEYS; i++)
    {
        newLeaf->pointers[i] = NULL;
    }

    newLeaf->parent = leaf->parent;
    newKey = newLeaf->keys[0];

    return insertIntoParent(root, leaf, newLeaf, newKey, keyType);
}

// Insert into the parent to set nodes
BPNode* insertIntoParent(BPNode* root, BPNode* left, BPNode* right, Key* key, KeyType keyType)
{
    int leftIndex;
    BPNode* parent;

    parent = left->parent;

    // This means there was no common root, so create it
    if (parent == NULL)
    {
        return insertIntoNewRoot(left, right, key, keyType);
    }

    leftIndex = getLeftIndex(parent, left);

    if (parent->numKeys < MAX_NUMBER_KEYS)
    {
        return insertIntoNode(root, parent, right, leftIndex, key, keyType);
    }

    return insertIntoNodeAfterSplitting(root, parent, right, leftIndex, key, keyType);
}

BPNode* insertIntoNewRoot(BPNode* left, BPNode* right, Key* key, KeyType keyType)
{
    BPNode* root = makeNode(keyType);
    root->keys[0] = key;
    root->pointers[0] = left;
    root->pointers[1] = right;
    root->numKeys++;
    root->parent = NULL;
    left->parent = root;
    right->parent = root;

    return root;
}

BPNode* insertIntoNode(BPNode* root, BPNode* node, BPNode* right, int leftIndex, Key* key, KeyType keyType)
{
    int i;

    for (i = node->numKeys; i > leftIndex; i--)
    {
        node->pointers[i+1] = node->pointers[i];
        node->keys[i] = node->keys[i-1];
    }

    node->pointers[leftIndex+1] = right;
    node->keys[leftIndex] = key;
    node->numKeys++;

    return root;
}

BPNode* insertIntoNodeAfterSplitting(BPNode* root, BPNode* oldNode, BPNode* right, int leftIndex, Key* key, KeyType keyType)
{
    int i, j, split;
    BPNode* newNode;
    BPNode* child;
    Key** tempKeys;
    Key* kPrime;
    BPNode** tempPointers;

    // Allocate temp memory
    tempKeys = malloc(MAX_NUMBER_KEYS * sizeof(key));
    if (tempKeys == NULL)
    {
        perror("Failed to allocate temporary key memory");
        exit(EXIT_FAILURE);
    }

    tempPointers = malloc((MAX_NUMBER_KEYS + 1) * sizeof(void*));
    if (tempPointers == NULL)
    {
        perror("Failed to allocate temporary pointer memory");
        exit(EXIT_FAILURE);
    }

    for (i = 0, j = 0; i < oldNode->numKeys+1; i++, j++)
    {
        if (j == leftIndex + 1)
        {
            j++;
        }

        tempPointers[j] = oldNode->pointers[i];
    }

    for (i = 0, j = 0; i < oldNode->numKeys; i++, j++)
    {
        if (j == leftIndex)
        {
            j++;
        }

        tempKeys[j] = oldNode->keys[i];
    }

    tempPointers[leftIndex+1] = right;
    tempKeys[leftIndex] = key;

    split = cut(MAX_NUMBER_KEYS);

    newNode = makeNode(keyType);
    oldNode->numKeys = 0;

    // Splitting the nodes to create the 2 separate nodes left: ceiling((n+1)/2), right: floor((n+1)/2)
    for (i = 0; i < split; i++)
    {
        oldNode->pointers[i] = tempPointers[i];
        oldNode->keys[i] = tempKeys[i];
        oldNode->numKeys++;
    }

    oldNode->pointers[i] = tempPointers[i];
    kPrime = tempKeys[split-1];
    for (++i, j = 0; i < MAX_NUMBER_KEYS; i++, j++)
    {
        newNode->pointers[j] = tempPointers[i];
        newNode->keys[j] = tempKeys[i];
        newNode->numKeys++;
    }

    newNode->pointers[j] = tempPointers[i];

    free(tempPointers);
    free(tempKeys);

    // Set the child nodes
    newNode->parent = oldNode->parent;
    for (i = 0; i <= newNode->numKeys; i++)
    {
        child = newNode->pointers[i];
        child->parent = newNode;
    }

    // Insert the nodes into the parent node
    return insertIntoParent(root, oldNode, newNode, kPrime, keyType);
}

// Get the index of the latest pointer within the parent
int getLeftIndex(BPNode* parent, BPNode* left)
{
    int leftIndex = 0;
    while (leftIndex <= parent->numKeys && parent->pointers[leftIndex] != left)
    {
        leftIndex++;
    }

    return leftIndex;
}

int insertDuplicate(Record* oldRecord, Record* newRecord)
{
    int count = 0;
    int explode = 0;
    Record* cur;

    cur = oldRecord;

    while (cur->next != NULL)
    {
        if (whileLoopInfiniteCheck(&count, 100000))
        {
            perror("Infinite loop detected while inserting duplicates");
            exit(EXIT_FAILURE);
        }

        cur = cur->next;
        count++;
    }

    cur->next = newRecord;

    return count;
}

// Insert a record into the B+ tree
BPNode* insert(BPNode* root, Key* key, KeyType keyType, Record* record, bool verbose)
{
    Record* recordPointer = NULL;
    BPNode* leaf = NULL;

    recordPointer = findRecord(root, key, keyType, verbose, NULL);
    if (recordPointer != NULL)
    {
        insertDuplicate(recordPointer, record);

        return root;
    }

    // If no root is found, create a new tree
    if (root == NULL)
    {
        return startNewTree(key, keyType, record);
    }

    // Find the leaf to assign to
    leaf = findLeaf(root, key, keyType, verbose);

    // As long as there is space, insert it in
    if (leaf->numKeys < MAX_NUMBER_KEYS)
    {
        leaf = insertIntoLeaf(leaf, key, keyType, record);
        return root;
    }

    // All else fails do the splitting logic and return the nodes after
    return insertIntoLeafAfterSplitting(root, leaf, key, keyType, record);
}

int cut(int numKeys)
{
    return floor((numKeys+1)/2);
}

void rangeQuery(BPNode* const root, Key* startKey, Key* endKey, KeyType keyType, bool verbose) {
    int i;
    BPNode* startLeaf = findLeaf(root, startKey, keyType, false);
    for(i = 0; i < startLeaf->numKeys; i++)
    {
        if (compareGTEByKeyType(keyType, startLeaf->keys[i]->value, startKey->value) && startLeaf->keys[i]->index == startKey->index)
        {
            break;
        }
    }

    // if leaf does not have the specific key return null
    if (i == startLeaf->numKeys)
    {
        printf("No keys found");
    }
    BPNode* curLeaf = startLeaf;
    int keysInRange[endKey -> value - startKey -> value];
    int idx = 0;
    while (compareLTEByKeyType(keyType, curLeaf->keys[i]->value, endKey->value) && curLeaf != NULL) {
        if (i == curLeaf -> numKeys) {
            if (curLeaf -> next != NULL) {
                curLeaf = curLeaf -> next;
                i = 0;
                continue;
            }
            else {
                break;
            }
        }
        keysInRange[idx] = curLeaf -> keys[i];
        idx++;
        i++;
    }
    printf("Keys found:");
    for (i = 0; i <= idx; i++) {
        printf("%d", keysInRange[idx]);
    }
    printf("\n");
}

int main()
{
    BPNode* root;
    Record* testRecord, *testRecord2, *testRecord3, *testRecord4, *testRecord5, *testRecord6;
    char instruction;

    char testId[10] = "abcdefg";
    root = NULL;
    testRecord = makeRecord(testId, 1.5, 100);
    testRecord2 = makeRecord(testId, 2.3, 200);
    testRecord3 = makeRecord(testId, 2.5, 300);
    testRecord4 = makeRecord(testId, 3, 400);
    testRecord5 = makeRecord(testId, 3, 500);
    testRecord6 = makeRecord(testId, 2, 100);

    KeyType keyType = NUM_VOTES;

    root = insert(root, createKey(keyType, testRecord), keyType, testRecord, false);
    root = insert(root, createKey(keyType, testRecord2), keyType, testRecord2, false);
    root = insert(root, createKey(keyType, testRecord3), keyType, testRecord3, false);
    root = insert(root, createKey(keyType, testRecord4), keyType, testRecord4, false);
    root = insert(root, createKey(keyType, testRecord5), keyType, testRecord5, true);
    root = insert(root, createKey(keyType, testRecord6), keyType, testRecord6, true);
    printTree(root, keyType, true);
}
