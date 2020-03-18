#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include "library.h"

clock_t start, end;
struct tms startCpu, endCpu;
MainArray *mainArray;
FILE *raport;

void startTime()
{
    start = times(&startCpu);
}

void endTime()
{
    end = times(&endCpu);
}

void carryResultToFile(FILE *fp, char* operation)
{
    endTime();
    double realTime = (double)(end - start) / sysconf(_SC_CLK_TCK);
    double userTime = (double)(endCpu.tms_utime - startCpu.tms_utime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double)(endCpu.tms_stime - startCpu.tms_stime) / sysconf(_SC_CLK_TCK);
    fprintf(fp, "Operation: %s\n", operation);
    fprintf(fp, "Real time: %f\n", realTime);
    fprintf(fp, "User time: %f\n", userTime);
    fprintf(fp, "System time: %f\n\n", systemTime);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("You should have given more than 3 arguments");
        return -1;
    }
    if(strcmp(argv[1], "createTable") != 0)
    {
        printf("Firstly yoy should have given argument: createTable");
        return -1;
    }

    raport = fopen("raport.txt", "a");

    int numberOfBlocks = atoi(argv[2]);
    createTable(numberOfBlocks);
    int i = 3;
    char* functionCalled;
    while(i < argc){
        startTime();

       if(strcmp(argv[i], "compareListOfTwoFiles") == 0)
       {
            compareListOfTwoFiles(argv[i+1], mainArray);
            i +=2;
            functionCalled = "compareListOfTwoFiles";
       }
       else if(strcmp(argv[i],"removeOperation") == 0)
       {
           int indexOfBlock =atoi(argv[i+1]);
           int indexOfOperation = atoi(argv[i+2]);
           removeOperation(indexOfBlock, indexOfOperation);
           i += 3;
           functionCalled = "removeOperation";
       }
       else if(strcmp(argv[i],"removeBlock") == 0)
       {
           int indexOfBlock = atoi(argv[i+1]);
           removeBlock(indexOfBlock);
       }
       else
       {
           functionCalled = "wrong function called";
           i++;
       }
       carryResultToFile(raport, functionCalled);
    }

    removeArray(mainArray);
    return 0;
}