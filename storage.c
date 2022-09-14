#include <stdio.h>
#include <string.h>

#define MAX_LEN 256
const int BLOCK_SIZE = 200;

typedef struct Record {
    int size;
    char* id;
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
Record* createRecord(char* line, Record* record);
void deleteRecord();
Block* createBlock(Block* preBlock);
void deleteBlock();
void insertBlock(Record* record, Block* block);


Record* createRecord(char* line, Record* record) {

}

void deleteRecord() {

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
    FILE* fp;
    fp = fopen("data.tsv", "r");
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

    fclose(fp);
    return 0;
}