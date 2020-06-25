#include "config.h"

void errorAndExit(char* msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

void handleSIGINT(int sgn)
{
    for(int i = 0; i < 6; i++)
        if(sem_close(sems[i] == -1))
                   errorAndExit("Problem with closing semaphore");
    exit(0);
        
}

int getValue(int idx)
{
    int val;
    sem_getvalue(sems[idx], &val);
    return val;
}

void openSems()
{
    for(int i=0; i < 6; i++)
    {
        sems[i] = sem_open(SEMS[i], O_RDWR);
        if(sems[i] == -1)
            errorAndExit("Problem with opening semaphores");  
    }  
}

void openShm()
{
    shmFd = shm_open("/SHARE_MEMORY", O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if(shmFd == -1)
        errorAndExit("Problem with opening shared memory");

}