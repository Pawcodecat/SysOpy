#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "config.h"

#define W1 3
#define W2 3
#define W3 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int semId;
int shmId;

pid_t pids[W1+W2+W3];

void errorAndExit(char* msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

void generateSemaphore()
{
    key_t semKey = ftok(getenv("HOME"), 0);
    semId = semget(semKey, 6, IPC_CREAT | 0666);

    if(semId == -1)
        errorAndExit("Problem with generating semaphore set");

    union semun arg;
    arg.val = 0;

    for(int i= 0; i < 6; i++)
        semctl(semId, i, SETVAL, arg);
}

void generateSharedMemory()
{
    key_t shmKey = ftok(getenv("HOME"), 1);
    shmId = semget(shmKey, sizeof(orders), IPC_CREAT | 0666);

    if(shmId == -1)
        errorAndExit("Problem with generating shared memory");
}

void clear()
{
    semctl(semId, 0, IPC_RMID, NULL);
    shmctl(shmId, IPC_RMID, NULL);
    system("make clean");
}


void handle_SIGINT(int sgn)
{
    for(int i=0; i < W1+W2+W3; i++)
        kill(pids[i], SIGINT);

    clear();
    exit(0);
}

void execWorkers()
{
    for(int i = 0; i < W1; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0)
            execlp("./worker1", "worker1", NULL);
        pids[i] = childPid;
    }

    for(int i = 0; i < W2; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0)
            execlp("./worker2", "worker2", NULL);
        pids[W1+i] = childPid;
    }

    for(int i = 0; i < W3; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0)
            execlp("./worker3", "worker3", NULL);
        pids[W1 + W2 + i] = childPid;
    }

    for(int i = 0; i < W1 + W2 + W3; i++ )
    {
        wait(NULL);
    }
}

int main()
{
    signal(SIGINT, handle_SIGINT);
    generateSemaphore();
    generateSharedMemory();
    execWorkers();
    // clear();
    return 0;
}