#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>


#define MAX_LEN 100


void errorAndExit(char* content){
    puts("Error: %s");
    exit(EXIT_FAILURE);

}

int main(int argc, char* argv[])
{
    if(argc < 4)
        errorAndExit("Too few arguments");

    
    char* pipeName = (char*)calloc(MAX_LEN, sizeof(char));
    strcpy(pipeName, argv[1]);
    char* outFileName = (char*)calloc(MAX_LEN, sizeof(char));
    strcpy(outFileName, argv[2]);
    int N = atoi(argv[3]);

    FILE* pipe = fopen(pipeName, "r");
    FILE* outFile = fopen(outFileName, "w");
    char* buffer = (char*)calloc(N, sizeof(char));

    while(fread(buffer, 1, N, pipe) > 0)
    {
        fwrite(buffer, 1, strlen(buffer), outFile);
        memset(buffer, 0, strlen(buffer));
    }
    free(buffer);
    fclose(outFile);
    fclose(pipe);
    free(pipeName);
    free(outFileName);
    return 0;

}