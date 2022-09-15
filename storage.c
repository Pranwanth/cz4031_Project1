#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_LEN 256
const int DISK_SIZE = 100000;
const int BLOCK_SIZE = 8;

typedef struct Record {
    int size;
    char id[10]; //Changed this from char* to char[] so the size of the record is always fixed.
    float averageRating;
    int numVotes;
    struct Record* next;
    int blockID;
} Record;


typedef struct Block {
    int id;
    int capacity;
    int remainSize;
    Record* firstRecord;
    struct Block* next;
} Block;


// method declaration

void createRecord(char* line, Record* record); 
void deleteRecord(char*id, Block* block); 
void printRecord(Record* record);
int isFull(Block* block, Record* record);
Block* createBlock(Block* preBlock);
void deleteBlock(Block** firstBlock, int blockID);
void printBlock(Block* block);
Record* getLastRecord(Block* block);
void insertRecord(Record* record, Block* block);


void createRecord(char* line, Record* record){
    char* tuple;
    char delim[2] = "\t";
    strcpy(tuple,line);

    char* id = strtok(tuple,delim);
    strcpy(record->id, id);

    char* averageRating = strtok(NULL,delim);
    record->averageRating = atof(averageRating);

    char* numVotes = strtok(NULL,delim);
    record->numVotes = atoi(numVotes);

    record->next = NULL;

    record->size = sizeof(Record);

    return;
}

int isFull(Block* block, Record* record) {
    int test = block->remainSize >= record->size ? 0 : 1;
    return test;
}

void deleteRecord(char*id, Block* block) {
    Record* recPtr = block->firstRecord;
    Record* pre;

    //Case 1: The record to be deleted is the first record.
    if(strcmp(recPtr->id, id) == 0)
    {
        pre = recPtr;
        recPtr = recPtr->next;
        block->firstRecord = recPtr;
        pre->next = NULL;
        block->remainSize += pre->size;  //update the remaining size of the block
        free(pre);
    }else{ //Case 2: The record to be deleted is the middle or at the end.

        //Travesing to the record to be deleted
        while(strcmp(recPtr->id, id) != 0)
        {
            pre = recPtr;
            recPtr = recPtr->next;
        }

        pre->next = recPtr->next;
        recPtr->next = NULL;
        block->remainSize += recPtr->size;
        free(recPtr);
    }
}

void printRecord(Record* record) {
    printf("Record: %s %.2f %d size: %d \n", record->id, record->averageRating, record->numVotes, record->size);
}

Block* createBlock(Block* preBlock) {
    static int blockID = 0;
    Block* newBlock = (Block*) malloc(sizeof(Block));

    newBlock->id = blockID ++;
    newBlock->capacity = BLOCK_SIZE;
    newBlock->remainSize = BLOCK_SIZE;
    newBlock->firstRecord = NULL;
    newBlock->next = NULL;
    
    if(preBlock!=NULL) { //If the block before this new block is not NULL, i.e., new block is not the first block created so far.
        preBlock->next = newBlock;
    }

    return newBlock;

}

// parameter: blocks refers to the pointer to the block
// note that blockID will not be updated, the blockID might not be consecutive
void deleteBlock(Block** firstBlock, int blockID) {
    Block* prev = *firstBlock;
    Block* temp = prev->next;

    if (firstBlock==NULL || (*firstBlock)==NULL) {
        printf("Invalid parameter\n");
        return;
    }

    //first block
    if ((*firstBlock)->id == blockID) {
        temp = *firstBlock;
        (*firstBlock) = (*firstBlock)->next;
        free(temp);
        printf("delete block with id %d successfully!\n", blockID);
        return;
    }

    while (temp!=NULL) {
        if (temp->id == blockID) {
            prev->next = temp->next;
            free(temp);
            printf("delete block with id %d successfully!\n", blockID);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void printBlock(Block* block) {
    char* recordID;
    if (block->firstRecord==NULL) {
        recordID = "No record currently!";
    } else {
        recordID = block->firstRecord->id;
    }
    printf("Block: id: %d, capacity: %d, remaining: %d, first record id: %s \n", block->id, block->capacity, block->remainSize, recordID);
}

// parameter: certain block
// return the last record stored in this block
Record* getLastRecord(Block* block) {
    Record* last = block->firstRecord;
    while(last->next!=NULL) {
        last = last->next;
    }
    return last;
}

// insert specified record into certain block
void insertRecord(Record* record, Block* block) {
    if (isFull(block, record)) {
        printf("Block is full, unable to insert!\n");
        return;
    }

    record->blockID = block->id;
    block->remainSize = block->remainSize - record->size;

    if (block->firstRecord==NULL) { //first record in block
        block->firstRecord = record;
    } else {
        Record* lastRecord = getLastRecord(block);
        lastRecord->next = record;
    }
}

int main(void)
{
    // load the file
    // Just to note tsv(tab separated values) means they are separated by '\t'(tab)
    // We probaby need an array to hold all the blocks
    FILE* fp;
    int maxBlockNum = floor(DISK_SIZE/BLOCK_SIZE);
    int testNum = 0;
    
    fp = fopen("./data.tsv", "r");
    if (fp == NULL) {
      perror("Failed: ");
      return 1;
    }

    char buffer[MAX_LEN];
    
    /*
    while (fgets(buffer, MAX_LEN, fp))
    {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
    }
    */
    Record* rec1 = (Record*) malloc(sizeof(Record));
    char id1[10] = "0001";
    rec1->size = sizeof(rec1);
    strcpy(rec1->id, id1);
    rec1->averageRating = 5.5;
    rec1->numVotes = 1000;
    rec1->next = NULL;
    rec1->blockID = -1;
    Record* rec2 = (Record*) malloc(sizeof(Record));
    char id2[10] = "0002";
    rec2->size = sizeof(rec2);
    strcpy(rec2->id, id2);
    rec2->averageRating = 6.5;
    rec2->numVotes = 1040;
    rec2->next = NULL;
    rec2->blockID = -1;
    Record* rec3 = (Record*) malloc(sizeof(Record));
    char id3[10] = "0003";
    rec3->size = sizeof(rec2);
    strcpy(rec3->id, id3);
    rec3->averageRating = 8.5;
    rec3->numVotes = 900;
    rec3->next = NULL;
    rec3->blockID = -1;

    printRecord(rec1);
    Block *block1 = (Block*) malloc(sizeof(Block));
    block1 = createBlock(NULL);
    printBlock(block1);
    insertRecord(rec1, block1);
    insertRecord(rec2, block1);
    Block* block2 = createBlock(block1);
    printBlock(block1->next);
    insertRecord(rec3, block2);
    printBlock(block1->next);
    deleteBlock(&block1, 0);
    printBlock(block1);

    /*
    //For testing 10 records
    while (fgets(buffer, MAX_LEN, fp) && testNum < 10) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
        Record* recPtr = (Record*) malloc(sizeof(Record)); //we have to malloc outside to prevent segementation fault
        createRecord(buffer, recPtr);
        printf("Record %d : %s %.2f %d size: %d \n", testNum, recPtr->id, recPtr->averageRating, recPtr->numVotes, recPtr->size);
        free(recPtr);
        testNum++;
    }
    */

    free(block1);
    free(block2);
    free(rec1);
    free(rec2);
    free(rec3);


    fclose(fp);
    return 0;
}