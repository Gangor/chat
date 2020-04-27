#pragma once
#include <pthread.h>
#include <sys/socket.h>

typedef struct client
{
    int fd;
    struct sockaddr * socket;
    char * pseudo;
    pthread_t thread;
} client_t;

typedef struct command
{
    const char* cmd;
    const char* usage;
    const char* description;
    void (*func)(client_t*,char*);
} command_t;