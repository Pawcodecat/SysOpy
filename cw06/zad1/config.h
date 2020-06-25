#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CLIENTS 4

typedef enum mType {
    STOP = 1,
    DISCONNECT = 2,
    INIT = 3,
    LIST = 4,
    CONNECT = 5
}mType;


typedef struct msgDetail {
    long mType;
    char mText[1024];
    key_t qKey;
    int clientId;
    int connectClientId;
}msgDetail;

const int SERVER_PRC_ID = 1;
const int MSG_SIZE = sizeof(msgDetail)-sizeof(long);

#endif