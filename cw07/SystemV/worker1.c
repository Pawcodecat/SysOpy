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
#include "config.h"

int semId;
int shmId;

typedef struct sembuf sembuf;

void takeAndAddOrder()
{
    sembuf * toAdd = calloc(3, sizeof(sembuf));

    toAdd[0].sem_num = 0;
    toAdd[0].sem_op = 0;
    toAdd[0].sem_flg = 0;

    toAdd[1].sem_num = 0;
    toAdd[1].sem_op = 1;
    toAdd[1].sem_flg = 0;

    toAdd[2].sem_num = 1;
    toAdd[2].sem_op = 1;
    toAdd[2].sem_flg = 0;

    semop(semId, toAdd, 3);

    orders* orders = shmat(shmId, NULL, 0);
    int idx = (semctl(semId, 1, GETVAL, NULL) -1) % MAX_ORDERS;
    int val = randInt();
    orders->values[idx] = val;

    int ordersToPrepare = semctl(semId, 3, GETVAL, NULL) + 1;
    int ordersToSend = semctl(semId, 5, GETVAL, NULL);
    
     printf("%d %ld: Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
           getpid(), time(NULL), val, ordersToPrepare, ordersToSend);

    shmdt(orders);

    sembuf *returned = calloc(2, sizeof(sembuf));

    returned[0].sem_num = 0;
    returned[0].sem_op = -1;
    returned[0].sem_flg = 0;

    returned[1].sem_num = 3;
    returned[1].sem_op = 1;
    returned[1].sem_flg = 0; 

    semop(semId, returned, 2);

}

int main()
{
    srand(time(NULL));
    semId = getSemaphore();
    shmId = getSharedMemory();

    while(1)
    {
        usleep(randTime());
        if(semctl(semId, 3, GETVAL, NULL) + semctl(semId, 5, GETVAL, NULL) < MAX_ORDERS)
            takeAndAddOrder();
    }
}
