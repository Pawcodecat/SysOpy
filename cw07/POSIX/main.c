 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "config.h"

#define W1 3
#define W2 3
#define W3 3
pid_t pids[W1 + W2 + W3];

void clear()
{
    for(int i = 0; i < 6; i++)
        if(sem_unlink(SEMS[i]) == -1)
            errorAndExit("Problem with deleting semaphore");
    
    if(shm_unlink("/SHARED_MEMORY"))
        errorAndExit("Problem with deleting shared memory");
    
    system("make clean");
}

void handleSIGINTpids(int sgn)
{
    puts("termination of action");
    for(int i = 0; i < W1 + W2 + W3; i++)
        kill(pids[i], SIGINT);
    
    clear();
    exit(0);
}

void generateSems()
{
    sem_t *sem = sem_open(SEMS[0], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if(sem == SEM_FAILED)
        errorAndExit("Problem with generating first semaphore");

    sem_close(sem);

    for(int i = 1; i < 6; i++)
    {
        sem = sem_open(SEMS[i], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
        if(sem == SEM_FAILED)
            errorAndExit("Problem with generatin semaphore");

        sem_close(sem);

    }
}

void generateShm()
{
    int fd = shm_open("/SHARED_MEMORY", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    
    if(fd == -1)
        errorAndExit("Problem with genarting shared memory");
    
    if(ftruncate(fd, sizeof(orders)) == -1)
        errorAndExit("Problem with generating segment size");
}

void execWorkers()
{
    for(int i= 0; i < W1; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0)
            execlp("./worker1", "worker1", NULL);
        pids[i]= childPid;
    }

    for(int i= 0; i < W2; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0)
            execlp("./worker2", "worker2", NULL);
        pids[i + W1]= childPid;
    }

    for(int i= 0; i < W3; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0)
            execlp("./worker3", "worker3", NULL);
        pids[i + W1 + W2]= childPid;
    }

    for(int i=0; i < W1 + W2 + W3; i++)
        wait(NULL);
}

int main()

{
    signal(SIGINT, handleSIGINTpids);
    generateSems();
    generateShm();
    execWorkers();
    clear();

    return 0;
}