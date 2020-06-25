#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>



#define MAX_ARG_LEN 50
#define MAX_ARGS  8
#define MAX_COMS_IN_LINE 10
#define MAX_LINE_LEN 10000

void errorAndExit(char* content){
    puts("Error: %s");
    exit(EXIT_FAILURE);

}

int main(int argc, char** argv){
    if(argc < 2)
    errorAndExit("To few arguments");

    FILE* file = fopen(argv[1], "r");

    char* row = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    while(fgets(row, MAX_LINE_LEN, file) != NULL)
    {
        row[strcspn(row, "\n")] = 0;  //change \n into 0

        char*** commands = (char***)calloc(MAX_COMS_IN_LINE, sizeof(char**));

        char* endCommand;
        char* command = strtok_r(row, "|", &endCommand);
        int cmdIdx = 0;
        while(command != NULL)
        {
            char* endArg;
            char* arg = strtok_r(command, " ", &endArg);
            int argIdx = 0;
            commands[cmdIdx] = (char**)calloc(MAX_ARGS, sizeof(char*));
            while(arg != NULL)
            {
                commands[cmdIdx][argIdx] = (char*)calloc(MAX_ARG_LEN, sizeof(char));
                strcpy(commands[cmdIdx][argIdx], arg);
                arg = strtok_r(NULL," ", &endArg);
                argIdx++;
            }
            command = strtok_r(NULL, "|", &endCommand);
            cmdIdx++;
        }
        pid_t* pids = (pid_t*)calloc(MAX_COMS_IN_LINE, sizeof(pid_t));
        
        int fd[2];
        pipe(fd);

        pid_t childPid = fork();
        if(childPid == 0)
        {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            execvp(commands[0][0], commands[0]);

        }
        pids[0] = childPid;
        
        int fdBefore[2];
        fdBefore[0] = fd[0];
        fdBefore[1] = fd[1];

        int i = 1;
        while(commands[i] != NULL)
        {
            fdBefore[0] = fd[0];
            fdBefore[1] = fd[1];
            pipe(fd);

            pid_t childPid = fork();

            if(childPid == 0)
            {
                dup2(fdBefore[0], STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);
                close(fdBefore[0]);
                close(fdBefore[1]);
                close(fd[0]);
                close(fd[1]);
                execvp(commands[i][0], commands[i]);
            }
            close(fdBefore[0]);
            close(fdBefore[1]);
            pids[i] = childPid;

            i++;
        }

        for(int j = 0; j < i; j++)
            waitpid(pids[j], NULL, 0);
        close(fd[1]);
        close(fd[0]);
        free(pids);

        int k = 0;
        while(commands[k] != NULL)
        {
            int l = 0;
            while(commands[k][l] != NULL)
            {
                free(commands[k][l]);
                l++;

            }
            free(commands[k]);
            k++;
        }
        free(commands);
    } 
    free(row);
    fclose(file);
    return 0;
}