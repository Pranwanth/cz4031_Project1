#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256
const int BLOCK_SIZE = 200;

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

//added parameters char*id to know which record to delete and block* to specify where to deleted the record from
void deleteRecord(char*id, Block* block); 
Block* createBlock(Block* preBlock);
void deleteBlock();
void insertBlock(Record* record, Block* block);


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
        free(recPtr);
    }
}

Block* createBlock(Block* preBlock) {

}

void deleteBlock() {

}

// insert specified record into certain block
// will also check if the record can be inserted or not

void insertBlock(Record* record, Block* block) {

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
    

    while (fgets(buffer, MAX_LEN, fp))
    {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n", buffer);
    }
    
    
    
    /* 
    For testing createRecord
    
    for(int i = 0; i < 6; i ++){
        fgets(buffer, MAX_LEN, fp);
        buffer[strcspn(buffer, "\n")] = 0;
        if(i > 0)
        {
            Record* recPtr = (Record*) malloc(sizeof(Record)); //we have to malloc outside to prevent segementation fault
            createRecord(buffer, recPtr);
            printf("Record %d : %s %.2f %d size: %d \n", i, recPtr->id, recPtr->averageRating, recPtr->numVotes, recPtr->size);
            free(recPtr);
        }
    } */

    fclose(fp);
    return 0;
}