#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>



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

double differenceOfTime(clock_t t1, clock_t t2)
{
    return ((double)(t2 - t1)/sysconf(_SC_CLK_TCK));
}

void showResult(int numberOfRecords, int lengthOfRecord, char *command, char *type, struct tms* startOfTime, struct tms* endOfTime,FILE *resultFile)
{
    fprintf(resultFile, "Records: %d\t %d byte(s)\t %s\t %s\t SYSTEM: %f\tUSER: %f\n",
    numberOfRecords, lengthOfRecord, command, type, differenceOfTime(startOfTime->tms_utime, endOfTime->tms_utime), 
    differenceOfTime(startOfTime->tms_stime, endOfTime->tms_stime));
}


char* getRecordLib(FILE* f, int lengthOfRecord, int index)
{   

    fseek(f, (lengthOfRecord +1)*index, 0);
    char* record = (char*)calloc(lengthOfRecord, sizeof(char));

    fread(record, 1, lengthOfRecord, f);
    return record;
}

void saveRecordLib(FILE *f, int lengthOfRecord, int index, char* record)
{
    
    fseek(f,(lengthOfRecord+1)*index, 0);

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

void copyLib( char* file1, char* file2, int numberOfRecords, int lengthOfRecord)
{
    char* buffer = (char*)calloc(lengthOfRecord, sizeof(char));
    FILE *f1 = fopen(file1,"r+");
    FILE *f2 = fopen(file2,"w");

    for(int i=0; i < numberOfRecords; i++)
    {
        buffer = getRecordLib(f1, lengthOfRecord, i);
        saveRecordLib(f2, lengthOfRecord, i, buffer);

    }
    fclose(f1);
    fclose(f2);
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

void copySys( char* file1, char* file2, int numberOfRecords, int lengthOfRecord)
{
     char* buffer = (char*)calloc(lengthOfRecord, sizeof(char));
     int fp1 = open(file1, O_RDONLY);
    int fp2 = open(file2, O_WRONLY | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);

    for(int i = 0; i< numberOfRecords; i++)
    {
        buffer = getRecordSys(fp1, lengthOfRecord, i);
        saveRecordSys(fp2,lengthOfRecord, i, buffer); 
    }
    close(fp1);
    close(fp2);
}










int main(int argc, char** argv)
{


    srand(time(0));
    if(argc < 5)
    {
        printf("Too   few arguments\n");
        return 1;
    }

    char* command = argv[1];
    struct tms* startOfTms = (struct tms*)malloc(sizeof(struct tms));
    struct tms* endOfTms = (struct tms*)malloc(sizeof(struct tms));

    FILE *reportFile;
    char file_name[] = "wyniki.txt";
    reportFile = fopen(file_name, "a");
    

    if(strcmp(command, "generate") == 0)
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
        showResult(numberOfRecords, lengthOfRecord, command, type, startOfTms, endOfTms, reportFile); 
    }

    
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

       
        if(strcmp(type,"sys") == 0)
            copySys(file1, file2, numberOfRecords, lengthOfRecord);
            
        else if(strcmp(type, "lib") == 0)
            copyLib(file1, file2, numberOfRecords, lengthOfRecord);
            
        else
        {
            printf("Wrong type operation: %s\n", type);
            return 1;
        }
        times(endOfTms);
        showResult(numberOfRecords, lengthOfRecord, command, type, startOfTms, endOfTms,reportFile);
    }else{
        printf("Wrong command: %s\n", command);
        return 1;
    }
fclose(reportFile);
return 0;
}

