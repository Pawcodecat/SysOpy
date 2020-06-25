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
#include <time.h>

#include "config.h"

sem_t *sems[6];
int shmFd;

void sendOrder()
{
    sem_wait(sems[0]);
    sem_post(sems[4]);
    sem_wait(sems[5]);

    orders *ords = mmap(NULL, sizeof(ords), PROT_READ |  PROT_WRITE, MAP_SHARED, shmFd, 0);
    if(ords == (void*)-1)
        errorAndExit("Problem with maping shared memory");

    int idx = (getVal(4) - 1) % MAX_ORDERS;
    ords->values[idx] *= 3;
    int ordsToPrepare = getValue(3);
    int ordsToSend = getValue(5);
    printf("%d %ld: Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: 5d. Liczba zamowien do wyslania%d",
            getpid(), time(NULL), ords->values[idx], ordsToPrepare, ordsToSend);
    ords->values[idx] = 0;

    if(munmap(ords, sizeof(orders)) == -1)
        errorAndExit("Problem with unmaping shared memory");
    
    sem_post(sems[0]);

}

int main()
{
    srand(time(NULL));

    signal(SIGINT, handleSIGINT);

    openSems();
    openShm();

    while(1)
    {
        usleep(randTime());
        if(getValue(5) > 0)
            sendOrder();
    }

    return 0;
}