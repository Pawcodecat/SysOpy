#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>


MainArray *mainArray;
char *result = "result.txt";
char *listOfPair = NULL;

void createTable(int size){
    mainArray = (MainArray*)calloc(1,sizeof(MainArray));
    mainArray-> size = size;
    mainArray->arrayOfBlocks = (Block**)calloc(size, sizeof(Block*));
    

}

int getFileSize(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}

void makeListOfPair(char *files)
{
    listOfPair = files;
}
 
int compareTwoFilesShell(char *file1, char *file2)
 {
     char command[256];
     snprintf(command, sizeof(command), "diff %s %s >>%s", file1, file2, result);
     int resultOfComparison = system(command);
     return resultOfComparison;
 }

 void translateBufferToArray(char *buffer, Operation ***arr, int size)
 {
    char *stack[size];
    char *f = buffer;
    int i = 0;
    stack[i] = f;

    while(*f != '\0')
    {
        if(*f == '\n' && (*(++f) > '0' && *(f) < '9'))
        {
            i++;
            stack[i] = f;
        }
        f++;
    }

    for(int j = 0; j < size; j++)
    {
        if(j == size -1)
        {
            (*arr)[j]->text = (char*)calloc(strlen(stack[j])+1, sizeof(char));
            memcpy((*arr)[j]->text, stack[j], strlen(stack[j]));
        }
        (*arr)[j]->text = (char*)calloc(strlen(stack[j])-strlen(stack[j+1]) + 1, sizeof(char));
        memcpy((*arr)[j]->text, stack[j], strlen(stack[j]) - strlen(stack[j+1]));
    }
 }

 int countBufferOperations(char *text){
     
     bool flag = false;
     int counter = 0;
     char *f;
     f = text;

     while(*f != '\0')
     {
         flag = true;
         if(*f == '\n' && (*(++f) > '0' && *(f) < '9'))
            counter++;
        f++;
     }
    int  totalCounter = 0;
    if(flag == true)
        totalCounter = counter + 1;
    return totalCounter;
 }

 int carryFile(Block **buffer)
 {
     char *tmpbuffer = NULL;
     FILE *file = fopen(result, "rb");
     if(!file)
     {
         fprintf(stdout, "Could not open file");
         return -1;
     }
     else
     {
         long sizeOfFile = getFileSize(file);
         tmpbuffer = (char*)calloc(sizeOfFile, sizeof(char));
         fread(tmpbuffer, sizeof(char), sizeOfFile, file);
         fclose(file);

         int counter = countBufferOperations(tmpbuffer);
         (*buffer) = (Block*)calloc(1, sizeof(Block));
        (*buffer)->counter = counter;
        (*buffer)->operations = (Operation**)calloc(counter, sizeof(Operation *));

         for(int j=0; j<counter; j++)
         {
             (*buffer)->operations[j] = (Operation*)calloc(1, sizeof(Operation));
         }
    translateBufferToArray(tmpbuffer, &((*buffer)->operations), counter);
    return 0;
     }
     
 }

 int carryFileToArray(char* file)
 {
     for(int i = 0; i < mainArray->size; i++)
     {
         if(mainArray->arrayOfBlocks[i] == NULL)
         {
             int ifLoaded = carryFile(&mainArray->arrayOfBlocks[i]);
             if(ifLoaded < 0){
                 fprintf(stdout, "file wasn't loaded");
                 return ifLoaded;
             }
             return i;
         }
     }
     fprintf(stdout, "array overflow, file wasn't loaded");
     return -1;
 }

 int carryResultToArray()
 {
     return carryFileToArray(result);
 }

void compareTwoFiles(char *twoFiles)
{
    char *text = strdup(twoFiles);

    char *file1 = strtok(text, ":");            //separate two files
    char *file2 = strtok(NULL, "");

    compareTwoFilesShell(file1, file2);
    carryResultToArray();


}


 void compareListOfTwoFiles()
 {
     char *first = strdup(listOfPair);

     char *chosenFileFirst = strtok(first, " ");
     int counter = 0;
     while(chosenFileFirst != NULL)
     {
         chosenFileFirst = strtok(NULL, " ");
         counter++;
     }

     char *listOfTwoFiles[counter];
     int i = 0;
     char *second = strdup(listOfPair);

     char * chosenFileSecond = strtok(second, " ");
     while (chosenFileSecond != NULL)
     {
         listOfTwoFiles[i] = chosenFileSecond;
         i++;
         chosenFileSecond = strtok(NULL, " ");
     }

    for(int i=0; i<counter; i++)
    {
        compareTwoFiles(listOfTwoFiles[i]);
    }

 }

 int counterOfOperations(int index)
 {
     return mainArray->arrayOfBlocks[index] == NULL ? 0 : mainArray->arrayOfBlocks[index]->counter;
 }

 void removeBlock(int index)
 {
     if(mainArray->arrayOfBlocks == NULL)
        return;

    for(int i = 0; i < mainArray->arrayOfBlocks[index]->counter; i++ )
    {
        free(mainArray->arrayOfBlocks[index]->operations);
        mainArray->arrayOfBlocks[index]->operations = NULL;
    }
    free(mainArray->arrayOfBlocks[index]->operations);
    mainArray->arrayOfBlocks[index]->operations = NULL;
    free(mainArray->arrayOfBlocks[index]);
    mainArray->arrayOfBlocks[index] = NULL;

 }

 void removeOperation(int indexOfBlock, int indexOfOperation)
 {
     if(mainArray->arrayOfBlocks[indexOfBlock] == NULL)
     {
         fprintf(stdout, "mainArray has not block with this index");
         return;
     }

     if(mainArray->arrayOfBlocks[indexOfBlock]->operations[indexOfOperation] == NULL)
     {
         fprintf(stdout, "mainArray has not operations with this index");
         return;
     }

     free(mainArray->arrayOfBlocks[indexOfBlock]->operations[indexOfOperation]);
     mainArray->arrayOfBlocks[indexOfBlock]->operations[indexOfOperation] = NULL;
     int counter = mainArray->arrayOfBlocks[indexOfBlock]->counter;
     if (counter == 1)                                                      ////////////////////
     {
         removeBlock(indexOfBlock);
     }                                                              
     else
     {
        mainArray->arrayOfBlocks[indexOfBlock]->counter -= 1;                                     ///////////////////////////////
     }
}    
    Block *returnBlock(int index)
    {
        if(mainArray->arrayOfBlocks == NULL)
        {
            fprintf(stdout, "Array has not block with this index");
        }

        return mainArray->arrayOfBlocks[index];
    }

char *returnOperation(int indexOfBlock, int indexOfOperation)
{
    if(mainArray->arrayOfBlocks[indexOfBlock] == NULL)
    {
            fprintf(stdout, "Array has not block with this index");   
    }

    if(mainArray->arrayOfBlocks[indexOfBlock]->operations[indexOfOperation] == NULL)
    {
        fprintf(stdout, "mainArray has not operations with this index");
        
    }
    return mainArray->arrayOfBlocks[indexOfBlock]->operations[indexOfOperation]->text;

}
 
void removeArray()
{
    for(int i = 0; i < mainArray->size; i++)
    {
        removeBlock(i);
    }
    free(mainArray->arrayOfBlocks);
    free(mainArray);
}

// int main(){
    
//     makeListOfPair("txt/a.txt:txt/b.txt txt/c.txt:txt/d.txt txt/a.txt:txt/c.txt txt/e.txt:txt/f.txt txt/f.txt:txt/c.txt");
//     createTable(20);
//     compareListOfTwoFiles();
//     carryResultToArray();

//     // printf("%s", mainArray->arrayOfBlocks[0]->operations[2]->text);
//     removeBlock(0);
//     // printf("%s", mainArray->arrayOfBlocks[2]->operations[0]->text);
//     compareListOfTwoFiles();
//     carryResultToArray();
//     removeOperation(0, 0);
//     removeOperation(0, 0);
//     removeOperation(0, 23);
//     removeOperation(0, 3);
//     carryResultToArray();

//     compareListOfTwoFiles();
//     // printf("%d\n", counterOfOperations(0));

    

//     // printf("\n\n\n\n\n%s\n\n", returnOperation(0, 1));
//     removeArray();
  
    
//     return 0;
// }