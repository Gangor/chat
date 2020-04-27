#pragma once
#include <stdlib.h>
#include <string.h>
#include <server.h>
#include <struct.h>

#define BUFFER 256
#define UNUSED -1

void client_init( client_t* p, int fd );
void client_destroy( client_t* p );
int client_pseudo( client_t* p, char* pseudo );