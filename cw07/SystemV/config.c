#include "config.h"


void errorAndExit(char* msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

int randInt()
{
    return (rand()%(MAX_VAL - MIN_VAL +1)+MIN_VAL);
}

int randTime()
{
    return ((rand() % (MAX_SLEEP - MIN_SLEEP + 1) + MIN_SLEEP) * 1000);
}

int getSemaphore()
{
    key_t semKey = ftok(getenv("HOME"), 0);
    int semId = semget(semKey, 0, 0);
    if(semId == -1)
        errorAndExit("Problem with getting semaphore");

    return semId;
    
}
int getSharedMemory()
{
    key_t shmKey = ftok(getenv("HOME"), 1);
    int shmId = shmget(shmKey, 0, 0);
    if (shmId == -1)
        errorAndExit("Problem with accesing shared memory");
    return shmId;
}