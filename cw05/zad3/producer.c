 
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

int main(int argc, char** argv)
{
    if(argc < 4)
        errorAndExit("To few arguments");

    srand(time(NULL));

    char* pipeName = (char*)calloc(MAX_LEN, sizeof(char));
    strcpy(pipeName, argv[1]);
    char* inFileName = (char*)calloc(MAX_LEN, sizeof(char));
    strcpy(inFileName, argv[2]);
    int N = atoi(argv[3]);

    FILE* inFile = fopen(inFileName, "r");
    FILE* pipe = fopen(pipeName, "w");
    char* buffer = (char*)calloc(N, sizeof(char));
    char* content = (char*)calloc(MAX_LEN, sizeof(char));
    pid_t pid = getpid();

    while(fread(buffer,1, N, inFile) > 0)
    {
        sleep(rand()%2 + 1);
        sprintf(content, "#%d#%s\n", pid, buffer);
        fwrite(content, 1, strlen(content), pipe);

        memset(buffer, 0, strlen(buffer));
        memset(content, 0, strlen(content));
    }
    free(content);
    fclose(inFile);
    fclose(pipe);
    free(inFileName);
    free(buffer);
    free(pipeName);
   
    return 0;

}