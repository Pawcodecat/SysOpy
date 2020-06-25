 
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>

#include "config.h"

mqd_t queue;
mqd_t serverQueue;
int clientId;

char queueName[NAME_LEN];

void errorAndExit(char* msg)
 {
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

void stopCmd()
{
    char *msg = (char*)calloc(MAX_MSG_LEN,  sizeof(char));
    msg[0] = clientId;

    if(mq_send(serverQueue, msg, MAX_MSG_LEN, STOP) == -1)
        errorAndExit("Problem with sending message");

    if(mq_close(serverQueue) == -1)
        errorAndExit("Problem with closing server queue");

    exit(0);
}

void chatMode(int externalId, mqd_t externalQueue)
{
    char* cmd = NULL;
    size_t len =0;
    ssize_t line = 0;

    char* msg = (char*)calloc(MAX_MSG_LEN, sizeof(char));

    while(1)
    {
        printf("Give message or disconnect: ");
        line = getline(&cmd, &len, stdin);
        cmd[line - 1] = '\0';

        struct timespec* timeS = (struct timespec*)malloc(sizeof(struct timespec));
        unsigned int type;
        bool connect = true;
        while(mq_timedreceive(queue, msg, MAX_MSG_LEN, &type, timeS) >= 0)
        {
            if(type == STOP)
            {
                puts("Stop from server, chat is quiting");
                stopCmd();
            }
            else if(type == DISCONNECT)
            {
                puts("Chat is disconnecting");
                connect = false;
                break;
            }
            else
            {
                printf("%d: %s\n", externalId, msg);
            }
        }
        if(!connect)
            break;

        if(strcmp(cmd, "DISCONNECT") == 0)
        {
            msg[0] = clientId;
            msg[1] = externalId;
            if(mq_send(serverQueue, msg, MAX_MSG_LEN, DISCONNECT) == -1)
                errorAndExit("Problem with sending message");
            break;

        }
        else if(strcmp(cmd, "") != 0)
        {
            strcpy(msg, cmd);
            if(mq_send(externalQueue, msg, MAX_MSG_LEN, connect) == -1)
                errorAndExit("Problem with sending message");
        }
    }
}



int initConnection()
{
    char* msg = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    strcpy(msg, queueName);

    if(mq_send(serverQueue, msg, MAX_MSG_LEN, INIT) == -1)
        errorAndExit("Problem with sending message");
    
    unsigned int clientId;
    
    if(mq_receive(queue, msg, MAX_MSG_LEN, &clientId) == -1)
        errorAndExit("Problem with receiving message");
    printf("Server received Id: %d\n", clientId);
}

void createQueueName(){
    queueName[0] = '/';
    for(int i = 1; i < NAME_LEN; i++)
        queueName[i]= rand()%('z' - 'a' +1) + 'a';
}

void listCmd()
{
    char* msg = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    msg[0] = clientId;

    if(mq_send(serverQueue, msg, MAX_MSG_LEN, LIST) == -1)
        errorAndExit("Problem with sending message");

    if(mq_receive(queue, msg, MAX_MSG_LEN, NULL) == -1)
        errorAndExit("Problem with receiving message");
    printf("%s\n", msg);

}

void findServerCmd()
{
    char* msg = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    struct timespec* timeS = (struct timespec*)malloc(sizeof(struct timespec));
    unsigned int type;
    if(mq_timedreceive(queue, msg, MAX_MSG_LEN, &type, timeS) >= 0)
    {
        if(type == STOP)
        {
            puts("Stop from server, chat is quiting");
            stopCmd();
        }
        else if(type == CONNECT)
        {
            puts("Server is connecting to client");

            char* externalQueueName = (char*)calloc(NAME_LEN, sizeof(char));
            strncpy(externalQueueName, msg +1, strlen(msg) - 1);
            printf("External queue name: %s\n", externalQueueName);
            mqd_t externalQueue = mq_open(externalQueueName, O_RDWR);
            if(externalQueue == -1 )
                errorAndExit("Problem with accessint to external client queue");
            chatMode((int)msg[0], externalQueue);
        }
    }
    
}

void connectCmd(int id)
{
    char* msg = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    msg[0] = clientId;
    msg[1] = id;


    if(mq_send(serverQueue, msg, MAX_MSG_LEN, CONNECT) == -1)
        errorAndExit("Problem with sending message");

    if(mq_receive(queue, msg, MAX_MSG_LEN, NULL) == -1)
        errorAndExit("Problem with receiving message");
    
    char* externalQueueName = (char*)calloc(NAME_LEN, sizeof(char));
    strncpy(externalQueueName, msg +1, strlen(msg) - 1);
    printf("External queue name: %s\n", externalQueueName);
    mqd_t externalQueue = mq_open(externalQueueName, O_RDWR);
    if(externalQueue == -1 )
        errorAndExit("Problem with accessint to external client queue");
    chatMode(id, externalQueue);
}
void quit(int sgn)
{
    stopCmd();
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    createQueueName();
    printf("Queue name %s\n", queueName);
    queue = mq_open(queueName, O_RDWR | O_CREAT | S_IRWXU | S_IRWXG | S_IRWXO, NULL);
    if(queue == -1) 
        errorAndExit("Problem with creating queue");
    serverQueue = mq_open(SERVER_QUEUE_NAME, O_RDWR);
    if(serverQueue == -1)
        errorAndExit("Problem with creating queue");

    clientId = initConnection();

    signal(SIGINT, quit);

    while(1)
    {
        printf("Give command: ");
        char* cmd = NULL;
        size_t len = 0;
        ssize_t line = 0;
        line = getline(&cmd, &len, stdin);
        cmd[line -1] = '\0';

        findServerCmd();

        if(strcmp(cmd, ""))
            continue;
        
        char* extractCmd = strtok(cmd," ");
        if(strcmp(extractCmd,"LIST") == 0)
        {
            puts("List command");
            listCmd();
        }
        else if(strcmp(extractCmd,"CONNECT") == 0)
        {   
            extractCmd =  strtok(NULL, " ");
            int id =atoi(extractCmd);
            puts("Connect command");
            connectCmd(id);
        }
        else if(strcmp(extractCmd, "STOP") == 0) 
        {
            printf("STOP command\n");
            stopCmd();
        } 
        else {
            printf("Wrong command: %s\n", cmd);

    }
    }

    return 0;
}