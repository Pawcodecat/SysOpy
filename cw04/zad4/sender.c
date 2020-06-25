#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>


bool wait = true;
pid_t pid;

int counter = 0;
union sigval sigVal = {.sival_ptr = NULL};

void ifSigUsr1(){counter ++;}
void ifSigUsr2(){wait = false;}

int main(int argc, char **argv){

    pid = atoi(argv[1]);
    int all = atoi(argv[2]);

    bool withSigqueue = false;
    bool withKill = false;
    bool withSigrt = false;
   
    if(strcmp(argv[3], "sigueue") == 0)
        withSigqueue = true;
    else if(strcmp(argv[3], "kill") == 0)
        withKill = true;
    else if(strcmp(argv[3], "sigrt") == 0)
        withSigrt = true;

    int S1, S2;

    if(withSigrt)
        S1 = SIGRTMIN;
    else
        S1 = SIGUSR1;

    if(withSigrt)
        S2 = S1 +1;
    else
        S2 = SIGUSR2;

    signal(S1,ifSigUsr1);
    signal(S2, ifSigUsr2);
    
    if(withKill || withSigrt)
    {
        for(int i=0; i < all; i++)
            kill(pid, S1);
        kill(pid, S2);
    }
    else if(withSigqueue)
    {
        for(int i=0; i < all; i++)
            sigqueue(pid, S1,sigVal);
        sigqueue(pid, S2, sigVal);
        
    }
        
    wait = true;
    while(wait);

    printf("In sender is/a: %d\n", counter);
    
   
    
}



