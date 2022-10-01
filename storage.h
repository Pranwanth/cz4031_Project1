#ifndef CZ4031_PROJECT1_STORAGE_H
#define CZ4031_PROJECT1_STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_LEN 2560
static char PBSTR[] = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
static int PBWIDTH = 60;
static int DISK_SIZE = 100000;
static int BLOCK_SIZE = 500; // 200
static char *DATA_PATH = "data.tsv"; // TODO configure

struct RecordHeader {
    int size;
    int blockID;
};

struct Record {
    struct RecordHeader header;
    char id[10]; //Changed this from char* to char[] so the size of the record is always fixed.
    float averageRating;
    int numVotes;
    Record *next;
};

struct BlockHeader {
    int id;
    int capacity;
    int remainSize;
    int recordSize;
};

struct Block {
    struct BlockHeader header;
    Record *firstRecord;
    struct Block *next;
};

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
