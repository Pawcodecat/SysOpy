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



void takeAndAddOrder()
{
    sem_wait(sems[0]);
    sem_post(sems[1]);

    orders* ords = mmap(NULL, sizeof(orders), PROT_WRITE | PROT_READ, MAP_SHARED, shmFd, 0);
    if(ords == (void*)-1)
        errorAndExit("Problem with mapping memory");
    
    int val = randInt();
    int idx = (getValue(1) - 1) % MAX_ORDERS;
    ords->values[idx] = val;

    int ordsToPrepare = getValue(3) + 1;
    int ordsToSend = getValue(5);
    printf("%d %ld: Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
            getpid(), time(NULL), val, ordsToPrepare, ordsToSend);

    if(munmap(ords,sizeof(ords)) == -1)
        errorAndExit("Problem with unmaping shared memory");

    sem_post(sems[0]);
    sem_post(sems[3]);
}



int main()
{
    srand(time(NULL));

    signal(SIGINT, handleSIGINT);

    openSems();
    openShm();

    while(1)
    {
        usleep(randTime);
        if(getVal(3) + getVal(5) < MAX_ORDERS)
            takeAndAddOrder();
    }

    return 0;
}