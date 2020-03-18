#ifndef COMPARE_H
#define COMPARE_H
#pragma once

int randomInt(int start, int end);
char* generate(int numberOfRecords, int lengthOfRecord);
double differenceOfTime(clock_t t1, clock_t t2);
void showResult(struct tms* startOfTime, struct tms* endOfTime);

char* getRecordLib(FILE* f, int lengthOfRecord, int index);
void saveRecordLib(FILE *f, int lengthOfRecord, int index, char* record);
void swapRecordsLib(FILE *f, int lengthOfRecord, int firstIndex, int secondIndex);
int partitionLib(FILE* f, int lengthOfRecord, int left, int right);
void quickSortLib(FILE *f, int lengthOfRecord, int left, int right);

char* getRecordSys(int f, int lengthOfRecord, int index);
void saveRecordSys(int file, int lengthOfRecord, int index, char* record);
void swapRecordsSys(int file, int lengthOfRecord, int firstIndex, int secondIndex);
int partitionSys(int file, int lengthOfRecord, int left, int right);
void quickSortSys(int file, int lengthOfRecord, int left, int right);

#endif //COMPARE_H 