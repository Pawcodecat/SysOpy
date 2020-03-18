#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include "compare.h"

int randomInt(int start, int end)
{
    return rand()%(end - start + 1) + start;
}

char* generate(int numberOfRecords, int lengthOfRecord)
{
    srand(time(0));
    
    int numberOfCharaccters = numberOfRecords * (lengthOfRecord +1);

    char* records = (char*)calloc(numberOfCharaccters, sizeof(char));

    for(int i=0; i < numberOfCharaccters; i++)
    {
        if(i%(lengthOfRecord + 1) == lengthOfRecord)
        {
            records[i] = '\n';
        }
        else
        {
            records[i] = (char)randomInt('a', 'z');
        }
        
    } 
    return records;
}



char* getRecordLib(FILE* f, int lengthOfRecord, int index)
{
    fseek(f, (lengthOfRecord + 1) * index, 0);
    char* record = (char*)calloc(lengthOfRecord, sizeof(char));

    fread(record, 1, lengthOfRecord, f);
    return record;
}

void saveRecordLib(FILE *f, int lengthOfRecord, int index, char* record)
{
    fseek(f,(lengthOfRecord +1) * index, 0);

    fwrite(record, 1, lengthOfRecord, f);
    fwrite("\n", 1, 1, f);
}

void swapRecordsLib(FILE *f, int lengthOfRecord, int firstIndex, int secondIndex)
{
    char *record1 = (char*)calloc(lengthOfRecord, sizeof(char));
    char *record2 = (char*)calloc(lengthOfRecord, sizeof(char));

    fseek(f,(lengthOfRecord +1) * firstIndex, 0);
    fread(record1, 1, lengthOfRecord, f);

    fseek(f,(lengthOfRecord +1) * secondIndex, 0);
    fread(record2, 1, lengthOfRecord, f);

    fseek(f,(lengthOfRecord +1) * firstIndex, 0);
    fwrite(record2, 1, lengthOfRecord, f);

    fseek(f,(lengthOfRecord +1) * secondIndex, 0);
    fwrite(record1, 1, lengthOfRecord, f);

    free(record1);
    free(record2);
}

int partitionLib(FILE* f, int lengthOfRecord, int left, int right)
{
    char* pivot = getRecordLib(f, lengthOfRecord, right);
    int i = left - 1;

    for(int j = left; j < right; j++)
    {
        char* compared = getRecordLib(f, lengthOfRecord, j);
        if(strcmp(compared, pivot) < 0)
        {
            i++;
            swapRecordsLib(f, lengthOfRecord, i, j);
        }
        free(compared);
    }
    swapRecordsLib(f, lengthOfRecord, i+1, right);
    free(pivot);

    return i+1;
}

void quickSortLib(FILE *f, int lengthOfRecord, int left, int right)
{
    if(left < right)
    {
        int q = partitionLib(f, lengthOfRecord, left, right);
        quickSortLib(f, lengthOfRecord, left, q-1 );
        quickSortLib(f, lengthOfRecord, q+1, right);
    }
}






char* getRecordSys(int f, int lengthOfRecord, int index)
{
    lseek(f, (lengthOfRecord + 1) * index, 0);
    char* record = (char*)calloc(lengthOfRecord, sizeof(char));

    if(read(f, record, lengthOfRecord) > 0)
        return record;

    return NULL;
}

void saveRecordSys(int file, int lengthOfRecord, int index, char* record)
{
    lseek(file,(lengthOfRecord +1) * index, 0);

    write(file, record, lengthOfRecord);
    write(file, "\n", 1);
}

void swapRecordsSys(int file, int lengthOfRecord, int firstIndex, int secondIndex)
{
    char *record1 = (char*)calloc(lengthOfRecord, sizeof(char));
    char *record2 = (char*)calloc(lengthOfRecord, sizeof(char));

    lseek(file,(lengthOfRecord +1) * firstIndex, 0);
    read(file, record1, lengthOfRecord);

    lseek(file, (lengthOfRecord +1) * secondIndex, 0);
    read(file, record2, lengthOfRecord);

    lseek(file,(lengthOfRecord +1) * firstIndex, 0);
    write(file, record2, lengthOfRecord);

    lseek(file,(lengthOfRecord +1) * secondIndex, 0);
    write(file, record1, lengthOfRecord);

    free(record1);
    free(record2);
}

int partitionSys(int file, int lengthOfRecord, int left, int right)
{
    char* pivot = getRecordSys(file, lengthOfRecord, right);
    int i = left - 1;

    for(int j = left; j < right; j++)
    {
        char* compared = getRecordSys(file, lengthOfRecord, j);
        if(strcmp(compared, pivot) < 0)
        {
            i++;
            swapRecordsSys(file, lengthOfRecord, i, j);
        }
        free(compared);
    }
    swapRecordsSys(file, lengthOfRecord, i+1, right);
    free(pivot);

    return i+1;
}

void quickSortSys(int file, int lengthOfRecord, int left, int right)
{
    if(left < right)
    {
        int q = partitionSys(file, lengthOfRecord, left, right);
        quickSortSys(file, lengthOfRecord, left, q-1 );
        quickSortSys(file, lengthOfRecord, q+1, right);
    }
}

double differenceOfTime(clock_t t1, clock_t t2)
{
    return ((double)(t2 - t1)/sysconf(_SC_CLK_TCK));
}

void showResult(struct tms* startOfTime, struct tms* endOfTime)
{
    printf("\tSYSTEM: %f\tUSER: %f\n", differenceOfTime(startOfTime->tms_utime, endOfTime->tms_utime), 
    differenceOfTime(startOfTime->tms_stime, endOfTime->tms_stime));
}