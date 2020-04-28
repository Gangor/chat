#pragma once
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <helper.h>
#include <server.h>
#include <struct.h>

#define MAX_CLIENT 50
#define UNUSED -1

client_t clients[ MAX_CLIENT ];

void clientInit();

client_t* clientAdd( int fd );
int clientChannel( client_t* p, channel_t* channel );
void clientDestroy( client_t* p );
void* clientProcess( void* ptr );
client_t* clientFind( const char* pseudo );
int clientPseudo( client_t* p, const char* pseudo );