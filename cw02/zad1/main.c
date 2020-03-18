#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include "compare.h"

int main(int argc, char** argv)
{
//     const char *file = "data.txt";
//     generate(file, 10, 15);

//      quickSortLib("data.txt", 0, 8, 3);
//     return 0;

    srand(time(0));
    if(argc < 5)
    {
        printf("Too   few arguments\n");
        return 1;
    }

    char* command = argv[1];
    struct tms* startOfTms = (struct tms*)malloc(sizeof(struct tms));
    struct tms* endOfTms = (struct tms*)malloc(sizeof(struct tms));

    if(strcmp(command, "genreate") == 0)
    {
        if(argc < 5)
        {
            printf("Not enough argumentds\n");
            return 1;
        }
    

    char* f = argv[2];
    int numberOfRecords = atoi(argv[3]);
    int lengthOfRecord = atoi(argv[4]);

    int file = open(f, O_RDWR | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
    write(file, generate(numberOfRecords, lengthOfRecord), numberOfRecords * (lengthOfRecord + 1));
    close(file);
    }
    else if(strcmp(command, "sort") == 0)
    {
        if(argc < 6)
        {
            printf("Not enough arguments\n");
            return 1;
        }
        times(startOfTms);

        char* file = argv[2];
        int numberOfRecords = atoi(argv[3]);
        int lengthOfRecord = atoi(argv[4]);
        char* type = argv[5];

        if(strcmp(type, "sys") == 0)
        {
            int fp = open(file, O_RDWR);
            quickSortSys(fp, lengthOfRecord, 0, numberOfRecords - 1);
            close(fp);
        }
        else if (strcmp(type, "lib") == 0)
        {
            FILE* f  = fopen(file, "r+");
            quickSortLib(f, lengthOfRecord, 0, numberOfRecords - 1);
            fclose(f);
        }
        else
        {
            printf("Type error %s\n", type);
            return 1;
        }
        times(endOfTms); 
        showResult(startOfTms, endOfTms); 
    }

    //copy
    else if(strcmp(command, "copy") == 0)
    {
        if(argc < 7)
        {
            printf("Not enough arguments");
            return 1;
        }

        times(startOfTms);

        char* file1 = argv[2];
        char* file2 = argv[3];
        int numberOfRecords = atoi(argv[4]);
        int lengthOfRecord = atoi(argv[5]);
        char* type = argv[6];

        char* buffer = (char*)calloc(lengthOfRecord, sizeof(char));
        if(strcmp(type,"sys") == 0)
        {
            int fp1 = open(file1, O_RDONLY);
            int fp2 = open(file2, O_WRONLY | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);

            for(int i = 0; i< numberOfRecords; i++)
            {
               buffer = getRecordSys(fp1, lengthOfRecord, i);
               saveRecordSys(fp2,lengthOfRecord, i, buffer); 
            }
            close(fp1);
            close(fp2);
        }else if(strcmp(type, "lib") == 0)
        {
            FILE *f1 = fopen(file1,"r+");
            FILE *f2 = fopen(file2,"w");

            for(int i=0; i < numberOfRecords; i++)
            {
                buffer = getRecordLib(f1, lengthOfRecord, i);
                saveRecordLib(f2, numberOfRecords, i, buffer);

            }
            fclose(f1);
            fclose(f2);
        }else
        {
            printf("Wrong type operation: %s\n", type);
            return 1;
        }
        free(buffer);

        times(endOfTms);
        showResult(startOfTms, endOfTms);
    }else{
        printf("Wrong command: %s\n", command);
        return 1;
    }
  return 0;
 }

