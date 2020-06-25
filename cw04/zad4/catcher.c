#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>


bool wait = true;
pid_t pid;

int counter = 0;
void ifSigusr1(){counter++;}
void ifSigusr2(int signal, siginfo_t *sigInfo, void* notUse)
{
    printf("In catcher is/ are %d \n", counter);
    pid = sigInfo->si_pid;
    wait = false;
}

union sigval sigVal = {.sival_ptr = NULL};


int main(int argc, char **argv){


    bool withSigqueue = false;
    bool withKill = false;
    bool withSigrt = false;
   
    if(strcmp(argv[3], "sigqueue") == 0)
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

    signal(S1,ifSigusr1);

    struct sigaction sigAction;
    sigAction.sa_flags = SA_SIGINFO;
    sigAction.sa_sigaction = ifSigusr2;
    sigaction(S2, &sigAction, NULL);

    while(wait);

    printf("Pid: %d\n", getpid());
    if(withKill || withSigrt)
    {
        for(int i=0; i < counter; i++)
            kill(pid, S1);
        kill(pid, S2);
    }
    else if(withSigqueue)
    {
        for(int i=0; i < counter; i++)
            sigqueue(pid, S1,sigVal);
        sigqueue(pid, S2, sigVal);
        
    }
}