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

void boxOrder()
{
    sem_wait(sems[0]);
    sem_post(sems[2]);
    sem_wait(sems[3]);
    orders *ords = mmap(NULL, sizeof(orders), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if(ords == (void*)-1)
        errorAndExit("Problem with mapping shared memory");
    
    int idx = (getValue(2) - 1) % MAX_ORDERS;
    ords->values[idx] *= 2;
    int ordsToPrepare = getValue(3);
    int ordsToSend = getValue(5) + 1;
    printf("%d %ld: Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d\n",
            getpid(), time(NULL), ords->values[idx], ordsToPrepare, ordsToSend);

    if(munmap(ords, sizeof(ords)) == -1)
        errorAndExit("Problem with unmaping shared memory");
    
    sem_post(sems[0]);
    sem_post(sems[5]);
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
        if(getValue(3) > 0)
            boxOrder();
    }

    return 0;
}