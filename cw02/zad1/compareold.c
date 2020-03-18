 
#include "compare.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>


void generate(const char *file, int numberOfRecords, int lengthOfRecord)
{
    char command[256];
    snprintf(command, sizeof command,"head -c %d /dev/urandom > %s", numberOfRecords * lengthOfRecord, file);
    int findStatus = system(command);
    if(findStatus != 0)
    {
        fprintf(stderr, "Problem with generation data: %s\n", strerror(errno));
        exit(-1);
    }
}

void quickSortLib(const char *fileName, int startRecord,int endRecord, int lengthOfRecord)
{
    // FILE *fp = fopen(fileName, "r+");

    if(startRecord < endRecord)
    {
        int q = partitionLib(fileName, startRecord, endRecord, lengthOfRecord);
        quickSortLib(fileName, startRecord, q - 1, lengthOfRecord);
        quickSortLib(fileName, q + 1, endRecord, lengthOfRecord);
    }


}


// int  partitionLib(const char *fileName, int startRecord, int endRecord, int lengthOfRecord)
// {
//     FILE *fp = fopen(fileName, "r+");
//     char *pivot = (char*)malloc(lengthOfRecord);

//     if(fseek(fp, endRecord * lengthOfRecord, SEEK_SET) < 0)
//         fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//     if(fread(pivot, 1, lengthOfRecord, fp) != lengthOfRecord)
//         fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));


//     char *comparedRecord = (char*)malloc(lengthOfRecord);
//     char *firstGreaterRecord= (char*)malloc(lengthOfRecord);
//     int i = startRecord -1;
//     int j = startRecord;
//         for(j; j < endRecord; j++)
//         {
//             if(fseek(fp,j * lengthOfRecord, SEEK_SET) < 0)
//                 fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//             if(fread(comparedRecord, 1, lengthOfRecord, fp) != lengthOfRecord)
//                 fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));

//             if(comparedRecord[0] < pivot[0])
//             {
//                 i+=1;
//                 if(fseek(fp,i * lengthOfRecord, SEEK_SET) < 0)
//                     fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//                 if(fread(firstGreaterRecord, 1, lengthOfRecord, fp) != lengthOfRecord)
//                     fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));

//                 //swap
//                 if(fseek(fp,i * lengthOfRecord, SEEK_SET) < 0)
//                     fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//                 if(fwrite(comparedRecord, 1, lengthOfRecord, fp) != lengthOfRecord)
//                     fprintf(stderr, "Problem with write in quickSortLib function: %s\n", strerror(errno));

//                 if(fseek(fp,j * lengthOfRecord, SEEK_SET) < 0)
//                     fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//                 if(fwrite(firstGreaterRecord, 1, lengthOfRecord, fp) != lengthOfRecord)
//                     fprintf(stderr, "Problem with write in quickSortLib function: %s\n", strerror(errno));

//             }
//         }
//         i ++;
        
//         if(fseek(fp,i * lengthOfRecord, SEEK_SET) < 0)
//             fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//         if(fread(firstGreaterRecord, 1, lengthOfRecord, fp) != lengthOfRecord)
//             fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));

        

//         //swap
//         if(fseek(fp,i * lengthOfRecord, SEEK_SET) < 0)
//             fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//         if(fwrite(pivot, 1, lengthOfRecord, fp) != lengthOfRecord)
//             fprintf(stderr, "Problem with write in quickSortLib function: %s\n", strerror(errno));

//         if(fseek(fp,endRecord * lengthOfRecord, SEEK_SET) < 0)
//             fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

//         if(fwrite(firstGreaterRecord, 1, lengthOfRecord, fp) != lengthOfRecord)
//             fprintf(stderr, "Problem with write in quickSortLib function: %s\n", strerror(errno));
        
//         free(pivot);
//         free(firstGreaterRecord);
//         free(comparedRecord);
        
//         return i;

// }

int  partitionLib(const char *fileName, int startRecord, int endRecord, int lengthOfRecord)
{
    FILE *fp = fopen(fileName, "r+");
    char *pivot = (char*)malloc(lengthOfRecord);

    if(fseek(fp, startRecord * lengthOfRecord, SEEK_SET) < 0)
        fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

    if(fread(pivot, 1, lengthOfRecord, fp) != lengthOfRecord)
        fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));


    char *comparedRecordLeft = (char*)malloc(lengthOfRecord);
    char *comparedRecordRight= (char*)malloc(lengthOfRecord);
    int i = startRecord -1;
    int j = endRecord + 1;
    while(1)
    {
        do
        {
            j--;
            if(fseek(fp, j * lengthOfRecord, SEEK_SET) < 0)
                fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

            if(fread(comparedRecordRight, 1, lengthOfRecord, fp) != lengthOfRecord)
                fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));


        } while (comparedRecordRight[0] > pivot[0]);
        do
        {
            i++;
            if(fseek(fp, i * lengthOfRecord, SEEK_SET) < 0)
                fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

            if(fread(comparedRecordLeft, 1, lengthOfRecord, fp) != lengthOfRecord)
                fprintf(stderr, "Problem with read in quickSortLib function: %s\n", strerror(errno));


        } while (comparedRecordLeft[0] < pivot[0]);

        if(i < j)
        {
            if(fseek(fp,i * lengthOfRecord, SEEK_SET) < 0)
                fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

            if(fwrite(comparedRecordRight, 1, lengthOfRecord, fp) != lengthOfRecord)
                fprintf(stderr, "Problem with write in quickSortLib function: %s\n", strerror(errno));

            if(fseek(fp,j * lengthOfRecord, SEEK_SET) < 0)
                fprintf(stderr, "Problem with seek in quickSortLib function: %s\n", strerror(errno));

            if(fwrite(comparedRecordLeft, 1, lengthOfRecord, fp) != lengthOfRecord)
                fprintf(stderr, "Problem with write in quickSortLib function: %s\n", strerror(errno));

        }
        else 
            return j;
        
    }
}