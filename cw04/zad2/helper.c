#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void ifFailure(){exit(0);};

int main(int  argc, char *argv[])
{
    int sig = atoi(argv[2]);
    int parentPid = atoi(argv[3]);

    if(strcmp(argv[1], "-i") == 0)
    {
        raise(sig);
        kill(parentPid, SIGUSR2);
    }
    else if(strcmp(argv[1], "-m") == 0)
    {
        signal(sig, ifFailure);
        raise(sig);
        kill(parentPid, SIGUSR2);
    }
    else if(strcmp(argv[1], "-p") == 0)
    {
        sigset_t mask;
        sigpending(&mask);
        if(sigismember(&mask, sig))
            kill(parentPid, SIGUSR2);
    }
}

