
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

void errorAndExit(char* content){
    puts("Error: %s");
    exit(EXIT_FAILURE);

}

int main()
{
    if(mkfifo("pipe", S_IRWXU | S_IRWXG | S_IRWXO) < 0)
        errorAndExit("Problem with creating pipe");

    char inFileName[4];
    for(int i = 0; i < 5; i++)
    {
        sprintf(inFileName, "in%d", i);

        FILE* file = fopen(inFileName, "w");
        for(int j = 0; j < 25; j++)
        {
            char str[12];
            sprintf(str, "%d", i);
            fwrite(&str, 1, 1, file);
        }

        if(fork() == 0)
            execl("./producer","./producer", "pipe", inFileName,"5", NULL);
    }
    if(fork() == 0)
        execl("./consumer", "./consumer", "pipe","outFile", "5", NULL);


}