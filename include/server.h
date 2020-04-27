#pragma once
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
#include <client.h>
#include <command.h>
#include <helper.h>
#include <logger.h>
#include <struct.h>

#define MAX_CLIENT 50
#define BUFFER 256

int sock, port;
client_t clients[ MAX_CLIENT ];

client_t* addClient( int fd );
void* clientProcess( void* ptr );
client_t* findClient( const char* pseudo );
int readLine( client_t* client, char* buffer );
void* mainProcess( void* ptr );
void message( client_t* client, const char* format, ... );
int stop();

#define MESSAGE( c, ... ) message( c, __VA_ARGS__ )
#define BROADCAST( ... ) \
    for( int i = 0; i < MAX_CLIENT; i++ ) \
        if( clients[i].pseudo != NULL ) \
            message( &clients[i], __VA_ARGS__ )
