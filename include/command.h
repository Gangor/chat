#pragma one
#include <pthread.h>
#include <unistd.h>
#include <chat.h>
#include <client.h>
#include <logger.h>
#include <server.h>
#include <struct.h>

void cmd_exit( client_t* client, char* token );
void cmd_help( client_t* client, char* token );
void cmd_kick( client_t* client, char* token );
void cmd_mp( client_t* client, char* token );
void cmd_nickname( client_t* client, char* token );
void cmd_shutdown( client_t* client, char* token );

int proc_command( client_t* client, const char* buff );

extern command_t cmds[];
extern size_t cmd_count;