#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>


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

    void maxDepth(char *root, int depth)
    {
        if(depth == 0)
            return;
        if(root == NULL)
            return;
        DIR *dir = opendir(root);

        if(dir == NULL)
        {
            fprintf(stderr," Problem with opening directory: %s\n", strerror(errno));
            exit(-1);
        }

        struct dirent *file;

        char nextPath[256];
        while((file = readdir(dir)) != NULL)
        {
            strcpy(nextPath, root);
            strcat(nextPath, "/");
            strcat(nextPath, file->d_name);

            struct stat stat;
            
            if(lstat(nextPath, &stat) < 0)
            {
                fprintf(stderr, "Error: lstat file %s: %s\n", nextPath, strerror(errno));
                exit(-1);
            }

            if(S_ISDIR(stat.st_mode))
            {
                if(strcmp(file->d_name,".") == 0 || strcmp(file->d_name, "..") == 0)
                    continue;
                maxDepth(nextPath, depth - 1);
            }
            takeFromStat(nextPath, &stat);
        }
    closedir(dir);      
    }

void mtime(char *root, char mode, int count, time_t date)
{
    if(count == 0)
        return;
    if(root == NULL)
        return;
    DIR *dir = opendir(root);

    if(dir == NULL)
    {
        fprintf(stderr,"Error: open dir: %s\n",strerror(errno));
        exit(-1);
    }

    struct dirent *file;

    char nextPath[256];
    while((file = readdir(dir)) != NULL)
    {
        strcpy(nextPath, root);
        strcat(nextPath, "/");
        strcat(nextPath, file->d_name);

        struct stat stat;

        if(lstat(nextPath, &stat) < 0)
        {
            fprintf(stderr, "Error: lstat file %s: %s\n", nextPath, strerror(errno));
        }

        if(S_ISDIR(stat.st_mode))
            {
                if(strcmp(file->d_name,".") == 0 || strcmp(file->d_name, "..") == 0)
                    continue;
                mtime(nextPath, mode, count - 1, date);
            }
        time_t timeOfModification = stat.st_mtime;
        if(mode == '-')
        {
            int differenceOfModification = difftime(date, timeOfModification);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                continue;
            takeFromStat(nextPath, &stat);
        }
        else if(mode == '+')
        {
            int differenceOfModification = difftime(date, timeOfModification);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                continue;
            takeFromStat(nextPath, &stat);
        }
        else if (mode == '=')
        {
            int differenceOfModification = abs(difftime(timeOfModification, date)) / (60 * 60 * 24);
            int differenceOfModification2 = difftime(date, timeOfModification);

            if((differenceOfModification == 0 && mode == '=') && !(differenceOfModification2 < 0 && mode == '='))
                takeFromStat(nextPath, &stat);

        }
    }
    closedir(dir);
}
    

    void atime(char* root, char mode, int count, time_t date)
    {
        if(count == 0)
        return;
    if(root == NULL)
        return;
    DIR *dir = opendir(root);

    if(dir == NULL)
    {
        fprintf(stderr,"Error: open dir: %s\n",strerror(errno));
        exit(-1);
    }

    struct dirent *file;

    char nextPath[256];
    while((file = readdir(dir)) != NULL)
    {
        strcpy(nextPath, root);
        strcat(nextPath, "/");
        strcat(nextPath, file->d_name);

        struct stat stat;

        if(lstat(nextPath, &stat) < 0)
        {
            fprintf(stderr, "Error: lstat file %s: %s\n", nextPath, strerror(errno));
        }

        if(S_ISDIR(stat.st_mode))
            {
                if(strcmp(file->d_name,".") == 0 || strcmp(file->d_name, "..") == 0)
                    continue;
                atime(nextPath, mode, count - 1, date);
            }
        time_t timeOfModification = stat.st_atime;
        if(mode == '-')
        {
            int differenceOfModification = difftime(date, timeOfModification);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                continue;
            takeFromStat(nextPath, &stat);
        }
        else if(mode == '+')
        {
            int differenceOfModification = difftime(date, timeOfModification);
            if(!((differenceOfModification == 0 && mode == '=') || (differenceOfModification > 0 && mode == '+') || (differenceOfModification < 0 && mode == '-')))
                continue;
        }
        else if (mode == '=')
        {
            int differenceOfModification = abs(difftime(timeOfModification, date)) / (60 * 60 * 24);
            int differenceOfModification2 = difftime(date, timeOfModification);

            if((differenceOfModification == 0 && mode == '=') && !(differenceOfModification2 < 0 && mode == '='))
                takeFromStat(nextPath, &stat);

        }
    }

}

void takeContentDirecotry(char *root, char *command, char mode, int count, int max_Depth)
{
    if(strcmp(command, "maxdept") == 0)
        maxDepth(root, count);
    else if(strcmp(command, "mtime") == 0)
    {
        time_t timeVar;
        struct tm *InformationAboutTime;

        time(&timeVar);
        InformationAboutTime = localtime(&timeVar);
        InformationAboutTime ->tm_mday -=count;
        
        if(mode == '+')
            InformationAboutTime->tm_mday -=1;

        if(max_Depth >= 0)
            mtime(root, mode, max_Depth, mktime(InformationAboutTime));
        
        else
            mtime(root, mode, -1, mktime(InformationAboutTime));

    }
    else if(strcmp(command, "atime") == 0)
    {
        time_t timeVar;
        struct tm *InformationAboutTime;

        time(&timeVar);
        InformationAboutTime = localtime(&timeVar);
        InformationAboutTime ->tm_mday -=count;
        
        if(mode == '+')
            InformationAboutTime->tm_mday -=1;

        if(max_Depth >= 0)
            atime(root, mode, max_Depth, mktime(InformationAboutTime));
        
        else
            atime(root, mode, -1, mktime(InformationAboutTime));

    }
    
}

int main(int argc, char *argv[])
{
    if(argc != 6)
    {
        fprintf(stderr, "wrong number of arguments");
        exit(-1);
    }
    char *dir = argv[1];
    char *command = argv[2];
    char *mode = argv[3];
    int day = atoi(argv[4]);
    int mdepth =  atoi(argv[5]);

    takeContentDirecotry(dir, command, mode[0], day, mdepth);
    return 0;
}

