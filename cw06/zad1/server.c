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

#include "config.h"

key_t queClients[MAX_CLIENTS];
bool accessClients[MAX_CLIENTS];


mqd_t server;

void errorAndExit(char* msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

void quit(int sgn)
{
    msgDetail* msgResponse = (msgDetail*)malloc(sizeof(msgDetail));
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        key_t qKey = queClients[i];
        if(qKey != -1)
        {
            msgResponse->mType = STOP;
            int clientQId = msgget(qKey, 0);
            if(clientQId == -1)
                errorAndExit("problem with acccessing client queue");
            if(msgsnd(clientQId, msgResponse, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
            if(msgrcv(server, msgResponse, MSG_SIZE, STOP, 0) == -1)
                errorAndExit("Problem with receiving message");

        }
        msgctl(server, IPC_RMID, NULL);
        exit(0);
    }
}

int findClientId()
{
    int i = 0;
    while(i < MAX_CLIENTS && queClients[i] != -1)
        i++;
    if(i < MAX_CLIENTS)
        return i+1;
    else
        return -1;
    
}


void execChating(msgDetail* msg)
{
    msgDetail* msgResponse = (msgDetail*)malloc(sizeof(msgDetail));
    int clientId = msg->clientId;
    int clientQId;
    int externalClientId;

    switch(msg->mType){
        case INIT:
            puts("Init received");
            int id = findClientId();
            if(id < 0)
                return;
            
            msgResponse->mType = id;
            clientQId = msgget(msg->qKey, 0);
            if(clientQId == -1)
                errorAndExit("Problem with acccessing client queue");

            queClients[id-1] = msg->qKey;
            accessClients[id-1] = true;

            if(msgsnd(clientQId, msgResponse, MSG_SIZE, 0) < 0)
                errorAndExit("Problem with sending message");
            break;
        case LIST:
            puts("List received");
            strcpy(msgResponse->mText,"");

            for(int i = 0; i < MAX_CLIENTS; i++)
            {
                if(queClients[i] != -1)
                    sprintf(msgResponse + strlen(msgResponse->mText),
                    "Id %d, client %s\n", i+1, accessClients[i] ? "accessible" : "NOT accesible");
            }
            clientQId = msgget(queClients[clientId - 1], 0);
            if(clientQId == -1)
                errorAndExit("Problem with accesssing client queue");
            
            msgResponse->mType = clientId;
            if(msgsnd(clientQId, msgResponse, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
            break;
        case CONNECT:
            puts("Connect received");
            externalClientId = msg->connectClientId;

            msgResponse->mType = CONNECT;
            msgResponse->qKey = queClients[externalClientId - 1];
            clientQId = msgget(queClients[clientId - 1], 0);
            if(clientQId == -1)
                errorAndExit("Problem with access client queue");
            if(msgsnd(clientQId, msgResponse, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
            
            msgResponse->mType = CONNECT;
            msgResponse->qKey = queClients[clientId -1];
            msgResponse->clientId = clientId;
            clientQId = msgget(queClients[externalClientId -1], 0);
            if(clientQId == -1)
                errorAndExit("Problem with access client queue");
            if(msgsnd(clientQId, msgResponse, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
            
            accessClients[clientId -1] = false;
            accessClients[externalClientId - 1] = false;
            break;
        case DISCONNECT:
            puts("Disconnect received");
            externalClientId = msg->connectClientId;

            msgResponse->mType = DISCONNECT;
            clientQId = msgget(queClients[externalClientId - 1], 0);
            if(clientQId == -1)
                errorAndExit("Problem with access client queue");
            if(msgsnd(clientQId, msgResponse, MSG_SIZE, 0) == -1)
                errorAndExit("Problem with sending message");
            
            accessClients[clientId -1] = true;
            accessClients[externalClientId - 1] = true;
            break;
        case STOP:
            puts("Stop received");
            queClients[clientId - 1] = -1;
            accessClients[clientId - 1] = false;
            break;
        default:
            puts("Problem with received message");


    }
}

int main()
{
    for(int i=0; i < MAX_CLIENTS; i++)
        queClients[i] = -1;
    
    key_t qKey = ftok(getenv("HOME"), SERVER_PRC_ID);
    printf("server has queue key: %d\n", qKey);
    server = msgget(qKey, IPC_CREAT | 0666);
    printf("Server has queue id: %d\n", server);
    signal(SIGINT, quit);

    msgDetail* msg = (msgDetail*)malloc(sizeof(msgDetail));
    while(1)
    {
        if(msgrcv(server, msg, MSG_SIZE, -6, 0) == -1)
            errorAndExit("Problem with recievnig message");
        execChating(msg);
    }
    return 0;
}
