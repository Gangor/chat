#pragma one
#include <pthread.h>
#include <unistd.h>
#include <chat.h>
#include <channel.h>
#include <client.h>
#include <logger.h>
#include <server.h>
#include <struct.h>

void cmd_add_channel( client_t* client, command_t* command, char* token );
void cmd_channels( client_t* client, command_t* command, char* token );
void cmd_exit( client_t* client, command_t* command, char* token );
void cmd_help( client_t* client, command_t* command, char* token );
void cmd_kick( client_t* client, command_t* command, char* token );
void cmd_mp( client_t* client, command_t* command, char* token );
void cmd_move_channel( client_t* client, command_t* command, char* token );
void cmd_nickname( client_t* client, command_t* command, char* token );
void cmd_remove_channel( client_t* client, command_t* command, char* token );
void cmd_shutdown( client_t* client, command_t* command, char* token );
void cmd_users( client_t* client, command_t* command, char* token );

int proc_command( client_t* client, const char* buff );

extern command_t cmds[];
extern size_t cmd_count;