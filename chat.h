#ifndef _CHAT_H
#define _CHAT_H

    #include <unistd.h>
    #include <stdio.h>
    #include <stdarg.h> 
    #include <stdlib.h>
    #include <signal.h>
    #include <string.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <pthread.h>
    #include <vector>
    
    #define MAX_CLIENT 5
    #define BUFFER 256

    typedef struct connection
    {
        int fd;
        struct sockaddr * socket;
        pthread_t thread;
    } connection_t;

    int fd;
    int End = 0;
    std::vector< connection_t * > Clients;
    struct sockaddr_in Server;

    //int daemon(int nochdir, int noclose);

    void* connectionProcess( void* args );

    void sendMessage( int fd, const char* format, ... );
    void sendAllMessage( const char* format, ... );

#endif