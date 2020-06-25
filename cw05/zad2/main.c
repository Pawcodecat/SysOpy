#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void errorAndExit(char* content){
    puts("Error: %s");

}

int main(int argc, char**argv)
{
    if(argc < 2)
        errorAndExit("Too few arguments");
    
    FILE* file = fopen(argv[1], "r");

    char* row = (char*)calloc(500, sizeof(char));
    FILE* sortInput = popen("sort", "w");
    while(fgets(row, 500, file)){
        fputs(row, sortInput);
    }
    pclose(sortInput);
    free(row);

    return 0;
    
}