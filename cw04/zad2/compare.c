#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdbool.h>
#ifndef WAIT_ANY
#define WAIT_ANY -1
#endif

#define _POSIX_C_SOURCE 200809L
bool execHelper = false;
static int sig;
static int parentPid;

char *convertToChar(int number)
{
    static char tmp[16];
    sprintf(tmp, "%d", number);
    return strdup(tmp);
}

int passSig = SIGUSR2;
void ifFail() {exit(0);}
void ifSignal() {kill(parentPid, passSig);}

void handleSignal()
{
    signal(sig,ifSignal);
    if(fork() == 0)
        raise(sig);
    waitpid(WAIT_ANY, NULL, WUNTRACED);
    passSig = SIGUSR1;
    raise(sig);
}

void ignoreSignal()
{
    signal(sig, SIG_IGN);
    if(fork() == 0)
    {
        if(execHelper)
            execl("./halper","./helper","i", convertToChar(sig), convertToChar(parentPid), NULL);
        raise(sig);
        kill(parentPid, SIGUSR2);
    }
    waitpid(WAIT_ANY, NULL, WUNTRACED);
    raise(sig);
    kill(parentPid, SIGUSR1);
}

void pendingSignal()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    raise(sig);
    sigset_t oldmask;
    if(fork() == 0)
    {
        if(execHelper)
            execl("./helper", "./helper", "p", convertToChar(sig), convertToChar(parentPid), NULL);
        sigpending(&oldmask);
        if(sigismember(&oldmask, sig))
            kill(parentPid, SIGUSR2);
    }
    waitpid(WAIT_ANY, NULL, WUNTRACED);
    sigpending(&oldmask);
    if(sigismember(&oldmask, sig))
        kill(parentPid, SIGUSR1);
}

void maskSignal()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    signal(sig, ifFail);
    if(fork() == 0)
    {
        if(execHelper)
            execl("./helper", "./helper", "p", convertToChar(sig), convertToChar(parentPid), NULL);
        raise(sig);
        kill(parentPid, SIGUSR2);
    }
    waitpid(WAIT_ANY, NULL, WUNTRACED);
    raise(sig);
    kill(parentPid, SIGUSR1);
}

int main(int argc, char const *argv[])
{
    parentPid = getppid();
    if (strcmp(argv[1], "-d") == 0)
    {
        execHelper = true;
        sig = atoi(argv[2]);
        if (strcmp(argv[3], "i") == 0)
            ignoreSignal();
        else if (strcmp(argv[3], "m") == 0)
            maskSignal();
        else if (strcmp(argv[3], "p") == 0)
            pendingSignal();
        else if (strcmp(argv[3], "h") == 0)
            handleSignal();
    }
    else
    {
        sig = atoi(argv[1]);
        if (strcmp(argv[2], "i") == 0)
            ignoreSignal();
        else if (strcmp(argv[2], "m") == 0)
            maskSignal();
        else if (strcmp(argv[2], "p") == 0)
            pendingSignal();
        else if (strcmp(argv[2], "h") == 0)
            handleSignal();
    }
}