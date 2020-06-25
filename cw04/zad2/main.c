#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <wait.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef WAIT_ANY
#define WAIT_ANY -1
#endif


static int status;

bool parentSignal, childSignal;
void setParentSignal() {parentSignal = true;}
void setChildSignal() {childSignal= true;}

char *convertToChar(int number)
{
    static char tmp[16];
    sprintf(tmp, "%d", number);
    return strdup(tmp);
}

int main()
{
    signal(SIGUSR1, setParentSignal);
    signal(SIGUSR2, setChildSignal);
    
    
   
    for(int i = 1; i <= 22; i++)
    {
        char *stringSig = convertToChar(i);
        printf("%d\t",i);
        static char *forkArr[4] = {"i", "h", "m", "p"};
        for(int i = 0; i < 4; i++)
        {

            if(strcmp(forkArr[i], "i") == 0)
                puts("ignore");
            else if(strcmp(forkArr[i], "m") == 0)
                puts("mask");
            else if(strcmp(forkArr[i], "h") == 0)
                puts("handle");
            else 
                puts("pending");
            childSignal = false;
            parentSignal = false;
            if(fork() == 0)
                execl("./compare", "./compare", "-d",stringSig, forkArr[i], NULL);
            waitpid(WAIT_ANY, &status, WUNTRACED);
            if(childSignal)
                printf("childSignal: True\t");
            else
                printf("childSignal: False\t");

              if(parentSignal)
                printf("parentSignal: True\t");
            else
                printf("parentSignal: False\t");     
        }
        puts("");

    }
    puts("");

    puts("Exec");
   
    for(int i = 1; i <= 22; i++)
    {
        char *stringSig = convertToChar(i);
        printf("%d\t",i);
        static char *execArr[4] = {"i", "m", "p"};
        for(int i = 0; i < 3; i++)
        {
            if(strcmp(execArr[i], "i") == 0)
                puts("ignore");
            else if(strcmp(execArr[i], "m") == 0)
                puts("mask");
            else 
                puts("pending");
            childSignal = false;
            parentSignal = false;
            if(fork() == 0)
                execl("./compare", "./compare", "-d", stringSig, execArr[i], NULL);
            waitpid(WAIT_ANY, &status, WUNTRACED);
            if(childSignal)
                printf("childSignal: True\t");
            else
                printf("childSignal: False\t");

              if(parentSignal)
                printf("parentSignal: True\t");
            else
                printf("parentSignal: False\t");    
            puts(""); 
        }
        puts("");

    }
}
