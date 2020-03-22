#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int maxDepth;
char *command;
char mode;
time_t date;

void takeFromStat(const char *fileName, const struct stat *statptr)
	{
        char typeOfFile[64] ="undefined";
		
        if(S_ISREG(statptr->st_mode))
            strcpy(typeOfFile, "file");
        else if(S_ISDIR(statptr->st_mode))
            strcpy(typeOfFile, "dir");
        else if(S_ISLNK(statptr->st_mode))
            strcpy(typeOfFile, "slink");
        else if(S_ISCHR(statptr->st_mode))
            strcpy(typeOfFile, "char dev");
        else if(S_ISBLK(statptr->st_mode))
            strcpy(typeOfFile, "block dev");
        else if(S_ISFIFO(statptr->st_mode))
            strcpy(typeOfFile, "fifo");
        else if(S_ISSOCK(statptr->st_mode))
            strcpy(typeOfFile, "socket");

        struct tm tmTimeModification;
        localtime_r(&statptr->st_mtime, &tmTimeModification);
        char timeModificationString[255];
        strftime(timeModificationString, 255, "%Y-%m-%d %H:%M:%S", &tmTimeModification);

        struct tm tmTimeAccess;
        localtime_r(&statptr->st_atime, &tmTimeAccess);
        char timeAccessString[255];
        strftime(timeAccessString, 255, "%Y-%m-%d %H:%M:%S", &tmTimeAccess);
        
        printf("%s || type: %s, size: %ld, modification time: %s, access time: %s, nlinks: %ld\n",
        fileName, typeOfFile, statptr->st_size, timeModificationString, timeAccessString, statptr->st_nlink);
  
	}

int fileDetails(const char *fileName, const struct stat* statptr, int fileFlags, struct FTW *pftw)
{
    if(maxDepth != -1 && pftw->level > maxDepth)
        return 0;
    if(strcmp(command,"maxDepth") == 0)
    {
        takeFromStat(fileName, statptr);
        return 0;
    }
    else if (strcmp(command, "mtime") == 0)
    {
        time_t modifficationTime = statptr->st_mtime;

        if(mode =='-')
        {
            int differenceOfModification = difftime(date, modifficationTime);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                return 0;
            takeFromStat(fileName, statptr);    

        }
        else if(mode == '+')
        {
            int differenceOfModification = difftime(date, modifficationTime);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                return 0;

            takeFromStat(fileName, statptr);
        }
        else if (mode == '=')
        {
            int differenceOfModification = abs(difftime(modifficationTime, date)) / (60 * 60 * 24);
            int differenceOfModification2 = difftime(date, modifficationTime);

            if((differenceOfModification == 0 && mode == '=') && !(differenceOfModification2 < 0 && mode == '='))
                takeFromStat(fileName, statptr);
            return 0;

        }
    }
     else if (strcmp(command, "atime") == 0)
    {
        time_t modifficationTime = statptr->st_atime;

        if(mode =='-')
        {
            int differenceOfModification = difftime(date, modifficationTime);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                return 0;
            takeFromStat(fileName, statptr);    

        }
        else if(mode == '+')
        {
            int differenceOfModification = difftime(date, modifficationTime);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                return 0;

            takeFromStat(fileName, statptr);
        }
        else if (mode == '=')
        {
            int differenceOfModification = abs(difftime(modifficationTime, date)) / (60 * 60 * 24);
            int differenceOfModification2 = difftime(date, modifficationTime);

            if((differenceOfModification == 0 && mode == '=') && !(differenceOfModification2 < 0 && mode == '='))
                takeFromStat(fileName, statptr);
            return 0;

        }
    }

    return 0;
}

void takeContentDirectory(char *root, char *commandFunc, char modeFunc, int count, int maxDepthFunc)
{
    int flags = FTW_PHYS;
    int fdLimit = 2;
    mode = modeFunc;
    command = commandFunc;
    maxDepth = maxDepthFunc;

    time_t timeVar;
    struct tm *timeInformation;

    time(&timeVar);
    timeInformation = localtime(&timeVar);
    timeInformation ->tm_mday -=count;
    if(mode == '+')
        timeInformation->tm_mday -= 1;

    date = mktime(timeInformation);

    nftw(root, fileDetails, fdLimit, flags);
}

int main(int argc, char *argv[])
{
    if(argc != 6)
    {
        fprintf(stderr,"wrong number of arguments\n");
        exit(-1);
    }
    char *dir = argv[1];
    char *command = argv[2];
    char *mode = argv[3];
    int day = atoi(argv[4]);
    int mdepth =  atoi(argv[5]);

    takeContentDirectory(dir, command, mode[0], day, mdepth);
    return 0;

}