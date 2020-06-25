#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

int handleSignal = 0;
const bool EXEC = true; 

void printContentDir(){
    

    DIR *dir = opendir(".");

    struct dirent *directory;

    while ((directory = readdir(dir)) != NULL)
        printf("%s\n", directory->d_name);
    closedir(dir);
    
}

void signalHandler(int signal)
{
    if(handleSignal == 0)
        printf("\nOtrzymano sygnal %d\n kontynuacja: CTRL+Z \t zakończenie programu: CTRL+C \n", signal);

    if(handleSignal == 0)
        handleSignal = 1;
    else
        handleSignal = 0;
    
}

void intSignal(int signal)
{
    printf("\nOdebrano sygnał SIGINT: %d\n", signal);
    exit(EXIT_SUCCESS);
}


int main(int argc, char const **argv)
{
    struct sigaction sigAction;
    sigAction.sa_handler = signalHandler;
    sigAction.sa_flags = 0;
    sigemptyset(&sigAction.sa_mask);
    time_t currentTime;

    while(EXEC)
    {
        sigaction(SIGTSTP, &sigAction, NULL);
        signal(SIGINT, intSignal);
        if(handleSignal == 0)
        {
            currentTime = time(0);
            char tmpTime[64];
            strftime(tmpTime, sizeof(tmpTime), "Time\t%H:%M:%S", localtime(&currentTime));
            printf("%s\n", tmpTime);
            printf("content directory: \n");
            printContentDir();
            printf("\n");
        }
        sleep(1);

    }

}