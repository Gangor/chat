#ifndef _CHAT_H
#define _CHAT_H

    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <signal.h>
    #include <string.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <pthread.h>
    
    #define MAX_CLIENT 5

    int Socket;
    struct sockaddr_in server;

    int end = 0;

    //int daemon(int nochdir, int noclose);
    void* connectionProcess( void* args );

#endif