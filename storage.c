#define _GNU_SOURCE

#include "storage.h"

void createRecord(char *line, Record *record) {
    char *tuple;
    char delim[2] = "\t";
    tuple = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(tuple, line);

    char *id = strtok(tuple, delim);
    strcpy(record->id, id);

    char *averageRating = strtok(NULL, delim);
    record->averageRating = atof(averageRating);

    char *numVotes = strtok(NULL, delim);
    record->numVotes = atoi(numVotes);

    record->next = NULL;

    record->header.size = sizeof(Record);

    return;
}

int isFull(Block *block, Record *record) {
    int test = block->header.remainSize >= record->header.size ? 0 : 1;
    return test;
}

void deleteRecord(char *id, Block *block) {
    Record *recPtr = block->firstRecord;
    Record *pre;

    // Case 1: The record to be deleted is the first record.
    if (strcmp(recPtr->id, id) == 0) {
        pre = recPtr;
        recPtr = recPtr->next;
        block->firstRecord = recPtr;
        pre->next = NULL;
        block->header.remainSize += pre->header.size;  //update the remaining size of the block
        free(pre);
    } else { // Case 2: The record to be deleted is the middle or at the end.

        // Traversing to the record to be deleted
        while (strcmp(recPtr->id, id) != 0) {
            pre = recPtr;
            recPtr = recPtr->next;
        }

        pre->next = recPtr->next;
        recPtr->next = NULL;
        block->header.remainSize += recPtr->header.size;
        free(recPtr);
    }
}

void printRecord(Record *record) {
    printf("Record: %s %.2f %d size: %d \n", record->id, record->averageRating, record->numVotes, record->header.size);
}

Block *createBlock(Block *preBlock) {
    static int blockID = 0;
    Block *newBlock = (Block *) malloc(sizeof(Block));

    newBlock->header.id = blockID++;
    newBlock->header.capacity = BLOCK_SIZE;
    newBlock->header.remainSize = BLOCK_SIZE -
                                  sizeof(Block);//Not BlockHeader as the firstRecPtr and nextBlockPtr also occupy space in a block
    newBlock->firstRecord = NULL;
    newBlock->next = NULL;

    if (preBlock !=
        NULL) { //If the block before this new block is not NULL, i.e., new block is not the first block created so far.
        preBlock->next = newBlock;
    }

    return newBlock;

}

// parameter: blocks refers to the pointer to the block
// note that blockID will not be updated, the blockID might not be consecutive
void deleteBlock(Block **firstBlock, int blockID) {
    Block *prev = *firstBlock;
    Block *temp = prev->next;

    if (firstBlock == NULL || (*firstBlock) == NULL) {
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

    while (temp != NULL) {
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

void printBlock(Block *block) {
    char *recordID;
    if (block->firstRecord == NULL) {
        recordID = "No record currently!";
    } else {
        recordID = block->firstRecord->id;
    }
    printf("Block: id: %d, capacity: %d, remaining: %d, first record id: %s \n", block->header.id,
           block->header.capacity, block->header.remainSize, recordID);
}

Block *getBlock(Block *startBlock, int id) {
    Block *currentBlock = startBlock;
    while (currentBlock != NULL) {
        if (currentBlock->header.id == id) {
            return currentBlock;
        }
        currentBlock = currentBlock->next;
    }
    return NULL;    // unable to find the target block or startBlock is invalid
}

int getBlockNum(Block *startBlock) {
    if (startBlock == NULL) {
        return -1;
    }
    int blocks = 0;
    Block *tempBlock = startBlock;
    while (tempBlock != NULL) {
        blocks++;
        tempBlock = tempBlock->next;
    }
    return blocks;
}

// parameter: certain block
// return the last record stored in this block
Record *getLastRecord(Block *block) {
    Record *last = block->firstRecord;
    while (last->next != NULL) {
        last = last->next;
    }
    return last;
}

// insert specified record into certain block
void insertRecord(Record *record, Block *block) {
    if (isFull(block, record)) {
        printf("Block is full, unable to insert!\n");
        return;
    }

    record->header.blockID = block->header.id;
    block->header.remainSize = block->header.remainSize - record->header.size;

    if (block->firstRecord == NULL) { //first record in block
        block->firstRecord = record;
    } else {
        Record *lastRecord = getLastRecord(block);
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

int getLineNum() {
    FILE *fp;
    fp = fopen(PATH, "r");
    char ch;
    int lines = 0;

    while (!feof(fp)) {
        ch = fgetc(fp);
        if (ch == '\n') {
            lines++;
        }
    }
    return lines;
}