#ifndef CONFIG_H
#define CONFIG_H

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
#include <time.h>

#define MAX_ORDERS  5
#define MIN_SLEEP  100
#define MAX_SLEEP  1000
#define MIN_VAL  10
#define MAX_VAL  100

typedef struct 
{
    int values[MAX_ORDERS];
} orders;



void errorAndExit(char* msg);
int randInt();
int randTime();
int getSemaphore();
int getSharedMemory();

#endif //CONFIG_H