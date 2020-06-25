#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "config.h"



key_t qKey;
int qId;
int serverQId;
int clientId;

void errorAndExit(char* msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

void stopCmd()
{
    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));
    msg->mType = STOP;
    msg->clientId = clientId;

    if(msgsnd(serverQId, msg, MSG_SIZE, 0) == -1 ) 
        errorAndExit("Problem with sending message");
    if(msgctl(qId, IPC_RMID, NULL) == -1 )
        errorAndExit("Problem with deleting queue");

    msgctl(qId, IPC_RMID, NULL);
    exit(0);
    
}


void chatMode(int externalId, int externalQueueId)
{
    char* cmd = NULL;
    size_t len = 0;
    ssize_t line = 0;
    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));
    while(1)
    {
        printf("Give message or disconnect: ");
        line = getline(&cmd, &len, stdin);
        cmd[line - 1] = '\0';

        if(msgrcv(qId, msg, MSG_SIZE, STOP, IPC_NOWAIT) >= 0)
        {
            puts("Stop from server, chat is quiting");
            stopCmd();
        }
        if(msgrcv(qId, msg, MSG_SIZE, DISCONNECT, IPC_NOWAIT) >= 0)
        {
            puts("Chat is disconnecting");
            break;
        }
        while(msgrcv(qId, msg, MSG_SIZE, 0, IPC_NOWAIT) >= 0)
            printf("%d: %s\n", externalId, msg->mText);

        if(strcmp(cmd, "DISconnect") == 0)
        {
            msg->mType = DISCONNECT;
            msg->clientId = clientId;
            msg->connectClientId = externalId;
            if(msgsnd(serverQId, msg, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
            break;
        }
        else if(strcmp(cmd,"") != 0)
        {
            msg->mType = CONNECT;
            strcpy(msg->mText, cmd);
            if(msgsnd(externalQueueId, msg, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
        }

    }
}

int initConnection()
{
    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));
    msg->mType = INIT;
    msg->qKey = qKey;
    if(msgsnd(serverQId, msg, MSG_SIZE, 0) == -1)
        errorAndExit("Problem with sending message");

    msgDetail* msgRecieve = (msgDetail*)malloc(sizeof(msgDetail));
    if(msgrcv(qId, msgRecieve, MSG_SIZE, 0, 0) == -1)
        errorAndExit("Problem with recieving message" );

    int clientId = msgRecieve->mType;
    return clientId;
}

int connectCmd(int id)
{
    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));
    msg->mType = CONNECT;
    msg->clientId = clientId;
    msg->connectClientId = id;
    if(msgsnd(serverQId, msg, MSG_SIZE, 0) == -1)
        errorAndExit("Problem with sending message");
    
    msgDetail* msgRecieve = (msgDetail*)malloc(sizeof(msgDetail));
    if(msgrcv(qId, msgRecieve, MSG_SIZE, 0, 0) == -1)
        errorAndExit("Problem with receiving message");
    
    key_t  externalQkey = msgRecieve->qKey;
    int externalQId = msgget(externalQkey, 0);
    if(externalQkey == -1)
        errorAndExit("Problem with accessing external client queue");

    chatMode(id, externalQId);
}

void listCmd()
{
    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));
    msg->mType = LIST;
    msg->clientId = clientId;
    if(msgsnd(serverQId, msg, MSG_SIZE, 0) == -1)
        errorAndExit("problem with sending message");
    
    msgDetail* msgReceive = (msgDetail*)malloc(sizeof(msgDetail));
    if(msgrcv(qId, msgReceive, MSG_SIZE, 0, 0) == -1)
        errorAndExit("Problem with reciving message");
    printf("%s\n", msgReceive->mText);
    
}

void findServerMessage()
{
    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));

    if(msgrcv(qId, msg, MSG_SIZE, 0, IPC_NOWAIT) >= 0)
    {
        if(msg->mType == STOP)
        {
            puts("stop from server, chat is quiting");
            stopCmd();
        }
        else if(msg->mType == CONNECT)
        {
            printf("Connecting with client %d\n", msg->clientId);
            int externalQId = msgget(msg->qKey, 0);
            if(externalQId == -1)
                errorAndExit("Problem with accessing external client queue");
            chatMode(msg->clientId, externalQId);
        }
    }
}

void quit(int sgn)
{
    stopCmd();
}

int main()
{
    srand(time(NULL));
    qKey = ftok(getenv("HOME"), rand()%255 +1);
    printf("Queue key: %d\n", qKey);

    qId = msgget(qKey, IPC_CREAT | 0666);
    if(qId == -1)
        errorAndExit("Problem with accessing server queue");
    printf("Server queue Id: %d\n", serverQId);

    clientId = initConnection();
    printf("ID received: %d\n", clientId);

    signal(SIGINT, stopCmd);

    char* cmd = NULL;
    size_t len = 0;
    ssize_t line = 0;
    while(1)
    {
        printf("Give command:");
        line = getline(&cmd, &len, stdin);
        cmd[line-1] = '\0';

        findServerMessage();

        if(strcmp(cmd,"") == 0)
            continue;

        char* extractCmd = strtok(cmd," ");
        if(strcmp(extractCmd, "LIST") == 0)
        {
            puts("List command");
            listCmd();
        }
        else if(strcmp(extractCmd, "CONNECT") == 0)
        {
            extractCmd = strtok(NULL, " ");
            int id = atoi(extractCmd);
            connectCmd(id);
        }
        else if(strcmp(extractCmd,"STOP") == 0)
        {
            puts("Stop command");
            stopCmd();
        }
        else
        {
            printf("Wrong command: %s\n", cmd);
        }
        
    }

    return 0;
}