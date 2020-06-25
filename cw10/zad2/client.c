#define _POSIX_C_SOURCE 200112L
#define MAX_MESSAGE_LENGTH 256

#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int server_socket;
int binded_socket;
int is_client_O;
char buffer[MAX_MESSAGE_LENGTH + 1];
char *name;

typedef enum
{
    FREE,
    O,
    X
} field;

typedef struct
{
    int move;
    field fields[9];

} Board;

int move(Board *board, int position)
{
    if(position < 0  || position  > 8 || board->fields[position] != FREE )
        return 0;
    if(board->move)
        board->fields[position] = O;
    else 
        board->fields[position] = X;
    
    board->move = !board->move;
    return 1;
}

field check_winner(Board *board)
{
    field column = FREE;
    for(int x = 0; x < 3; x++)
    {
        field first = board->fields[x];
        field second = board->fields[x + 3];
        field third = board->fields[x + 6];
        if(first == second && first == third && first != FREE)
            column = first;

    }
    if(column != FREE)
            return column;

    field row = FREE;
    for(int y = 0; y < 3; y++)
    {
        field first = board->fields[3*y];
        field second = board->fields[3*y + 1];
        field third = board->fields[3*y + 2];
        if(first == second && first == third && first != FREE)
            row = first;
    }
    if(row != FREE)
            return row;
    
    field lower_diagonal = FREE;

    field first = board->fields[0];
    field second = board->fields[4];
    field third = board->fields[8];
    if (first == second && first == third && first != FREE)
        lower_diagonal = first;
    if (lower_diagonal != FREE)
        return lower_diagonal;

    field upper_diagonal = FREE;
    first = board->fields[2];
    second = board->fields[4];
    third = board->fields[6];
    if (first == second && first == third && first != FREE)
        upper_diagonal = first;
    return upper_diagonal;
}

Board board;

typedef enum
{
    GAME_STARTING,
    WAITING,
    WAITING_FOR_MOVE,
    MOVE_RIVAL,
    MOVE,
    QUIT
} State;

State state = GAME_STARTING;

char *command, *arg;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void quit()
{
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "quit ;%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}

void check_game()
{
    int win = 0;
    field winner = check_winner(&board);
    if(winner != FREE)
    {
        if((is_client_O && winner == 0) || (!is_client_O && winner == X))
            puts("Win");
        else 
            puts("Lost");
        
        win = 1;
    }

    int draw = 1;
    for(int i=0; i < 9; i++)
    {
        if(board.fields[i] == FREE)
        {
            draw = 0;
            break;
        }
    }

    if(draw && !win)
        puts("draw");

    if(win || draw)
        state = QUIT;
}

void split(char *reply)
{
    command = strtok(reply, ":");
    arg = strtok(NULL, ":");
}

Board new_board()
{
    Board board = {1, {FREE}};

    return board;
}

void draw()
{
    char symbol;
    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            if(board.fields[y * 3 + x] == FREE)
                symbol = y * 3 + x + 1 + '0';
            else if(board.fields[y * 3 + x] == 0)
                symbol = 'O';
            else 
                symbol = 'X';
            
            printf(" %c ", symbol);
        }
        printf("\n==========================\n");
        
    }
}

void play_game()
{
    while(1)
    {
        if(state == GAME_STARTING)
        {
            if(strcmp(arg,"name_taken") == 0)
            {
                puts("name is already taken");
                exit(1);
            }
            else if(strcmp(arg, "no_enemy") == 0)
            {
                puts("Waiting for rival");
                state = WAITING;
            }
            else
            {
                board = new_board();
                if(arg[0] == 'O')
                    is_client_O = 1;
                else
                    is_client_O = 0;

                if(is_client_O)
                    state = MOVE;
                else
                    state = WAITING_FOR_MOVE;
            }
        }
        else if (state == WAITING)
        {
            pthread_mutex_lock(&mutex);
            while(state != GAME_STARTING && state != QUIT)
            {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);

             board = new_board();
                if(arg[0] == 'O')
                    is_client_O = 1;
                else
                    is_client_O = 0;

                if(is_client_O)
                    state = MOVE;
                else
                    state = WAITING_FOR_MOVE;
        }
        else if(state == WAITING_FOR_MOVE)
        {
            puts("Waiting for rivals move");
             pthread_mutex_lock(&mutex);
            while(state != MOVE_RIVAL && state != QUIT)
            {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if(state == MOVE_RIVAL)
        {
            int pos = atoi(arg);
            move(&board, pos);
            check_game();
            if(state != QUIT)
                state = MOVE;
        }
        else if(state == MOVE)
        {
            draw();
            int pos;
            do 
            {
                printf("Next move %c ", is_client_O ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            }while(!move(&board, pos));

            draw();

            char buffer[MAX_MESSAGE_LENGTH + 1];
            sprintf(buffer, "move:%d:%s",pos, name);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

            check_game();
            if(state != QUIT)
                state = WAITING_FOR_MOVE;
            
        }
        else if(state == QUIT)
            quit();
    }
}

void init_server_connection(char *destination, int is_local)
{
    struct sockaddr_un local_sockaddr;

    if(is_local)
    {
        server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);

        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, destination);

        connect(server_socket, (struct sockaddr *)&local_sockaddr, sizeof(struct sockaddr_un));
        binded_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un binded_sockaddr;
        memset(&binded_sockaddr, 0, sizeof(struct sockaddr_un));
        binded_sockaddr.sun_family = AF_UNIX;
        sprintf(binded_sockaddr.sun_path, "%d", getpid());
        bind(binded_socket, (struct sockaddr *)&binded_sockaddr, sizeof(struct sockaddr_un));
    }
    else
    {
        struct addrinfo *info;

        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        getaddrinfo("localhost", destination, &hints, &info);

        server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_socket, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);

    }
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "add: :%s", name);
    if(is_local)
        sendto(binded_socket, buffer, MAX_MESSAGE_LENGTH, 0, 
        (struct sockaddr *)&local_sockaddr, sizeof(struct sockaddr_un));
    else
        send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        
    
    
    
}

void listen_server(int is_local)
{
    int game_thread_running = 0;
    while(1)
    {
        if(is_local)
            recv(binded_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        else
            recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
            
        split(buffer);

        pthread_mutex_lock(&mutex);
        if(strcmp(command, "add") == 0)
        {
            state = GAME_STARTING;
            if(!game_thread_running)
            {
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *))play_game, NULL);
                game_thread_running = 1;
            }
        }
        else if(strcmp(command, "move") == 0)
            state = MOVE_RIVAL;
        else if(strcmp(command, "quit") == 0)
        {
            state = QUIT;
            exit(0);
        }
        else if(strcmp(command, "ping") == 0)
        {
            sprintf(buffer, "pong: :%s", name);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        fprintf(stderr,"Wrong number of arguments");
        return 1;
    }
    name = argv[1];
    char *type = argv[2];
    char *destination = argv[3];

    signal(SIGINT, quit);
    int is_local = strcmp(type, "local") == 0;
    init_server_connection(destination, is_local);
   

    listen_server(is_local);
    return 0;
}