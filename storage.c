#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_LEN 256
const int DISK_SIZE = 100000;
const int BLOCK_SIZE = 8;

typedef struct RecordHeader
{
    int size;
    int blockID;
} RecordHeader;


typedef struct Record {
    struct  RecordHeader header;
    char id[10]; //Changed this from char* to char[] so the size of the record is always fixed.
    float averageRating;
    int numVotes;
    struct Record* next;
} Record;

typedef struct BlockHeader
{
    int id;
    int capacity;
    int remainSize;
} BlockHeader;



typedef struct Block {
    struct BlockHeader header;
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

    record->header.size = sizeof(Record);

    return;
}

int isFull(Block* block, Record* record) {
    int test = block->header.remainSize >= record->header.size ? 0 : 1;
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
        block->header.remainSize += pre->header.size;  //update the remaining size of the block
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
        block->header.remainSize += recPtr->header.size;
        free(recPtr);
    }
}

void printRecord(Record* record) {
    printf("Record: %s %.2f %d size: %d \n", record->id, record->averageRating, record->numVotes, record->header.size);
}

Block* createBlock(Block* preBlock) {
    static int blockID = 0;
    Block* newBlock = (Block*) malloc(sizeof(Block));

    newBlock->header.id = blockID ++;
    newBlock->header.capacity = BLOCK_SIZE;
    newBlock->header.remainSize = BLOCK_SIZE - sizeof(BlockHeader);
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
    if ((*firstBlock)->header.id == blockID) {
        temp = *firstBlock;
        (*firstBlock) = (*firstBlock)->next;
        free(temp);
        printf("delete block with id %d successfully!\n", blockID);
        return;
    }

    while (temp!=NULL) {
        if (temp->header.id == blockID) {
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
    printf("Block: id: %d, capacity: %d, remaining: %d, first record id: %s \n", block->header.id, block->header.capacity, block->header.remainSize, recordID);
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

    record->header.blockID = block->header.id;
    block->header.remainSize = block->header.remainSize - record->header.size;

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

    //For testing createRecord
    
    for(int i = 0; i < 6; i ++){
        fgets(buffer, MAX_LEN, fp);
        buffer[strcspn(buffer, "\n")] = 0;
        if(i > 0)
        {
            Record* recPtr = (Record*) malloc(sizeof(Record)); //we have to malloc outside to prevent segementation fault
            createRecord(buffer, recPtr);
            printf("Record %d : %s %.2f %d size: %d \n", i, recPtr->id, recPtr->averageRating, recPtr->numVotes, recPtr->header.size);
            free(recPtr);
        }
    }

    fclose(fp);
    return 0;
}