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
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MAX_ORDERS 5
#define MIN_SLEEP 100
#define MAX_SLEEP 1000
#define MIN_VAL 1
#define MAX_VAL 100


int shmFd;
const char *SEMS[6] = {"/SWITCH", "/IDX", "/ORDER_IDX", "/ORDS_TO_PREP", "/SEND_IDX", "/ORDS_TO_SEND"};
struct  sem_t *sems[6];
typedef struct
{
    int values[MAX_ORDERS];
} orders;

int randInt(){return (rand()%(MAX_VAL - MIN_VAL +1)+MIN_VAL);}

int randTime(){return ((rand() % (MAX_SLEEP - MIN_SLEEP + 1) + MIN_SLEEP) * 1000);}

void errorAndExit(char* msg);
void handleSIGINT(int sgn);
int getValue(int idx);
void openSems();
void openShm();


#endif //CONFIG_H