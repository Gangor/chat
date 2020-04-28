#pragma once
#include <pthread.h>
#include <sys/socket.h>

typedef struct channel
{
    char* name;
    int home;
} channel_t;

typedef struct client
{
    int fd;
    char * pseudo;
    channel_t* channel;
    pthread_t thread;
} client_t;

typedef struct command
{
    const char* cmd;
    const char* usage;
    const char* description;
    void (*func)(client_t*, struct command*,char*);
} command_t;