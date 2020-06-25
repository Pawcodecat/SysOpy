#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CLIENTS 4
#define NAME_LEN 30

typedef enum mType {
    STOP = 1, 
    DISCONNECT = 2, 
    INIT = 3, 
    LIST = 4, 
    CONNECT = 5
} mType;

const char* SERVER_QUEUE_NAME = "/SERVER";
const int MAX_MSG_LEN = 8192;

#endif //CONFIG_H