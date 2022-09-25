#ifndef CZ4031_PROJECT1_STORAGE_H
#define CZ4031_PROJECT1_STORAGE_H

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
const char *PATH = "./data.tsv";

typedef struct RecordHeader {
    int size;
    int blockID;
} RecordHeader;

typedef struct Record {
    struct RecordHeader header;
    char id[11]; //Changed this from char* to char[] so the size of the record is always fixed.
    float averageRating;
    int numVotes;
    struct Record *next;
} Record;

typedef struct BlockHeader {
    int id;
    int capacity;
    int remainSize;
} BlockHeader;

typedef struct Block {
    struct BlockHeader header;
    Record *firstRecord;
    struct Block *next;
} Block;

void createRecord(char *line, Record *record);

void deleteRecord(char *id, Block *block);

void printRecord(Record *record);

int isFull(Block *block, Record *record);

Block *createBlock(Block *preBlock);

void deleteBlock(Block **firstBlock, int blockID);

void printBlock(Block *block);

Block *getBlock(Block *startBlock, int id);

Record *getLastRecord(Block *block);

void insertRecord(Record *record, Block *block);

int getBlockNum(Block *startBlock);

int getLineNum();

// extra method for better display
void printProgress(double percentage);

#endif //CZ4031_PROJECT1_STORAGE_H
