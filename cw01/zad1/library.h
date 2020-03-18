#ifndef LIBRARY_H
#define LIBRARY_H
#pragma once
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

typedef struct 
{
    char* text;
}Operation;

typedef struct 
{
    int counter;
    Operation **operations;
}Block;

typedef struct
{
    int size;
    Block **arrayOfBlocks;
}MainArray;


struct twoFile{
    char *first;
    char *second;
};

void createTable(int size);
int getFileSize(FILE *file);
void makeListOfPair(char *files);
int compareTwoFilesShell(char *file1, char* file2);
void translateBufferToArray(char *buffer, Operation ***arr, int size);
int countBufferOperations(char *text);
int carryFile(Block **buffer);
int carryFileToArray(char* file);
int carryResultToArray();
void compareListOfTwoFiles();
int counterOfOperations(int index);
void removeBlock(int index);
void removeOperation(int indexOfBlock, int indexOfOperation);
char *returnOperation(int indexOfBlock, int indexOfOperation);
void removeArray();

#endif //LIBRARY_H