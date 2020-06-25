
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

char* queClients[MAX_CLIENTS];
bool accessClients[MAX_CLIENTS];


mqd_t server;

void errorAndExit(char* msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}
int findClientId()
{
    int i = 0;
    while(i < MAX_CLIENTS && queClients[i] != NULL)
        i++;
    if(i < MAX_CLIENTS)
        return i+1;
    else
        return -1;
    
}

void quit(int sgn) {
    char* msgResponse = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(queClients[i] != NULL) {
            mqd_t clientQueue = mq_open(queClients[i], O_RDWR);

            if(clientQueue < 0)
                errorAndExit("Problem with accessing client queue");
           
            if(mq_send(clientQueue, msgResponse, MAX_MSG_LEN, STOP) == -1)
                errorAndExit("Problem with sending message");
            
            if(mq_receive(server, msgResponse, MAX_MSG_LEN, NULL) == -1) 
                errorAndExit("Problem with receiving message");
            
            if(mq_close(clientQueue)== -1)
                errorAndExit("Problem with closing queue");
        }
    }

    if(mq_close(server) == -1) 
        errorAndExit("cProblem with closing queue");
   
    if(mq_unlink(SERVER_QUEUE_NAME) == -1) 
        errorAndExit("Problem with deleting queue");
    exit(0);
}

void execChat(char* msg, int priorytet)
{
    char* msgResponse = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    mqd_t clientQueue;
    int clientId;
    int externalClientId;

    switch(priorytet) {
        case INIT: ;
            puts("Init received");

            int id = findClientId();
            if(id < 0) return;

            clientQueue = mq_open(msg, O_RDWR);
            if(clientQueue == -1)
                errorAndExit("Problem with accessing client queue");

            accessClients[id - 1] = true;
            queClients[id - 1] = (char*)calloc(NAME_LEN, sizeof(char));
            strcpy(queClients[id - 1], msg);

            if(mq_send(clientQueue, msgResponse, MAX_MSG_LEN, id) == -1)
                errorAndExit("Problem with sending message");
            if(mq_close(clientQueue) == -1 )
                errorAndExit("Problem with closing queue");
            break;
        case LIST:
            puts("List received");

            for(int i = 0; i < MAX_CLIENTS; i++)
            {
                if(queClients[i] != NULL)
                {
                    sprintf(msgResponse + strlen(msgResponse),
                            "ID %d, client %s\n", i + 1, accessClients[i] ? "accessible" : "not accessible");
                }
            }

            clientId = (int) msg[0];
            clientQueue = mq_open(queClients[clientId - 1], O_RDWR);

            if(clientQueue == -1)
                errorAndExit("Problem with accessing client queue");
            if(mq_send(clientQueue, msgResponse, MAX_MSG_LEN, LIST) == -1)
                errorAndExit("Problem with sending message");
            if(mq_close(clientQueue) == -1)
                errorAndExit("Problem with closing queue");
            break;
        case CONNECT:
            puts("Connect received");
            clientId = (int) msg[0];
            externalClientId = (int) msg[1];

            clientQueue = mq_open(queClients[clientId - 1], O_RDWR);
            if(clientQueue == -1)
                errorAndExit("Problem with accessing client queue");

            msgResponse[0] = externalClientId;
            strcat(msgResponse, queClients[externalClientId - 1]);

            if(mq_send(clientQueue, msgResponse, MAX_MSG_LEN, CONNECT) == -1)
                errorAndExit("Problem with sending message");

            memset(msgResponse, 0, strlen(msgResponse));
            clientQueue = mq_open(queClients[externalClientId - 1], O_RDWR);

            if(clientQueue == -1)
                errorAndExit("Problem with accessing client queue");

            msgResponse[0] = clientId;

            strcat(msgResponse, queClients[clientId - 1]);

            if(mq_send(clientQueue, msgResponse, MAX_MSG_LEN, CONNECT) == -1)
                errorAndExit("Problem with sending message");
            if(mq_close(clientQueue) == -1)
                errorAndExit("Problem with closing queue");

            accessClients[clientId - 1] = false;
            accessClients[externalClientId - 1] = false;
            break;
        case DISCONNECT:
            puts("Disconnect received");
            clientId = (int) msg[0];
            externalClientId = (int) msg[1];

            clientQueue = mq_open(queClients[externalClientId - 1], O_RDWR);

            if(clientQueue == -1)
                errorAndExit("Problem with accessing client queue");
            if(mq_send(clientQueue, msgResponse, MAX_MSG_LEN, DISCONNECT) == -1)
                errorAndExit("Problem with sending message");
            if(mq_close(clientQueue) == -1)
                errorAndExit("Problem with closing queue");

            accessClients[clientId - 1] = true;
            accessClients[externalClientId - 1] = true;
            break;
        case STOP:
            puts("Stop received");
            clientId = (int) msg[0];

            accessClients[clientId - 1] = false;
            queClients[clientId - 1] = NULL;
            break;
        default:
            puts("wrong message");
    }
}

int main() {
    for(int i = 0; i < MAX_CLIENTS; i++)  
        queClients[i] = NULL;

    server = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if(server == -1 ) 
        errorAndExit("Problem with creating queue");

    signal(SIGINT, quit);

    char* msg = (char*)calloc(MAX_MSG_LEN, sizeof(char));
    unsigned int priorytet;

    puts("Server has started");

    while(1) 
    {
        if(mq_receive(server, msg, MAX_MSG_LEN, &priorytet) == -1) 
            errorAndExit("Problem with receiving message");
        execChat(msg, priorytet);
    }

    return 0;
}