 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <memory.h>

volatile pid_t childGlob;
volatile int L, T, sendChild, getByChild, getFromChild;

void showSignals()
{
    printf("Sygnały wysłane do dziecka: %d\n", sendChild);
    printf("Sygnały odebrane przez dziecko: %d\n", getFromChild);
}

void handleParent(int sgn, siginfo_t *sigInfo, void *text)
{
    if(sgn == SIGINT)
    {
        printf("Rodzic odebrał SIGINT\n");
        kill(childGlob,SIGUSR2);
        showSignals();
        exit(9);
    }
    if(sigInfo->si_pid != childGlob)
        return;

    if((T == 1 || T == 2) && sgn == SIGUSR1)
    {
        getFromChild++;
        printf("Rodzic odebrał SIGRTMIN\n");
    }
}

void handleChild(int sgn, siginfo_t *sigInfo, void *text)
{
    if(sgn == SIGINT)
    {
        sigset_t mask;
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        printf("Liczba sygnałów otrzymanych przez dziecko: %d\n", getByChild);
        exit((unsigned)getByChild);
    }
    if(sigInfo->si_pid != getppid())
        return;
    
    if(T == 1 || T == 2)
    {
        if(sgn == SIGUSR1)
        {
            getByChild++;
            kill(getppid(), SIGUSR1);
            printf("dziecko odebrało SIGUSR1 \tdziecko wysłało SIGUSR1\n");

        }
        else if(sgn == SIGUSR2)
        {
            getByChild++;
            printf("Dziecko odebrało SIGUSR2\n");
            printf("Liczba odebranych sygnałów przez dziecko: %d \n", getByChild);
            exit((unsigned)getByChild);
        }
    }
    else if(T == 3)
    {
        if(sgn == SIGRTMIN)
        {
            getByChild++;
            kill(getppid(), SIGRTMIN);
            printf("dziecko odebrało SIGRTMIN \tdzieco wysłało SIGRTMIN\n");
        }
        else if(sgn == SIGRTMAX)
        {
            getByChild++;
            printf("Dziecko odebrało SIGRTMAX\n");
            printf("Liczba odebranych sygnałów przez dziecko: %d \n", getByChild);
            exit((unsigned)getByChild);
        }
    }
}

void parent()
{
    sleep(1);
    struct sigaction sigAction;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_SIGINFO;
    sigAction.sa_sigaction = handleParent;

    if(T == 1 || T == 2)
    {
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGINT);
        for(;sendChild < L; sendChild++)
        {
            printf("Rodzic wysłał SIGUSR1\n");
            kill(childGlob, SIGUSR1);
            if(T == 2)
                sigsuspend(&mask);
        }
        printf("rodzic wysłał SIGUSR2\n");
        kill(childGlob, SIGUSR2);
    }
    else if(T == 3)
    {
        for(;sendChild < L; sendChild++)
        {
            printf("Rodzic wysłał SIGRTMIN\n");
            kill(childGlob, SIGRTMIN);
        }
        sendChild++;
        printf("Rodzic wysłał SIGRTMAX\n");
        kill(childGlob, SIGRTMAX);

    }

    int status = 0;
    waitpid(childGlob, &status, 0);
    if(WIFEXITED(status))
        getByChild=WEXITSTATUS(status);
    else
        exit(1);
    
}

void child()
{
    struct sigaction sigAction;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_SIGINFO;
    sigAction.sa_sigaction = handleChild;

    if(sigaction(SIGINT, &sigAction, NULL) ==-1)
        exit(1);
    if(sigaction(SIGRTMIN, &sigAction, NULL) ==-1)
        exit(1);
    if(sigaction(SIGRTMAX, &sigAction, NULL) ==-1)
        exit(1);
    if(sigaction(SIGUSR1, &sigAction, NULL) ==-1)
        exit(1);
    if(sigaction(SIGUSR2, &sigAction, NULL) ==-1)
        exit(1);
    
    while(1)
        sleep(1);
}

int main(int argc, char *argv[])
{
    sendChild = 0;
    getByChild = 0;
    getFromChild = 0;

    L = (int)strtol(argv[1], '\0', 10);
    T = (int)strtol(argv[2], '\0', 10);

    childGlob = fork();

    if(!childGlob)
        child();
    else if(childGlob > 0)
        parent();

    showSignals();

    return 0;
}