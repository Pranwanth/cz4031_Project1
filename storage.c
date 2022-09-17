#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_LEN 2560
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
const int DISK_SIZE = 100000;
const int BLOCK_SIZE = 200;
const char* PATH = "./data.tsv";

typedef struct RecordHeader
{
    int size;
    int blockID;
} RecordHeader;


typedef struct Record {
    struct  RecordHeader header;
    char id[11]; //Changed this from char* to char[] so the size of the record is always fixed.
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
Block* getBlock(Block* startBlock, int id);
Record* getLastRecord(Block* block);
void insertRecord(Record* record, Block* block);
Block* run(FILE* fp);
int getBlockNum(Block* startBlock);

// extra method for better display
int getLineNum();
void printProgress(double percentage);

void createRecord(char* line, Record* record){
    char* tuple;
    char delim[2] = "\t";
    tuple = malloc(sizeof(char) * (strlen(line) + 1));
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
    newBlock->header.remainSize = BLOCK_SIZE - sizeof(Block);//Not BlockHeader as the firstRecPtr and nextBlockPtr also occupy space in a block 
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

Block* getBlock(Block* startBlock, int id) {
    Block* currentBlock = startBlock;
    while(currentBlock!=NULL) {
        if (currentBlock->header.id == id) {
            return currentBlock;
        }
        currentBlock = currentBlock->next;
    }
    return NULL;    // unable to find the target block or startBlock is invalid
}

int getBlockNum(Block* startBlock) {
    if (startBlock==NULL){
        return -1;
    }
    int blocks = 0;
    Block* tempBlock = startBlock;
    while(tempBlock!=NULL) {
        blocks ++;
        tempBlock = tempBlock->next;
    }
    return blocks;
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

void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

Block* run(FILE* fp) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Record* record;
    Block* firstBlock;
    Block* tempBlock = NULL;    // pointer to the current block (last block)
    
    read = getline(&line, &len, fp); //get the first line
    int flag = 0;   // initial flag
    while ((read = getline(&line, &len, fp)) != -1) {
        
        record = (Record*) malloc(sizeof(Record));
        createRecord(line, record);
        if (flag==0) {//initial stage
            firstBlock = createBlock(NULL);
            tempBlock = firstBlock;
            flag = 1;
        } else if (isFull(tempBlock, record)) {// current block is full
            tempBlock = createBlock(tempBlock);
        }

        insertRecord(record, tempBlock);

    }
    free(line);

    return firstBlock;
}

int getLineNum() {
    FILE* fp;
    fp = fopen(PATH, "r");
    char ch;
    int lines = 0;

    while(!feof(fp)) {
        ch = fgetc(fp);
        if(ch == '\n') {
            lines++;
        }
    }
    return lines;
}

int main(void)
{
    // load the file
    // Just to note tsv(tab separated values) means they are separated by '\t'(tab)
    // We probaby need an array to hold all the blocks
    FILE* fp;
    fp = fopen(PATH, "r");
    if (fp == NULL) {
      perror("Failed: ");
      return 1;
    }

    /*
    while (fgets(buffer, MAX_LEN, fp))
    {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
    }
    */

    Block* startBLock = run(fp);
    printf("total number of blocks: %d\n", getBlockNum(startBLock));

    free(startBLock);

    fclose(fp);
    return 0;
}