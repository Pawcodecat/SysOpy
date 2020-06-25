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

void sendOrder()
{
    sembuf * toSend = calloc(4, sizeof(sembuf));

    toSend[0].sem_num = 0;
    toSend[0].sem_op = 0;
    toSend[0].sem_flg = 0;

    toSend[1].sem_num = 0;
    toSend[1].sem_op = 1;
    toSend[1].sem_flg = 0;

    toSend[2].sem_num = 4;
    toSend[2].sem_op = 1;
    toSend[2].sem_flg = 0;

    toSend[3].sem_num = 5;
    toSend[3].sem_op = -1;
    toSend[3].sem_flg = 0;

    semop(semId, toSend, 4);

    orders* orders = shmat(shmId, NULL, 0);
    int idx = (semctl(semId, 4, GETVAL, NULL) -1) % MAX_ORDERS;
    orders->values[idx] *= 3;


    int ordersToPrepare = semctl(semId, 3, GETVAL, NULL);
    int ordersToSend = semctl(semId, 5, GETVAL, NULL);
    
     printf("%d %ld: Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
           getpid(), time(NULL), orders->values[idx], ordersToPrepare, ordersToSend);
    orders->values[idx] = 0;
    
    shmdt(orders);

    sembuf *returned = calloc(1, sizeof(sembuf));

    returned[0].sem_num = 0;
    returned[0].sem_op = -1;
    returned[0].sem_flg = 0;


    semop(semId, returned, 1);

}

int main()
{
    srand(time(NULL));
    semId = getSemaphore();
    shmId = getSharedMemory();

    while(1)
    {
        usleep(randTime());
        if(semctl(semId, 5, GETVAL, NULL) > 0)
            sendOrder();
    }
    return 0;
}
